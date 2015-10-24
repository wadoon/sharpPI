//
// Created by weigl on 10.10.15.
//

#include "CommandLineArguments.h"
#include <unistd.h>
#include <iostream>
#include "util.h"
#include "termbox.h"

void CommandLineArguments::initialize(int argc, char *argv[]) {
    int opt = 0;
    while ((opt = getopt(argc, argv, "hvc:i:o:s:m:")) >= 0) {
        switch (opt) {
            case 'c':
                _mode = (OperationMode) atoi(optarg);
                break;
            case 'v':
                _verbose = true;
                break;
            case 'h':
                _help = true;
                break;
            case 'i':
                _input_variables.push_back(optarg);
                break;
            case 'o':
                _output_variables.push_back(optarg);
                break;
            case 'm':
                _output_model_filename = optarg;
                break;
            case 's':
                _seed_variables.push_back(optarg);
                break;
        }
    }

    if (optind < argc)
        _input_filename = std::string(argv[optind]);
}

void CommandLineArguments::printUsage() {
    std::cout << "Usage: " << program_invocation_short_name << " [-h] [-v] [-i VARNAME] [-o VARNAME] DIMACS"
    << std::endl << "\n\n"
            "-i VARNAME    declares an input variable\n"
            "-o VARNAME    declares an output variable\n"
            "-s VARNAME    declares an seeds variable\n"
            "-v            set verbose outputs/debug messages\n"
            "-c MODE       counting modes\n"
            "               " << OPERATION_MODE_DETERMINISTIC << ": Determinstic (output->input relation)\n"
            "               " << OPERATION_MODE_DETERMINISTIC_ITERATIVE << ": Determinstic ITERATIVE (each output, limitation possible)\n"
            "               " << OPERATION_MODE_DETERMINISTIC_SHUFFLE << ": Determinstic SHUFFLE (find each input/output pair, limitation possible)\n"
            "               " << OPERATION_MODE_DETERMINISTIC_SUCCESSIVE << ": Determinstic SUCCESSIVE ()\n"
            "               " << OPERATION_MODE_NDETERMINISTIC << ": Non-Determinstic\n"

            "-h            prints this help\n\n\n"
    << endl
    << TB.create_box(COPYRIGHT, "COPYRIGHT") << std::endl;
}
