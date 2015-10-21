//
// Created by weigl on 29.09.15.
//
#include <string>
#include <vector>
#include <fstream>
#include <zlib.h>
#include <set>

#ifndef MINISAT_CBMCPARSER_H
#define MINISAT_CBMCPARSER_H

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


class CbmcDimacsParser {
public:
    CbmcDimacsParser(string &input_file, vector<string> &input_names,
                     vector<string> &seed_names,
                     vector<string> &output_names) :
            input_file(input_file),
            inames(input_names),
            snames(seed_names),
            onames(output_names) { }


    vector<vector<int>> &clauses() { return _clauses; }

    void read();

    void sort_variables();

    vector<uint> &ovars() { return ovar; }

    vector<uint> &ivars() { return ivar; }
    vector<uint> &svars() { return svar; }


    int max_var;

    const vector<CBMCVariable> &output_variables() const {
        return _output_variables;
    }

    const vector<CBMCVariable> &seed_variables() const {
        return _seed_variables;
    }


    const vector<CBMCVariable> &input_variables() const {
        return _input_variables;
    }


private:
    vector<uint> ivar, ovar, svar;
    std::string input_file;

    std::vector<CBMCVariable> _output_variables;
    std::vector<CBMCVariable> _input_variables;
    std::vector<CBMCVariable> _seed_variables;

    vector<string> inames, onames, snames;

    vector<vector<int>> _clauses;

    vector<CBMCVariable> variables;

    std::vector<uint> collectVariablesInto(
            const vector<string> &names,
            vector<CBMCVariable> &collected_variables) const;
};

std::vector<CBMCVariable> parseCBMCFile(const char *filename);

#endif //MINISAT_CBMCPARSER_H
