//
// Created by weigl on 29.09.15.
//
#pragma once

#include "dimacsparser.h"

using namespace std;

struct CBMCVariable {
    std::string function_name;
    std::string unknown_field;
    std::string variable_name;
    std::string thread_name;
    int rec_depth;
    int time;
    std::set<uint> variables;
    std::vector<uint> positions;
};


class CbmcDimacsParser : public DimacsParser {
public:
    CbmcDimacsParser(string &input_file, vector<string> &input_names,
                     vector<string> &seed_names,
                     vector<string> &output_names) :
            DimacsParser(input_file),
            inames(input_names),
            snames(seed_names),
            onames(output_names) { }



    virtual void read();

    void sort_variables();

    vector<uint> &ovars() { return ovar; }

    vector<uint> &ivars() { return ivar; }

    vector<uint> &svars() { return svar; }


    const vector<CBMCVariable> &output_variables() const {
        return _output_variables;
    }

    const vector<CBMCVariable> &seed_variables() const {
        return _seed_variables;
    }


    const vector<CBMCVariable> &input_variables() const {
        return _input_variables;
    }


    bool handle_comment(const std::string &line) override;

private:
    vector<uint> ivar, ovar, svar;

    std::vector<CBMCVariable> _output_variables;
    std::vector<CBMCVariable> _input_variables;
    std::vector<CBMCVariable> _seed_variables;

    vector<string> inames, onames, snames;


    vector<CBMCVariable> variables;

    std::vector<uint> collectVariablesInto(
            const vector<string> &names,
            vector<CBMCVariable> &collected_variables) const;
};

std::vector<CBMCVariable> parseCBMCFile(const char *filename);
