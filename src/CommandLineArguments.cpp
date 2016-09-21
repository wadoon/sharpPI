//
// Created by weigl on 10.10.15.
//

#include "CommandLineArguments.h"
#include <unistd.h>
#include <iostream>
#include <limits>
#include "util.h"

#include "sharpsat.h"
#include <boost/program_options/errors.hpp>


namespace po = boost::program_options;

typedef std::vector<std::string> StringList;

std::istream& operator>>(std::istream& in, OperationMode& mode)
{
    std::string token;
    in >> token;

    if(token == "sync") {
        mode = OperationMode::DSYNC;
    }
    else
        if(token == "bucket") {
            mode = OperationMode::DBUCKET;
        }
        else
            if(token == "bucketa") {
                mode = OperationMode::DBUCKETALL;
            }
            else
                if(token == "unguided") {
                    mode = OperationMode::DUNGUIDED;
                }
                else
                    if(token == "rand") {
                        mode = OperationMode::NRAND;
                    }
                    else {
                        if(token == "bucketsharp") {
                            mode = OperationMode::BUCKETSHARP;
                        }
                        else {
                            throw boost::program_options::validation_error
                                (boost::program_options::validation_error::invalid_option_value, "mode", token);
                        }
                    }
    //    std::cout << "Mode: " << static_cast<int>(mode) << endl;
    return in;
}

std::istream& operator>>(std::istream& in, SharpSatTool& tool) {
    std::string token;
    in >> token;

    tool = SharpSatTool::NONE;

    if(token == "dsharp") {
        tool = SharpSatTool::DSHARP;
    }

    if(token == "clasp") {
        tool = SharpSatTool::CLASP;
    }

    if(token == "sharpsat") {
        tool = SharpSatTool::SHARPSAT;
    }

    if(token == "approxmc") {
        tool = SharpSatTool::APPROXMC;
    }


    if(tool == SharpSatTool::NONE) {
        console() << "!!! ERROR: #SAT-p unknown !!!" << endl;
    }

    return in;
}



CommandLineArguments::CommandLineArguments()
    : _verbose(false), _help(false), //_output_model_filename("model.csv"),
          //_statistics(false),
      _mode(OperationMode::DBUCKET),
      _ssat_tool(SharpSatTool::NONE),
      general("General"),
      ssat("#SAT-p"),
      ndet("Non-determinism") {
    general.add_options()
        ("help,h", "produce help message")
        ("verbose,v", "verbose mode")
        //		  ("stat", "enable statistics")
        //("limit,l", po::value<uint64_t>(&_limit)->value_name("INT")->default_value( (uint64_t) -1 ), "limit rounds")
        ("mode,m", po::value<OperationMode>(&_mode)->value_name("MODE_ID"), "mode")
        //(",n", po::value<uint>(&_max_models)->value_name("INT")->default_value(-1), "maximum count on models that should be found")
        ("input,i", po::value<StringList>(&_input_variables)->value_name("VARIABLE"), "input")
        ("output,o", po::value<StringList>(&_output_variables)->value_name("VARIABLE"), "output")
        ("tolerance,t", po::value<double>(&_tolerance)->value_name("DOUBLE")->default_value(-INFINITY), "tolerance criterium")
        ("statistic", po::value<string>(&_stat_filename)->value_name("FILE")->default_value(""), "statistics filename, default: no statistic is written")
        //positional
        ("filename", po::value<string>(&_input_filename)->required()->composing(), "input filename");



    ssat.add_options()
        ("ssat-tool", po::value<SharpSatTool>(&_ssat_tool)->value_name("NAME"), "name of the  #SAT-p tool")
        ("ssat-command", po::value<string>(&_ssat_command)->value_name("command"), "command to call for #SAT-p.")
        ("ssat-indicator", po::value<string>(&_ssat_indicator)->value_name("STRING"), "prefix.");

    ndet.add_options()
        ("seeds,s", po::value<StringList>(&_seed_variables)->value_name("variable"), "seed")
        ("density", "density values for each output");

    general.add(ndet);
    general.add(ssat);

    p.add("filename", 1);
}

void CommandLineArguments::initialize(int argc, char *argv[]) {
    try {
        po::store(
                po::command_line_parser(argc, argv)
                        .positional(p)
                        .options(general)
                        .run(), vm);


        //_statistics = vm.count("stat");
        _verbose = vm.count("verbose");

        if (!vm.count("help")) { // do not signal errors if help is provided
            po::notify(vm);
        } else {
            printUsage();
            exit(0);
        }


        if(vm.count("ssat-tool")) {
            switch(_ssat_tool) {
            case SharpSatTool::APPROXMC:
                _ssat_command = COMMAND_APPROXMC;
                _ssat_indicator = INDICATOR_APPROXMC;
                break;
            case SharpSatTool::DSHARP:
                _ssat_command = COMMAND_DHSARP;
                _ssat_indicator = INDICATOR_DSHARP;
                break;
            case SharpSatTool::SHARPSAT:
                _ssat_command = COMMAND_SHARPSAT;
                _ssat_indicator = INDICATOR_SHARPSAT;
                break;
            case SharpSatTool::CLASP:
                _ssat_command = COMMAND_CLASP;
                _ssat_indicator = INDICATOR_CLASP;
                break;
            default:
                break;
            }

            console() << "Use: #SAT-p: '" << _ssat_command << "' with " << _ssat_indicator << endl;

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
}

void CommandLineArguments::printUsage() {
    std::cout << "Usage: " << program_invocation_short_name << " {options} input-file" << endl;
    std::cout << general
              << "\nOperation Modes:\n"
              << "\t unguided : Unguided Determinstic\n"
              << "\t bucket   : Bucket-Wise Deterministic\n"
              << "\t sync     : Sync Deterministic\n"
              << "\t rand     : Non-Deterministic\n"
              << "\t dsharp   : #SAT (dsharp) Determinstic\n"
              << "\n\nVersion: " << SHARP_PI_VERSION << " / " << SHARP_PI_DATE << endl << endl
              << "Copyright: " << endl
              << COPYRIGHT << endl;
}
