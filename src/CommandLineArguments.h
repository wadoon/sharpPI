//
// Created by weigl on 10.10.15.
//
#pragma once


#include <vector>
#include <string>
#include "main.h"

#include <boost/program_options.hpp>

namespace po = boost::program_options;

enum OperationMode {
    OPERATION_MODE_DETERMINISTIC = 1,
    OPERATION_MODE_DETERMINISTIC_ITERATIVE = 2,
    OPERATION_MODE_DETERMINISTIC_SHUFFLE = 3,
    OPERATION_MODE_DETERMINISTIC_SUCCESSIVE = 4,
    OPERATION_MODE_NDETERMINISTIC = 5,
    OPERATION_MODE_SHARPSAT = 6,
    OPERATION_MODE_DETERMINISTIC_ITERATIVE_SHARP = 7
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

    const std::string &statistic_filename()  const { return _stat_filename; }
    const bool has_statistic() const { return statistic_filename().size() > 0;}

    uint limit() const { return _limit; };
    uint max_models() const { return _max_models; };
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
    std::string _stat_filename;
    uint _limit;
    uint _max_models;
    //bool _statistics;
};
