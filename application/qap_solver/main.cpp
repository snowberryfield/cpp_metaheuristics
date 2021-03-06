/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/

#include <string>
#include <iostream>

#include "../printemps/utility/qap_utility.h"
#include "../printemps/utility/option_utility.h"

int main([[maybe_unused]] int argc, char *argv[]) {
    /**
     * If no argument is specified, the usage is displayed and the program
     * exits.
     */
    if (argv[1] == nullptr) {
        std::cout << "Usage: ./qap_solver.exe [-p OPTION_FILE_NAME] qap_file"
                  << std::endl;
        std::cout << std::endl;
        std::cout  //
            << "  -p OPTION_FILE_NAME: Specify option file name." << std::endl;
        exit(1);
    }

    /**
     * Parse the arguments.
     */
    std::string qap_file_name;
    std::string option_file_name;

    std::vector<std::string> args(argv, argv + argc);
    int                      i = 1;
    while (i < static_cast<int>(args.size())) {
        if (args[i] == "-p") {
            option_file_name = args[i + 1];
            i += 2;
        } else {
            qap_file_name = args[i];
            i++;
        }
    }

    /**
     * Read the specified QAPLIB file and convert to the model.
     */
    printemps::utility::QAPLIBReader qaplib_reader;
    auto &model = qaplib_reader.create_model_from_qaplib(qap_file_name);
    model.set_name(printemps::utility::base_name(qap_file_name));

    /**
     * If the option file is given, the option values specified in the file will
     * be used for the calculation. Otherwise, the default values will be used.
     */
    printemps::solver::Option option;
    if (!option_file_name.empty()) {
        option = printemps::utility::read_option(option_file_name);
    }
    option.is_enabled_binary_move       = false;
    option.is_enabled_integer_move      = false;
    option.is_enabled_user_defined_move = true;
    option.is_enabled_chain_move        = false;
    option.improvability_screening_mode = printemps::solver::Off;

    /**
     * Run the solver.
     */
    auto result = printemps::solver::solve(&model, option);

    /**
     * Print the result summary.
     */
    printemps::utility::print_info(
        "status: " + std::to_string(result.solution.is_feasible()),
        option.verbose >= printemps::solver::Verbose::Outer);

    printemps::utility::print_info(
        "objective: " + std::to_string(result.solution.objective()),
        option.verbose >= printemps::solver::Verbose::Outer);

    auto summary = model.export_summary();
    result.solution.write_json_by_name("incumbent.json", summary);
    result.solution.write_solution("incumbent.sol");
    result.status.write_json_by_name("status.json", summary);

    if (option.is_enabled_collect_historical_data) {
        result.solution_archive.write_solutions_json("feasible.json", summary);
    }

    return 0;
}
/*****************************************************************************/
// END
/*****************************************************************************/
