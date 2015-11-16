//
// Created by weigl on 16.11.15.
//

#include <string>
#include <vector>
#include <fstream>
#include <zlib.h>
#include <set>

using namespace std;

#ifndef SHARPPIC_DIMACSPARSER_H
#define SHARPPIC_DIMACSPARSER_H

class DimacsParser {
public:
    DimacsParser(string &input_file) : input_file(input_file) { }

    virtual ~DimacsParser() { };

    virtual void read();

    vector<uint> &projection_corpus() { return cr; }

    int max_variable() { return max_var; }


    vector<vector<int>> &clauses() { return _clauses; }

protected:


    virtual bool handle_line(const std::string &line);

    virtual bool handle_comment(const std::string &line) { }

    virtual bool handle_rembered_variables(const std::string &line);

    virtual bool handle_clause(const string &line, vector<int> &clause);

    int max_var;

    std::string input_file;
    vector<vector<int>> _clauses;
    vector<uint> cr;

};

#endif //SHARPPIC_DIMACSPARSER_H
