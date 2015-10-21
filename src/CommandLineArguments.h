//
// Created by weigl on 10.10.15.
//

#ifndef SHARPPIC_COMMANDLINEARGUMENTS_H
#define SHARPPIC_COMMANDLINEARGUMENTS_H


#include <vector>
#include <string>

/*┌─────────────────────────┬───────────────┬───────────────────────────┐
       │Interface                │ Attribute     │ Value                     │
       ├─────────────────────────┼───────────────┼───────────────────────────┤
       │getopt(), getopt_long(), │ Thread safety │ MT-Unsafe race:getopt env │
       │getopt_long_only()       │               │                           │
       └─────────────────────────┴───────────────┴───────────────────────────┘
*/

const std::vector<std::string> COPYRIGHT = {
        "(c) 2015 Alexander Weigl <weigl@kit.edu>",
        "      Application-Oriented Formal Verification",
        "      Institute for Theoretical Informatics",
        "       Karlsruhe Institute for Technology (KIT)"};

class CommandLineArguments {
public:
    CommandLineArguments() : _verbose(false), _help(false), _output_model_filename("model.csv") { }

    void initialize(int argc, char **argv);

    bool help() { return _help; }

    bool verbose() { return _verbose; }

    std::vector<std::string> &input_variables() { return _input_variables; }

    std::vector<std::string> &output_variables() { return _output_variables; }

    std::vector<std::string> &seed_variables() { return _seed_variables; }


    std::string &input_filename() { return _input_filename; };

    std::string &output_model_filename() { return _output_model_filename; }

    void printUsage();

private:
    std::vector<std::string> _input_variables, _output_variables,_seed_variables;
    std::string _input_filename;
    bool _verbose;
    bool _help;
    std::string _output_model_filename;
};


#endif //SHARPPIC_COMMANDLINEARGUMENTS_H
