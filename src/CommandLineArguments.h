//
// Created by weigl on 10.10.15.
//
#pragma once


#include <vector>
#include <string>
#include "main.h"

#include <boost/program_options.hpp>

namespace po = boost::program_options;

enum class OperationMode {
        DETERMINISTIC = 'A',
        DETERMINISTIC_ITERATIVE = 'B',
        DETERMINISTIC_SHUFFLE = 'U',
        DETERMINISTIC_SUCCESSIVE = 'S',
        NDETERMINISTIC = 'N',
        SHARPSAT = 'T',
        ITERATIVE_SHARP = 'I'};

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

    uint64_t limit() const { return _limit; };
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
    uint64_t _limit;
    uint _max_models;
    //bool _statistics;
};
