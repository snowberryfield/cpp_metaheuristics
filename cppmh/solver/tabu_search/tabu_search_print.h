/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_SOLVER_TABU_SEARCH_TABU_SEARCH_PRINT_H__
#define CPPMH_SOLVER_TABU_SEARCH_TABU_SEARCH_PRINT_H__

namespace cppmh {
namespace solver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class IncumbentHolder;

namespace tabu_search {
/*****************************************************************************/
struct TabuSearchMoveScore;

/*****************************************************************************/
inline void print_table_header(const bool a_IS_ENABLED_PRINT) {
    if (!a_IS_ENABLED_PRINT) {
        return;
    }

    utility::print(
        "---------+------------------------+----------------------+-----------"
        "-----------",
        true);
    utility::print(
        "Iteration| Number of Neighborhoods|   Current Solution   |"
        "  Incumbent Solution ",
        true);
    utility::print(
        "         |  All Feas. Perm. Impr. |   Aug.Obj.(Penalty)  | "
        "  Aug.Obj.  Feas.Obj ",
        true);
    utility::print(
        "---------+------------------------+----------------------+-----------"
        "-----------",
        true);
}
/*****************************************************************************/
template <class T_Variable, class T_Expression>
inline void print_table_initial(
    const model::Model<T_Variable, T_Expression> *   a_MODEL,
    const model::SolutionScore &                     a_SOLUTION_SCORE,
    const IncumbentHolder<T_Variable, T_Expression> &a_INCUMBENT_HOLDER,
    const bool                                       a_IS_ENABLED_PRINT) {
    if (!a_IS_ENABLED_PRINT) {
        return;
    }

    std::printf(
        " INITIAL |    -     -     -     - | %9.2e(%9.2e) | %9.2e  %9.2e\n",
        a_SOLUTION_SCORE.local_augmented_objective * a_MODEL->sign(),
        a_SOLUTION_SCORE.local_penalty,
        a_INCUMBENT_HOLDER.global_augmented_incumbent_objective() *
            a_MODEL->sign(),
        a_INCUMBENT_HOLDER.feasible_incumbent_objective() * a_MODEL->sign());
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
inline void print_table_body(
    const model::Model<T_Variable, T_Expression> *a_MODEL,                //
    const int                                     a_ITERATION,            //
    const int                   a_NUMBER_OF_ALL_NEIGHBORHOODS,            //
    const int                   a_NUMBER_OF_FEASIBLE_NEIGHBORHOODS,       //
    const int                   a_NUMBER_OF_PERMISSIBLE_NEIGHBORHOOD,     //
    const int                   a_NUMBER_OF_IMPROVABLE_NEIGHBORHOOD,      //
    const model::SolutionScore &a_CURRENT_SOLUTION_SCORE,                 //
    const int                   a_STATUS,                                 //
    const IncumbentHolder<T_Variable, T_Expression> &a_INCUMBENT_HOLDER,  //
    const bool                                       a_IS_ENABLED_PRINT) {
    if (!a_IS_ENABLED_PRINT) {
        return;
    }

    char mark_current                    = ' ';
    char mark_global_augmented_incumbent = ' ';
    char mark_feasible_incumbent         = ' ';

    if (a_STATUS &
        IncumbentHolderConstant::STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
        mark_current                    = '!';
        mark_global_augmented_incumbent = '!';
    }

    if (a_STATUS & IncumbentHolderConstant::STATUS_FEASIBLE_INCUMBENT_UPDATE) {
        mark_current            = '*';
        mark_feasible_incumbent = '*';
    }

    std::printf(
        "%8d |%5d %5d %5d %5d |%c%9.2e(%9.2e) |%c%9.2e %c%9.2e\n",
        a_ITERATION,                           //
        a_NUMBER_OF_ALL_NEIGHBORHOODS,         //
        a_NUMBER_OF_FEASIBLE_NEIGHBORHOODS,    //
        a_NUMBER_OF_PERMISSIBLE_NEIGHBORHOOD,  //
        a_NUMBER_OF_IMPROVABLE_NEIGHBORHOOD,   //
        mark_current,                          //
        a_CURRENT_SOLUTION_SCORE.local_augmented_objective *
            a_MODEL->sign(),                     //
        a_CURRENT_SOLUTION_SCORE.local_penalty,  //
        mark_global_augmented_incumbent,         //
        a_INCUMBENT_HOLDER.global_augmented_incumbent_objective() *
            a_MODEL->sign(),      //
        mark_feasible_incumbent,  //
        a_INCUMBENT_HOLDER.feasible_incumbent_objective() * a_MODEL->sign());
}
/*****************************************************************************/
inline void print_table_footer(const bool a_IS_ENABLED_PRINT) {
    if (!a_IS_ENABLED_PRINT) {
        return;
    }

    utility::print(
        "---------+------------------------+----------------------+-----------"
        "-----------");
}
}  // namespace tabu_search
}  // namespace solver
}  // namespace cppmh

#endif
/*****************************************************************************/
// END
/*****************************************************************************/