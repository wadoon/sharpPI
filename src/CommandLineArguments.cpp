//
// Created by weigl on 10.10.15.
//

#include "CommandLineArguments.h"
#include <unistd.h>
#include <iostream>
#include "util.h"
#include "termbox.h"

namespace po = boost::program_options;

typedef std::vector<std::string> StringList;

CommandLineArguments::CommandLineArguments()
        : _verbose(false), _help(false), _output_model_filename("model.csv"),
          _statistics(false),
          _mode(OPERATION_MODE_DETERMINISTIC),

		  general("General"),
		  ndet("Non-determinism") {

			  general.add_options()
				  ("help,h", "produce help message")
				  ("verbose,v", "verbose mode")
				  ("stat", "enable statistics")
				  ("limit,l", po::value<uint>(&_limit)->value_name("INT")->default_value(10), "limit rounds")
				  ("mode,m", po::value<uint>(&_mode)->value_name("INT"), "mode")
				  (",n", po::value<uint>(&_max_models)->value_name("INT"), "maximum count on models that should be found")
				  ("input,i", po::value<StringList>(&_input_variables)->value_name("VARIABLE"), "input")
				  ("output,o", po::value<StringList>(&_output_variables)->value_name("VARIABLE"), "output")
				  //positional
				  ("filename", po::value<string>(&_input_filename)->required()->composing(), "input filename");


			  ndet.add_options()("s", po::value<StringList>(&_seed_variables)->value_name("variable"), "seed");
			  ndet.add_options()("density", "density values for each output");

			  general.add(ndet);

			  p.add("filename", 1);
		  }

void CommandLineArguments::initialize(int argc, char *argv[]) {
    try {
        po::store(
                po::command_line_parser(argc, argv)
                        .positional(p)
                        .options(general)
                        .run(), vm);


        _statistics = vm.count("stat");
        _verbose = vm.count("verbose");

        if (!vm.count("help")) { // do not signal errors if help is provided
            po::notify(vm);
        } else {
            printUsage();
            exit(0);
        }
    }
    catch (po::required_option &e) {
        std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
        exit(1);
    }
    catch (po::error &e) {
        std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
        exit(1);
    }
/*
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
        */
}

void CommandLineArguments::printUsage() {
    std::cout << "Usage: " << program_invocation_short_name << " {options} input-file" << endl;
    std::cout <<
    general
    << "\nOperation Modes:\n"
    << "\t" << OPERATION_MODE_DETERMINISTIC
    << ": Determinstic (output->input relation)\n"
    << "\t" << OPERATION_MODE_DETERMINISTIC_ITERATIVE
    << ": Determinstic ITERATIVE (each output, limitation possible)\n"
    << "\t" << OPERATION_MODE_DETERMINISTIC_SHUFFLE
    << ": Determinstic SHUFFLE (find each input/output pair, limitation possible)\n"
    << "\t" << OPERATION_MODE_DETERMINISTIC_SUCCESSIVE << ": Determinstic SUCCESSIVE ()\n"
    << "\t" << OPERATION_MODE_NDETERMINISTIC << ": Non-Determinstic\n\n"
    << "\t" << OPERATION_MODE_SHARPSAT << ": Normal #SAT based on `cr` entries\n\n"


    << "\n\nVersion: " << SHARP_PI_VERSION << " / " << SHARP_PI_DATE << endl << endl
    << TB.create_box(COPYRIGHT, "COPYRIGHT") << std::endl;
    /*
"-i VARNAME    declares an input variable\n"
    "-o VARNAME    declares an output variable\n"
    "-s VARNAME    declares an seeds variable\n"
    "-v            set verbose outputs/debug messages\n"
    "-c MODE       counting modes\n"
    "               " << OPERATION_MODE_DETERMINISTIC << ": Determinstic (output->input relation)\n"
    "               " << OPERATION_MODE_DETERMINISTIC_ITERATIVE <<
": Determinstic ITERATIVE (each output, limitation possible)\n"
    "               " << OPERATION_MODE_DETERMINISTIC_SHUFFLE <<
": Determinstic SHUFFLE (find each input/output pair, limitation possible)\n"
    "               " << OPERATION_MODE_DETERMINISTIC_SUCCESSIVE << ": Determinstic SUCCESSIVE ()\n"
    "               " << OPERATION_MODE_NDETERMINISTIC << ": Non-Determinstic\n"

    "-h            prints this help\n\n\n"
<< endl
<< TB.create_box(COPYRIGHT, "COPYRIGHT") << std::endl;*/
}
