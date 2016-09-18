//
// Created by weigl on 10.10.15.
//
#pragma once


#include <vector>
#include <string>
#include "version.h"

#include <boost/program_options.hpp>

namespace po = boost::program_options;

enum class OperationMode {
    DBUCKETALL = 'A',
        DBUCKET = 'B',
        DUNGUIDED = 'U',
        DSYNC = 'S',
        NRAND = 'N',
        DSHARP = 'T',
        SHARPSAT = 'X',
        DISHARP = 'I'};

enum class SharpSatTool {
    NONE, DSHARP, CLASP, SHARPSAT, APPROXMC
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

    uint64_t limit() const { return _limit; }
    uint max_models() const { return _max_models; }
    void printUsage();

    double tolerance() {return _tolerance;}

    SharpSatTool& sharpsat_tool() { return _ssat_tool; }
    std::string& sharpsat_command() { return _ssat_command; }
    std::string& sharpsat_indicator() { return _ssat_indicator; }

private:
    po::options_description general, ndet, ssat;
    po::positional_options_description p;
    po::variables_map vm;

    int inputfile = 0;

    //---
    double _tolerance;
    std::vector<std::string> _input_variables, _output_variables, _seed_variables;
    OperationMode _mode;
    std::string _input_filename;
    bool _verbose;
    bool _help;
    std::string _stat_filename;
    uint64_t _limit;
    uint _max_models;
    //bool _statistics;

    SharpSatTool _ssat_tool;
    std::string _ssat_command;
    std::string _ssat_indicator;
};
