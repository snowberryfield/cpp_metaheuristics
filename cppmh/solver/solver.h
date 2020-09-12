/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_SOLVER_H__
#define CPPMH_SOLVER_H__

#include <iostream>
#include <cstdlib>
#include <random>

#include "../model/model.h"
#include "../utility/utility.h"

#include "incumbent_holder.h"
#include "option.h"
#include "status.h"
#include "history.h"
#include "result.h"
#include "tabu_search/tabu_search.h"
#include "local_search/local_search.h"
#include "lagrange_dual/lagrange_dual.h"

namespace cppmh {
namespace solver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
Result<T_Variable, T_Expression> solve(
    model::Model<T_Variable, T_Expression>* a_model) {
    Option option;
    return solve(a_model, option);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
Result<T_Variable, T_Expression> solve(
    model::Model<T_Variable, T_Expression>* a_model,  //
    const Option&                           a_OPTION) {
    if (a_model->is_solved()) {
        throw std::logic_error(utility::format_error_location(
            __FILE__, __LINE__, __func__,
            "This model has already been solved."));
    } else {
        a_model->set_is_solved(true);
    }
    /**
     * Define type aliases.
     */
    using Model_T           = model::Model<T_Variable, T_Expression>;
    using Solution_T        = model::Solution<T_Variable, T_Expression>;
    using IncumbentHolder_T = IncumbentHolder<T_Variable, T_Expression>;
    /**
     * Start to measure computational time.
     */
    utility::TimeKeeper time_keeper;

    /**
     * Copy arguments as local variables.
     */
    Model_T* model         = a_model;
    Option   master_option = a_OPTION;

    /**
     * Set default target objective value if it is not defined by the user.
     * For minimization problems, the default target value is set as -1E100.
     * For maximization problems, the default target value is set as 1E100.
     * If there is no objective function definition, the default target value is
     * set as 0 and the algorithm will be terminated if a feasible solution is
     * found.
     */
    auto target_objective_changed_rate =
        master_option.target_objective_value /
            OptionConstant::DEFAULT_TARGET_OBJECTIVE -
        1.0;

    if (fabs(target_objective_changed_rate) > constant::EPSILON) {
        master_option.target_objective_value *= model->sign();
    }

    if (fabs(target_objective_changed_rate) < constant::EPSILON) {
        if (!model->is_defined_objective()) {
            master_option.target_objective_value = 0.0;
        }
    }

    /**
     * Print the option value.
     */
    if (master_option.verbose >= Verbose::Outer) {
        master_option.print();
    }

    /**
     * Setup the model. This includes the following processes.
     * - setup_variable_related_constraints()
     * - setup_unique_name()
     * - setup_is_linear()
     * - setup_variable_sensitivity()
     * - presolve()
     * - categorize_variables()
     * - categorize_constraints()
     * - extract_selections()
     * - setup_neighborhood()
     * - verify_and_correct_selection_variables_initial_values()
     * - verify_and_correct_binary_variables_initial_values()
     * - verify_and_correct_integer_variables_initial_values()
     * - setup_fixed_sensitivities()
     */
    model->setup(master_option.is_enabled_improvability_screening,
                 master_option.is_enabled_parallel_neighborhood_update,
                 master_option.is_enabled_presolve,
                 master_option.is_enabled_initial_value_correction,
                 master_option.is_enabled_aggregation_move,
                 master_option.is_enabled_precedence_move,
                 master_option.is_enabled_variable_bound_move,
                 master_option.is_enabled_exclusive_move,
                 master_option.is_enabled_user_defined_move,
                 master_option.is_enabled_chain_move,
                 master_option.selection_mode,
                 master_option.verbose >= Verbose::Warning);

    /**
     * Print the problem size.
     */
    if (master_option.verbose >= Verbose::Outer) {
        model->print_number_of_variables();
        model->print_number_of_constraints();
    }

    /**
     * Enables the default neighborhood moves.
     */
    if (master_option.is_enabled_binary_move) {
        model->neighborhood().enable_binary_move();
    }

    if (master_option.is_enabled_integer_move) {
        model->neighborhood().enable_integer_move();
    }

    if (master_option.is_enabled_user_defined_move) {
        model->neighborhood().enable_user_defined_move();
    }

    if (master_option.selection_mode != model::SelectionMode::None) {
        model->neighborhood().enable_selection_move();
    }

    /**
     * NOTE: Special neighborhood moves will be enabled when optimization
     * stagnates.
     */

    /**
     * Check whether there exist special neighborhood moves or not.
     */
    bool has_special_neighborhood_moves =
        (model->neighborhood().aggregation_moves().size() +
         model->neighborhood().precedence_moves().size() +
         model->neighborhood().variable_bound_moves().size() +
         model->neighborhood().exclusive_moves().size()) > 0;

    if (master_option.is_enabled_chain_move) {
        has_special_neighborhood_moves = true;
    }

    /**
     * Prepare a random generator.
     */
    std::mt19937 get_rand_mt(master_option.seed);

    utility::print_single_line(  //
        master_option.verbose >= Verbose::Outer);

    utility::print_message(  //
        "Optimization starts.", master_option.verbose >= Verbose::Outer);

    /**
     * Create local and global penalty coefficient for each constraint.
     */
    std::vector<model::ValueProxy<double>> penalty_coefficient_proxies =
        model->generate_constraint_parameter_proxies(
            master_option.initial_penalty_coefficient);

    auto global_penalty_coefficient_proxies = penalty_coefficient_proxies;
    auto local_penalty_coefficient_proxies  = penalty_coefficient_proxies;

    /**
     * Create memory which stores updating count for each decision variable.
     */
    Memory memory(model);

    /**
     * Prepare historical solutions pool.
     */
    model::PlainSolutionPool<T_Variable, T_Expression>  //
        solution_pool(master_option.historical_data_capacity,
                      model->is_minimization());

    /**
     * Compute the values of expressions, constraints, and the objective
     * function according to the initial solution.
     */
    model->update();

    Solution_T        current_solution = model->export_solution();
    IncumbentHolder_T incumbent_holder;

    model::SolutionScore current_solution_score =
        model->evaluate({},                                 //
                        local_penalty_coefficient_proxies,  //
                        global_penalty_coefficient_proxies);

    [[maybe_unused]] int update_status = incumbent_holder.try_update_incumbent(
        current_solution, current_solution_score);

    int number_of_lagrange_dual_iterations = 0;
    int number_of_local_search_iterations  = 0;
    int number_of_tabu_search_iterations   = 0;
    int number_of_tabu_search_loops        = 0;

    /**
     * Solve Lagrange dual to obtain a better initial solution ß(Optional).
     */
    if (master_option.is_enabled_lagrange_dual) {
        if (!model->is_linear()) {
            utility::print_warning(
                "Solving lagrange dual was skipped because the problem is "
                "nonlinear.",
                master_option.verbose >= Verbose::Warning);
        } else if (model->number_of_selection_variables() > 0) {
            utility::print_warning(
                "Solving lagrange dual was skipped because it does not "
                "applicable to selection variables.",
                master_option.verbose >= Verbose::Warning);
        } else {
            double elapsed_time = time_keeper.clock();

            /**
             *  Check the terminating condition.
             */
            if (elapsed_time > master_option.time_max) {
                utility::print_message(
                    "Outer loop was terminated because of time-over (" +
                        utility::to_string(elapsed_time, "%.3f") + "sec).",
                    master_option.verbose >= Verbose::Outer);
            }

            /**
             *  Prepare an option object for local search.
             */
            Option option                    = master_option;
            option.lagrange_dual.time_offset = elapsed_time;

            /**
             * Prepare the initial variable values.
             */
            std::vector<model::ValueProxy<T_Variable>>
                initial_variable_value_proxies =
                    current_solution.variable_value_proxies;

            /**
             * Run the lagrange dual search.
             */
            auto result =
                lagrange_dual::solve(model,                               //
                                     option,                              //
                                     local_penalty_coefficient_proxies,   //
                                     global_penalty_coefficient_proxies,  //
                                     initial_variable_value_proxies,      //
                                     incumbent_holder);                   //

            /**
             * Update the current solution.
             */
            current_solution =
                result.incumbent_holder.global_augmented_incumbent_solution();

            /**
             * Update the historical data.
             */
            if (master_option.is_enabled_collect_historical_data &&
                result.historical_feasible_solutions.size() > 0) {
                solution_pool.push(result.historical_feasible_solutions);
            }

            /**
             * Update the global augmented incumbent solution if it was
             * improved by the Lagrange dual search.
             */
            update_status = incumbent_holder.try_update_incumbent(
                result.incumbent_holder.global_augmented_incumbent_solution(),
                result.incumbent_holder.global_augmented_incumbent_score());

            /**
             * Update the feasible incumbent solution if it was improved by
             * the Lagrange dual search
             */
            if (result.incumbent_holder.is_found_feasible_solution()) {
                update_status = incumbent_holder.try_update_incumbent(
                    result.incumbent_holder.feasible_incumbent_solution(),
                    result.incumbent_holder.feasible_incumbent_score());
            }

            /**
             * Preserve the number of iterations for solving the Lagrange dual
             * problem.
             */
            number_of_lagrange_dual_iterations = result.number_of_iterations;

            /**
             * Measure the elapsed time to solve the Lagrange dual problem.
             */
            elapsed_time = time_keeper.clock();

            /**
             * Print the search summary.
             */
            utility::print_message(
                "Solving Lagrange dual was finished. Total elapsed time: " +
                    utility::to_string(elapsed_time, "%.3f") + "sec",
                master_option.verbose >= Verbose::Outer);

            utility::print_info(
                " - Global augmented incumbent objective: " +
                    utility::to_string(
                        incumbent_holder
                                .global_augmented_incumbent_objective() *
                            model->sign(),
                        "%.3f"),
                master_option.verbose >= Verbose::Outer);

            utility::print_info(
                " - Feasible incumbent objective: " +
                    utility::to_string(
                        incumbent_holder.feasible_incumbent_objective() *
                            model->sign(),
                        "%.3f"),
                master_option.verbose >= Verbose::Outer);
        }
    }

    /**
     * Run a local search to improve the initial solution (optional).
     */
    if (master_option.is_enabled_local_search) {
        double elapsed_time = time_keeper.clock();

        /**
         *  Check the terminating condition.
         */
        if (elapsed_time > master_option.time_max) {
            utility::print_message(
                "Outer loop was terminated because of time-over (" +
                    utility::to_string(elapsed_time, "%.3f") + "sec).",
                master_option.verbose >= Verbose::Outer);
        }

        /**
         *  Prepare an option object for local search.
         */
        Option option                   = master_option;
        option.local_search.time_offset = elapsed_time;

        /**
         * Prepare the initial variable values.
         */
        std::vector<model::ValueProxy<T_Variable>>
            initial_variable_value_proxies =
                current_solution.variable_value_proxies;

        /**
         * Run the local search.
         */
        auto result =
            local_search::solve(model,                               //
                                option,                              //
                                local_penalty_coefficient_proxies,   //
                                global_penalty_coefficient_proxies,  //
                                initial_variable_value_proxies,      //
                                incumbent_holder,                    //
                                memory);                             //

        /**
         * Update the current solution.
         */
        current_solution =
            result.incumbent_holder.global_augmented_incumbent_solution();

        /**
         * Update the historical data.
         */
        if (master_option.is_enabled_collect_historical_data &&
            result.historical_feasible_solutions.size() > 0) {
            solution_pool.push(result.historical_feasible_solutions);
        }

        /**
         * Update the global augmented incumbent solution if it was improved
         * by the local search.
         */
        update_status = incumbent_holder.try_update_incumbent(
            result.incumbent_holder.global_augmented_incumbent_solution(),
            result.incumbent_holder.global_augmented_incumbent_score());

        /**
         * Update the feasible incumbent solution if it was improved by the
         * local search.
         */
        if (result.incumbent_holder.is_found_feasible_solution()) {
            update_status = incumbent_holder.try_update_incumbent(
                result.incumbent_holder.feasible_incumbent_solution(),
                result.incumbent_holder.feasible_incumbent_score());
        }

        /**
         * Update the memory.
         */
        memory = result.memory;

        /**
         * Preserve the number of iterations for the local search.
         */
        number_of_local_search_iterations = result.number_of_iterations;

        /**
         * Measure the elapsed time for the local search.
         */
        elapsed_time = time_keeper.clock();

        /**
         * Print the search summary.
         */
        utility::print_message(
            "Local search was finished. Total elapsed time: " +
                utility::to_string(elapsed_time, "%.3f") + "sec",
            master_option.verbose >= Verbose::Outer);

        utility::print_info(
            " - Global augmented incumbent objective: " +
                utility::to_string(
                    incumbent_holder.global_augmented_incumbent_objective() *
                        model->sign(),
                    "%.3f"),
            master_option.verbose >= Verbose::Outer);

        utility::print_info(
            " - Feasible incumbent objective: " +
                utility::to_string(
                    incumbent_holder.feasible_incumbent_objective() *
                        model->sign(),
                    "%.3f"),
            master_option.verbose >= Verbose::Outer);
    }

    /**
     * Run tabu searches to find better solutions.
     */
    int iteration                           = 0;
    int not_update_count                    = 0;
    int next_number_of_initial_modification = 0;
    int next_inital_tabu_tenure = master_option.tabu_search.initial_tabu_tenure;
    bool   penalty_coefficient_reset_flag = false;
    double bias                           = memory.bias();

    /**
     * The integer variable next_iteration_max is used if the option
     * tabu_search.is_enabled_automatic_iteration_adjustment is set true.
     */
    int next_iteration_max = master_option.tabu_search.iteration_max;

    while (true) {
        /**
         *  Check the terminating condition.
         */
        double elapsed_time = time_keeper.clock();
        if (elapsed_time > master_option.time_max) {
            utility::print_message(
                "Outer loop was terminated because of time-over (" +
                    utility::to_string(elapsed_time, "%.3f") + "sec).",
                master_option.verbose >= Verbose::Outer);
            break;
        }

        if (iteration >= master_option.iteration_max) {
            utility::print_message(
                "Outer loop was terminated because of iteration limit (" +
                    utility::to_string(iteration, "%d") + " iterations).",
                master_option.verbose >= Verbose::Outer);
            break;
        }

        if (incumbent_holder.feasible_incumbent_objective() <=
            master_option.target_objective_value) {
            utility::print_message(
                "Outer loop was terminated because of feasible objective "
                "reaches the target limit (" +
                    utility::to_string(iteration, "%d") + " iterations).",
                master_option.verbose >= Verbose::Outer);
            break;
        }

        /**
         *  Prepare an option object for tabu search.
         */
        Option option = master_option;

        if (option.tabu_search.is_enabled_automatic_iteration_adjustment) {
            option.tabu_search.iteration_max = next_iteration_max;
        }

        option.tabu_search.time_offset = elapsed_time;
        option.tabu_search.seed += iteration;
        option.tabu_search.number_of_initial_modification =
            next_number_of_initial_modification;
        option.tabu_search.initial_tabu_tenure = next_inital_tabu_tenure;

        /**
         * Prepare the initial variable values.
         */
        std::vector<model::ValueProxy<T_Variable>>
            initial_variable_value_proxies =
                current_solution.variable_value_proxies;

        /**
         * Run the tabu search.
         */
        auto result = tabu_search::solve(model,  //
                                         option,
                                         local_penalty_coefficient_proxies,   //
                                         global_penalty_coefficient_proxies,  //
                                         initial_variable_value_proxies,      //
                                         incumbent_holder,                    //
                                         memory);

        /**
         * Update the current solution.
         */
        auto result_local_solution =
            result.incumbent_holder.local_augmented_incumbent_solution();
        auto result_global_solution =
            result.incumbent_holder.global_augmented_incumbent_solution();
        bool is_changed = false;

        switch (master_option.tabu_search.restart_mode) {
            case tabu_search::RestartMode::Global: {
                is_changed = (result_global_solution.variable_value_proxies !=
                              current_solution.variable_value_proxies);
                current_solution = result_global_solution;

                break;
            }
            case tabu_search::RestartMode::Local: {
                is_changed = (result_local_solution.variable_value_proxies !=
                              current_solution.variable_value_proxies);
                current_solution = result_local_solution;
                break;
            }
            default: {
                throw std::logic_error(utility::format_error_location(
                    __FILE__, __LINE__, __func__,
                    "The specified restart mode is invalid."));
            }
        }

        /**
         * Update the historical data.
         */
        if (master_option.is_enabled_collect_historical_data &&
            result.historical_feasible_solutions.size() > 0) {
            solution_pool.push(result.historical_feasible_solutions);
        }

        /**
         * Update the global augmented incumbent solution if it was improved
         * by the tabu search.
         */
        update_status = incumbent_holder.try_update_incumbent(
            result.incumbent_holder.global_augmented_incumbent_solution(),
            result.incumbent_holder.global_augmented_incumbent_score());

        if ((update_status && (IncumbentHolderConstant::
                                   STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE))) {
            not_update_count               = 0;
            penalty_coefficient_reset_flag = false;
        } else {
            not_update_count++;
            if (not_update_count ==
                master_option.penalty_coefficient_reset_count_threshold) {
                penalty_coefficient_reset_flag = true;
                not_update_count               = 0;
            } else {
                penalty_coefficient_reset_flag = false;
            }
        }

        /**
         * Update the feasible incumbent solution if it was improved by the
         * tabu search.
         */
        if (result.incumbent_holder.is_found_feasible_solution()) {
            update_status = incumbent_holder.try_update_incumbent(
                result.incumbent_holder.feasible_incumbent_solution(),
                result.incumbent_holder.feasible_incumbent_score());
        }

        /**
         * Update the memory.
         */
        memory = result.memory;

        /**
         * Update the local penalty coefficients.
         */
        double gap =
            incumbent_holder.global_augmented_incumbent_objective() -
            result.incumbent_holder.local_augmented_incumbent_objective();

        if (penalty_coefficient_reset_flag) {
            /**
             * If penalty_coefficient_reset_flag is true, the penalty
             * coefficients will be reset;
             */
            local_penalty_coefficient_proxies =
                global_penalty_coefficient_proxies;
        } else if (gap > constant::EPSILON &&
                   !result_local_solution.is_feasible) {
            /**
             * If the gap the (global augmented incumbent objective) - (the
             * local augmented incumbent objective obtained in the last tabu
             * search) is positive, tighten the local penalty coefficients.
             * The latter condition considers cancellation of significant
             * digits.
             */
            double total_penalty           = 0.0;
            double total_squared_violation = 0.0;
            for (const auto& proxy :
                 result_local_solution.violation_value_proxies) {
                for (const auto& element : proxy.flat_indexed_values()) {
                    total_penalty += element;
                    total_squared_violation += element * element;
                }
            }

            const double balance =
                master_option.penalty_coefficient_updating_balance;

            for (auto&& proxy : local_penalty_coefficient_proxies) {
                int  id = proxy.id();
                auto violation_values =
                    result_local_solution.violation_value_proxies[id]
                        .flat_indexed_values();

                int flat_index = 0;
                for (auto&& element : proxy.flat_indexed_values()) {
                    double delta_penalty_constant =
                        std::max(0.0, gap) / total_penalty;
                    double delta_penalty_proportional =
                        std::max(0.0, gap) / total_squared_violation *
                        violation_values[flat_index];

                    element +=
                        master_option.penalty_coefficient_tightening_rate *
                        (balance * delta_penalty_constant +
                         (1.0 - balance) * delta_penalty_proportional);

                    flat_index++;
                }

                if (master_option.is_enabled_grouping_penalty_coefficient) {
                    double max_penalty =
                        utility::max(proxy.flat_indexed_values());
                    for (auto&& element : proxy.flat_indexed_values()) {
                        element = max_penalty;
                    }
                }

                /**
                 * Penalty coefficients are bounded by the initial penalty
                 * coefficient specified in option.
                 */
                for (auto&& element : proxy.flat_indexed_values()) {
                    element = std::min(
                        element, master_option.initial_penalty_coefficient);
                }
            }
        } else {
            /**
             * Otherwise relax the local penalty coefficients of which
             * corresponding constraints are satisfied.
             */
            for (auto&& proxy : local_penalty_coefficient_proxies) {
                int  id = proxy.id();
                auto violation_values =
                    result_local_solution.violation_value_proxies[id]
                        .flat_indexed_values();

                int flat_index = 0;
                for (auto&& element : proxy.flat_indexed_values()) {
                    if (violation_values[flat_index] < constant::EPSILON) {
                        element *=
                            master_option.penalty_coefficient_relaxing_rate;
                    }

                    flat_index++;
                }
            }
        }

        /**
         * Update the initial tabu tenure for the next loop.
         */
        auto previous_bias = bias;
        bias               = memory.bias();
        if (master_option.tabu_search
                .is_enabled_automatic_tabu_tenure_adjustment) {
            if (result.total_update_status &
                IncumbentHolderConstant::
                    STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
                next_inital_tabu_tenure =
                    std::min(master_option.tabu_search.initial_tabu_tenure,
                             model->number_of_not_fixed_variables());
            } else if (bias > previous_bias) {
                next_inital_tabu_tenure =
                    std::min(option.tabu_search.initial_tabu_tenure + 1,
                             model->number_of_not_fixed_variables());

            } else if (bias < previous_bias) {
                next_inital_tabu_tenure =
                    std::max(option.tabu_search.initial_tabu_tenure - 1, 1);
            }
        } else {
            next_inital_tabu_tenure =
                master_option.tabu_search.initial_tabu_tenure;
        }

        /**
         * Update the number of initial modification for the next loop.
         */
        if (result.total_update_status &
            IncumbentHolderConstant::STATUS_FEASIBLE_INCUMBENT_UPDATE) {
            next_number_of_initial_modification = 0;
        } else if (result.total_update_status &
                   IncumbentHolderConstant::
                       STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
            next_number_of_initial_modification = 0;
        } else {
            if (master_option.tabu_search.is_enabled_initial_modification &&
                !is_changed) {
                int nominal_number_of_initial_modification  //
                    = static_cast<int>(
                        std::floor(master_option.tabu_search
                                       .initial_modification_fixed_rate *
                                   next_inital_tabu_tenure));

                int initial_modification_random_width = static_cast<int>(
                    option.tabu_search.initial_modification_randomize_rate *
                    nominal_number_of_initial_modification);

                int number_of_initial_modification =
                    nominal_number_of_initial_modification;
                if (initial_modification_random_width > 0) {
                    number_of_initial_modification +=
                        get_rand_mt() %
                            (2 * initial_modification_random_width) -
                        initial_modification_random_width;
                }

                next_number_of_initial_modification =
                    std::max(1, number_of_initial_modification);
            }
        }

        /**
         * Update the maximum number of iterations for the next loop.
         */
        if (master_option.tabu_search
                .is_enabled_automatic_iteration_adjustment &&
            !result.is_early_stopped) {
            int next_iteration_max_temp = 0;
            if (result.total_update_status &
                IncumbentHolderConstant::
                    STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
                next_iteration_max_temp  //
                    = static_cast<int>(ceil(
                        result.last_local_augmented_incumbent_update_iteration *
                        master_option.tabu_search.iteration_increase_rate));

            } else {
                next_iteration_max_temp  //
                    = static_cast<int>(ceil(
                        option.tabu_search.iteration_max *
                        master_option.tabu_search.iteration_increase_rate));
            }
            next_iteration_max =
                std::max(master_option.tabu_search.initial_tabu_tenure,
                         std::min(master_option.tabu_search.iteration_max,
                                  next_iteration_max_temp));
        }

        bool is_enabled_special_neighborhood_move  = false;
        bool is_disabled_special_neighborhood_move = false;

        if (result.total_update_status &
            IncumbentHolderConstant::STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
            /**
             * Disable the special neighborhood moves if the incumbent was
             * updated.
             */
            /// Aggregation
            if (master_option.is_enabled_aggregation_move) {
                if (model->neighborhood().is_enabled_aggregation_move()) {
                    model->neighborhood().disable_aggregation_move();
                    is_disabled_special_neighborhood_move = true;
                }
            }

            /// Precedence
            if (master_option.is_enabled_precedence_move) {
                if (model->neighborhood().is_enabled_precedence_move()) {
                    model->neighborhood().disable_precedence_move();
                    is_disabled_special_neighborhood_move = true;
                }
            }

            /// Variable Bound
            if (master_option.is_enabled_variable_bound_move) {
                if (model->neighborhood().is_enabled_variable_bound_move()) {
                    model->neighborhood().disable_variable_bound_move();
                    is_disabled_special_neighborhood_move = true;
                }
            }

            /// Exclusive
            if (master_option.is_enabled_exclusive_move) {
                if (model->neighborhood().is_enabled_exclusive_move()) {
                    model->neighborhood().disable_exclusive_move();
                    is_disabled_special_neighborhood_move = true;
                }
            }

            /// Chain
            if (master_option.is_enabled_chain_move) {
                if (model->neighborhood().is_enabled_chain_move()) {
                    model->neighborhood().disable_chain_move();
                    is_disabled_special_neighborhood_move = true;
                }
            }

        } else {
            /**
             * Enable the special neighborhood moves if the incumbent was not
             * updated.
             */
            if (!result.is_early_stopped &&
                (option.tabu_search.iteration_max ==
                 master_option.tabu_search.iteration_max)) {
                /// Aggregation
                if (master_option.is_enabled_aggregation_move) {
                    if (!model->neighborhood().is_enabled_aggregation_move()) {
                        model->neighborhood().enable_aggregation_move();
                        is_enabled_special_neighborhood_move = true;
                    }
                }

                /// Precedence
                if (master_option.is_enabled_precedence_move) {
                    if (!model->neighborhood().is_enabled_precedence_move()) {
                        model->neighborhood().enable_precedence_move();
                        is_enabled_special_neighborhood_move = true;
                    }
                }

                /// Variable Bound
                if (master_option.is_enabled_variable_bound_move) {
                    if (!model->neighborhood()
                             .is_enabled_variable_bound_move()) {
                        model->neighborhood().enable_variable_bound_move();
                        is_enabled_special_neighborhood_move = true;
                    }
                }

                /// Exclusive
                if (master_option.is_enabled_exclusive_move) {
                    if (!model->neighborhood().is_enabled_exclusive_move()) {
                        model->neighborhood().enable_exclusive_move();
                        is_enabled_special_neighborhood_move = true;
                    }
                }

                /// Chain
                if (master_option.is_enabled_chain_move) {
                    if (!model->neighborhood().is_enabled_chain_move()) {
                        model->neighborhood().enable_chain_move();
                        is_enabled_special_neighborhood_move = true;
                    }
                }
            }
        }

        /**
         * Preserve the number of iterations of the previous loop.
         */
        number_of_tabu_search_iterations += result.number_of_iterations;
        number_of_tabu_search_loops++;

        /**
         * Measure the elapsed time of the previous loop.
         */
        elapsed_time = time_keeper.clock();

        /**
         * Print the summary.
         */
        utility::print_message(
            "Tabu search loop (" + std::to_string(iteration + 1) + "/" +
                std::to_string(master_option.iteration_max) +
                ") was finished. Total elapsed time: " +
                utility::to_string(elapsed_time, "%.3f") + "sec",
            master_option.verbose >= Verbose::Outer);
        utility::print_info(
            " - Global augmented incumbent objective: " +
                utility::to_string(
                    incumbent_holder.global_augmented_incumbent_objective() *
                        model->sign(),
                    "%.3f"),
            master_option.verbose >= Verbose::Outer);
        utility::print_info(
            " - Feasible incumbent objective: " +
                utility::to_string(
                    incumbent_holder.feasible_incumbent_objective() *
                        model->sign(),
                    "%.3f"),
            master_option.verbose >= Verbose::Outer);

        /**
         * Print the optimization status of the previous tabu search loop.
         */
        if (result.total_update_status &
            IncumbentHolderConstant::STATUS_FEASIBLE_INCUMBENT_UPDATE) {
            utility::print_message("Feasible incumbent objective was updated. ",
                                   master_option.verbose >= Verbose::Outer);

        } else if (result.total_update_status &
                   IncumbentHolderConstant::
                       STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
            utility::print_message("Global incumbent objective was updated. ",
                                   master_option.verbose >= Verbose::Outer);
        } else {
            if (master_option.tabu_search.is_enabled_initial_modification &&
                !is_changed) {
                utility::print_message("Incumbent objective was not updated.",
                                       master_option.verbose >= Verbose::Outer);
            }
        }

        /**
         * Print message if the penalty coefficients were reset.
         */
        if (penalty_coefficient_reset_flag) {
            utility::print_message(
                "The penalty coefficients were reset due to search stagnation.",
                master_option.verbose >= Verbose::Outer);
        }

        /**
         * Print the initial tabu tenure for the next loop.
         */
        utility::print_message("The tabu tenure for the next loop was set to " +
                                   std::to_string(next_inital_tabu_tenure) +
                                   ".",
                               master_option.verbose >= Verbose::Outer);

        /**
         * Print the number of initial modification for the next loop.
         */
        if (next_number_of_initial_modification > 0) {
            utility::print_message(
                "For the initial " +
                    std::to_string(next_number_of_initial_modification) +
                    " iterations in the next loop, the solution will "
                    "be randomly updated to escape from the local minimum.",
                master_option.verbose >= Verbose::Outer);
        }

        /**
         * Print the number of iterations for the next loop.
         */
        if (master_option.tabu_search
                .is_enabled_automatic_iteration_adjustment) {
            utility::print_message(
                "The maximum number of iterations for the next loop was "
                "set to " +
                    std::to_string(next_iteration_max) + ".",
                master_option.verbose >= Verbose::Outer);
        }

        /**
         * Print a message of the special neighborhood moves
         * activation/deactivation.
         */
        if (is_disabled_special_neighborhood_move &&
            has_special_neighborhood_moves) {
            utility::print_message("Special neighborhood moves were disabled.",
                                   master_option.verbose >= Verbose::Outer);
        }

        if (is_enabled_special_neighborhood_move &&
            has_special_neighborhood_moves) {
            utility::print_message("Special neighborhood moves were enabled.",
                                   master_option.verbose >= Verbose::Outer);
        }

        model->callback();
        iteration++;
    }

    /**
     * If a feasible solution is found in optimization, the incumbent
     * solution is defined by the solution with the best objective function
     * value among the feasible solutions. If no feasible solution is found,
     * the incumbent solution is substituted by solution with the best
     * augmented solution which has smallest sum of the objective function
     * value and the penalty value.
     * */

    auto incumbent =
        incumbent_holder.is_found_feasible_solution()
            ? incumbent_holder.feasible_incumbent_solution()
            : incumbent_holder.global_augmented_incumbent_solution();

    /**
     * All value of the expressions and the constraints are updated forcibly
     * to take into account the cases they are disabled.
     */
    model->import_variable_values(incumbent.variable_value_proxies);
    model->update();
    incumbent = model->export_solution();

    auto named_solution = model->convert_to_named_solution(incumbent);

    /**
     * Export the final penalty coefficient values.
     */
    std::unordered_map<std::string, model::ValueProxy<double>>
        named_penalty_coefficients;

    int        constraint_proxies_size = model->constraint_proxies().size();
    const auto constraint_names        = model->constraint_names();
    for (auto i = 0; i < constraint_proxies_size; i++) {
        named_penalty_coefficients[constraint_names[i]] =
            local_penalty_coefficient_proxies[i];
    }

    /**
     * Export the final variable update counts.
     */
    std::unordered_map<std::string, model::ValueProxy<int>> named_update_counts;
    int         variable_proxies_size = model->variable_proxies().size();
    const auto& variable_names        = model->variable_names();

    const auto& update_counts = memory.update_counts();
    for (auto i = 0; i < variable_proxies_size; i++) {
        named_update_counts[variable_names[i]] = update_counts[i];
    }

    /**
     * Prepare the result object to return.
     */
    Result<T_Variable, T_Expression> result;
    result.solution = named_solution;

    result.status.model_summary              = model->export_summary();
    result.status.penalty_coefficients       = named_penalty_coefficients;
    result.status.update_counts              = named_update_counts;
    result.status.is_found_feasible_solution = named_solution.is_feasible();
    result.status.elapsed_time               = time_keeper.elapsed_time();
    result.status.number_of_lagrange_dual_iterations =
        number_of_lagrange_dual_iterations;
    result.status.number_of_local_search_iterations =
        number_of_local_search_iterations;
    result.status.number_of_tabu_search_iterations =
        number_of_tabu_search_iterations;
    result.status.number_of_tabu_search_loops = number_of_tabu_search_loops;

    result.history.model_summary      = model->export_summary();
    result.history.feasible_solutions = solution_pool.solutions();

    return result;
}
}  // namespace solver
}  // namespace cppmh

#endif
/*****************************************************************************/
// END
/*****************************************************************************/