//
// Created by weigl on 10.10.15.
//

#ifndef SHARPPIC_COMMANDLINEARGUMENTS_H
#define SHARPPIC_COMMANDLINEARGUMENTS_H


#include <vector>
#include <string>
#include "main.h"

#include <boost/program_options.hpp>

namespace po = boost::program_options;

enum OperationMode {
    OPERATION_MODE_DETERMINISTIC,
   	OPERATION_MODE_DETERMINISTIC_ITERATIVE,
   	OPERATION_MODE_DETERMINISTIC_SHUFFLE,
    OPERATION_MODE_DETERMINISTIC_SUCCESSIVE,
   	OPERATION_MODE_NDETERMINISTIC,
    OPERATION_MODE_SHARPSAT,
	OPERATION_MODE_DETERMINISTIC_ITERATIVE_SHARP
};

class CommandLineArguments {
public:
    CommandLineArguments();

    void initialize(int argc, char **argv);

    bool help() const { return _help; }

    bool verbose() const { return _verbose; }

    std::vector<std::string> &input_variables() { return _input_variables; }

    std::vector<std::string> &output_variables()  { return _output_variables; }

    std::vector<std::string> &seed_variables()  { return _seed_variables; }

    OperationMode mode() const { return (OperationMode) _mode; }

    std::string &input_filename()  { return _input_filename; };

    const std::string &output_model_filename()  { return _output_model_filename; }

    uint limit() const { return _limit; };
    uint max_models() const { return _max_models; };

    bool statistics() const { return _statistics; }

    void printUsage();

private:
    po::options_description general, ndet;
    po::positional_options_description p;
    po::variables_map vm;

    int inputfile = 0;

    //---

    std::vector<std::string> _input_variables, _output_variables, _seed_variables;
    /*OperationMode*/ uint _mode;
    std::string _input_filename;
    bool _verbose;
    bool _help;
    std::string _output_model_filename;
    uint _limit;
    uint _max_models;
    bool _statistics;
};


#endif //SHARPPIC_COMMANDLINEARGUMENTS_H
