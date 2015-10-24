//
// Created by weigl on 29.09.15.
//

#include "cbmcparser.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <map>
#include <err.h>

using namespace std;

const string DIGIT_REGEX = R"(-?(\d+))";

const std::string REGEX_NORMAL =
        R"(^c (c::)?(.*?)::(.*?)::(.*?)!(\d*)@(\d*)#(\d*) (.*)$)";

const string REGEX_SHORT = // boolean variables!!!!
        R"(^c (\d*) (c::)?(.*?)::(.*?)::(.*?)!(\d*)@(\d*)#(\d*)$)";

void extractNormalLine(vector<CBMCVariable> &variables, const smatch &match);

void collectLastUpdate(const vector<CBMCVariable> &vars, const string &name, vector<CBMCVariable> &variables_ids) {
    int max_time = -1;
    CBMCVariable max_var;

    for (const auto &v : vars) {
        if (v.variable_name == name && v.time > max_time) {
            max_time = v.time;
            max_var = v;
        }
    }
    if (max_time != -1) {
        variables_ids.push_back(max_var);
    }
}

void collectAllUpdates(const vector<CBMCVariable> &vars, const string &name, vector<CBMCVariable> &variables_ids) {
    int min_frame_time = 10000000; // big integer inserted here

    for (const auto &v : vars) {
        if (v.variable_name == name && v.time < min_frame_time)
            min_frame_time = v.time;
    }

    for (const auto &v : vars) {
        //exclude the initial update
        if (v.variable_name == name && v.time != min_frame_time) {
            variables_ids.push_back(v);
        }
    }
}

void collectVariables(vector<CBMCVariable> variables,
                      const vector<string> &names,
                      vector<CBMCVariable> &vars) {
    for (const auto &name : names) {
        auto select_all = name.at(0) == '@';
        if (select_all) {
            string n(name, 1);
            collectAllUpdates(variables, n, vars);
        }
        else {
            collectLastUpdate(variables, name, vars);
        }
    }
}


vector<uint> parseLiterals(const string lit) {
    vector<uint> literals;

    istringstream buffer(lit);
    string token, s_false = "FALSE", s_true = "TRUE";

    while (getline(buffer, token, ' ')) {
        if (token == s_false) {
            literals.push_back(0);

        } else

        if (token == s_true) {
            literals.push_back(-1);
        } else {
//            std::cout << token << std::endl;
            literals.push_back(stoi(token));
        }
    }

    return literals;
}


std::set<uint> get_literals(const vector<uint> &pos) {
    std::set<uint> a;
    for (const auto p: pos) {
        if (p != 0 && p != -1)
            a.insert(p);
    }
    return a;
}

CBMCVariable extractNormalLine(const smatch &match) {
    CBMCVariable variable;

    variable.function_name = match[2].str();
    variable.unknown_field = match[3].str();
    variable.variable_name = match[4].str();
    variable.thread_name = match[5].str();
    variable.rec_depth = stoi(match[6].str());
    variable.time = stoi(match[7].str());

    variable.positions = parseLiterals(match[8].str());
    variable.variables = get_literals(variable.positions);

#ifdef DEBUG
    cout << variable.variable_name << "@" << variable.time << "found" << endl;
#endif

    return variable; //variables.push_back(variable);
}


CBMCVariable extractBooleanLine(const smatch &match) {
    CBMCVariable variable;

    variable.function_name = match[3].str();
    variable.unknown_field = match[4].str();
    variable.variable_name = match[5].str();
    variable.thread_name = match[6].str();
    variable.rec_depth = stoi(match[7].str());
    variable.time = stoi(match[8].str());

    vector<uint> literals;
    literals.push_back(stoi(match[1].str()));
    variable.variables = get_literals(literals);
    variable.positions = literals;

#ifdef DEBUG
    cout << variable.variable_name << "@" << variable.time << "found" << endl;
#endif

    return variable; //variables.push_back(variable);
}

vector<CBMCVariable> parseCBMCFile(const char *filename) {
    vector<CBMCVariable> variables;

    ifstream input(filename);

    regex rgx1(REGEX_NORMAL);
    regex rgx2(REGEX_SHORT);
    smatch match;

    for (string line; getline(input, line);) {

        // skip empty lines or non comments
        if (line.length() == 0 || line.at(0) != 'c') {
            continue;
        }

        if (regex_match(line, match, rgx1)) {
            variables.push_back(
                    extractNormalLine(match));
        }

        if (regex_match(line, match, rgx2)) {
            variables.push_back(extractBooleanLine(match));

        }

    }

    return variables;
}


void CbmcDimacsParser::read() {
    ifstream input(input_file);
    max_var = 0;

    if (!input.is_open()) {
        err(1, "%s", input_file.c_str());
        wcout << "Could not open file" << endl;
    }

    regex rgx1(REGEX_NORMAL);
    regex rgx2(REGEX_SHORT);
    smatch match;


    for (string line; getline(input, line);) {
        // skip empty lines or non comments
        if (line.length() == 0) {
            continue;
        }

        if (line[0] == 'p')
            continue;

        if (line[0] == 'c') {
            if (regex_match(line, match, rgx1)) {
                variables.push_back(
                        extractNormalLine(match));
            }

            if (regex_match(line, match, rgx2)) {
                variables.push_back(extractBooleanLine(match));

            }
        } else {
            vector<int> clause;
            stringstream in(line);
            int x = 0;
            while (!in.eof()) {
                in >> x;
                if (x == 0)
                    break;

                int v = abs(x);

                this->max_var = max(v, max_var);

                clause.push_back(x);
            }
            this->_clauses.push_back(move(clause));
        }
    }

    sort_variables();
}

std::vector<uint> CbmcDimacsParser::collectVariablesInto(
        const vector<string> &names,
        vector<CBMCVariable> &collected_variables) const {

    std::vector<uint> literals;

    collectVariables(variables, names, collected_variables);

    for (auto &var : collected_variables) {
        literals.insert(literals.cend(),
                        var.variables.cbegin(),
                        var.variables.cend());
    }

    return literals;
}

void CbmcDimacsParser::sort_variables() {
    ivar = collectVariablesInto(inames, _input_variables);
    ovar = collectVariablesInto(onames, _output_variables);
    svar = collectVariablesInto(snames, _seed_variables);

}
