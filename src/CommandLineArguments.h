//
// Created by weigl on 10.10.15.
//

#ifndef SHARPPIC_COMMANDLINEARGUMENTS_H
#define SHARPPIC_COMMANDLINEARGUMENTS_H


#include <vector>
#include <string>
#include "main.h"

/*
       ┌─────────────────────────┬───────────────┬───────────────────────────┐
       │Interface                │ Attribute     │ Value                     │
       ├─────────────────────────┼───────────────┼───────────────────────────┤
       │getopt(), getopt_long(), │ Thread safety │ MT-Unsafe race:getopt env │
       │getopt_long_only()       │               │                           │
       └─────────────────────────┴───────────────┴───────────────────────────┘
*/

enum OperationMode {
    OPERATION_MODE_DETERMINISTIC, OPERATION_MODE_DETERMINISTIC_ITERATIVE, OPERATION_MODE_DETERMINISTIC_SHUFFLE,
    OPERATION_MODE_DETERMINISTIC_SUCCESSIVE, OPERATION_MODE_NDETERMINISTIC
};

class CommandLineArguments {
public:
    CommandLineArguments() : _verbose(false), _help(false), _output_model_filename("model.csv"), _mode(OPERATION_MODE_DETERMINISTIC) { }

    void initialize(int argc, char **argv);

    bool help() const { return _help; }

    bool verbose() const { return _verbose; }

    std::vector<std::string> &input_variables() { return _input_variables; }

    std::vector<std::string> &output_variables() { return _output_variables; }

    std::vector<std::string> &seed_variables() { return _seed_variables; }

    OperationMode mode() {return _mode;}

    std::string &input_filename() { return _input_filename; };

    std::string &output_model_filename() { return _output_model_filename; }

    void printUsage();

private:
    std::vector<std::string> _input_variables, _output_variables,_seed_variables;
    OperationMode _mode;
    std::string _input_filename;
    bool _verbose;
    bool _help;
    std::string _output_model_filename;
};


#endif //SHARPPIC_COMMANDLINEARGUMENTS_H
