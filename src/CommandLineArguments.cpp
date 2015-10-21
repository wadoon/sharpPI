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
    while ((opt = getopt(argc, argv, "hvci:o:s:m:")) >= 0) {
        switch (opt) {
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

    _input_filename = std::string(argv[optind]);
}

void CommandLineArguments::printUsage() {
    std::cout << "Usage: " << program_invocation_short_name << " [-h] [-v] [-i VARNAME] [-o VARNAME] DIMACS"
    << std::endl << "\n\n"
            "-i VARNAME    declares an input variable\n"
            "-o VARNAME    declares an output variable\n"
            "-v            set verbose outputs/debug messages\n"
            "-h            prints this help\n\n\n"
    << TB.create_box(COPYRIGHT, "COPYRIGHT") << std::endl;
}
