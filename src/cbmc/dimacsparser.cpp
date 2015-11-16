//
// Created by weigl on 16.11.15.
//

#include <iostream>
#include <err.h>
#include <sstream>

#include "dimacsparser.h"


void DimacsParser::read() {
    ifstream input(input_file);
    max_var = 0;

    if (!input.is_open()) {
        err(1, "%s", input_file.c_str());
        wcout << "Could not open file" << endl;
    }

    for (string line; getline(input, line);) {
        // skip empty lines or non comments
        if (line.length() == 0) {
            continue;
        }

        handle_line(line);
    }
}

bool DimacsParser::handle_line(const std::string &line) {
    if (line[0] == 'p')
        return true;

    if (line[0] == 'c') {
        if (line[1] == 'r') {
            return handle_rembered_variables(line);
        }
        else {
            return handle_comment(line);
        }
    } else {
        vector<int> clause;
        handle_clause(line, clause);
        this->_clauses.push_back(move(clause));
        return true;
    }
}

bool DimacsParser::handle_rembered_variables(const std::string &line) {
    vector<int> _tmp;
    handle_clause(line.substr(2), _tmp);
    for (auto v : _tmp) {
        cr.push_back((unsigned int) abs(v));
    }
    return true;

}

 bool DimacsParser::handle_clause(const string &line, vector<int> &clause) {
    stringstream in(line);
    int x = 0;
    while (!in.eof()) {
        in >> x;
        if (x == 0)
            break;

        int v = abs(x);

        max_var = max(v, max_var);

        clause.push_back(x);
    }
    return true;
}
