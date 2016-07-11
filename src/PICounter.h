//
// Created by weigl on 11.10.15.
//

#pragma once

#include <errno.h>
#include <zlib.h>
#include <vector>
#include "cbmcparser.h"
#include "entropy.h"

#include "core/Solver.h"
#include "utils/System.h"


//#include "pstreams-0.8.1/pstream.h"


#ifdef GLUCOSE
using namespace Glucose;
#else
using namespace Minisat;
#endif


using namespace std;

#include "sat.h"

using LabelList = std::vector<std::pair<uint64_t, Var>>;


class PICounter {
public:

    PICounter() :
            solver(nullptr) {
    }

    void prohibit_project(const vector<Var> &solver);

    vector<Lit> project_model(const vector<Var> &variables);

    void set_input_literals(const vector<uint> &input_variables) {
        _input_literals.clear();
        for (uint i : input_variables) {
            _input_literals.push_back(i - 1);
        }
    }

    void set_seed_literals(const vector<uint> &seed_variables) {
        _seed_literals.clear();
        for (uint i : seed_variables) {
            _seed_literals.push_back(i - 1);
        }
    }

    vector<Var> &get_seed_literals() {
        return _seed_literals;
    }

    vector<Var> &get_output_literals() {
        return _output_literals;
    }

    vector<Var> &get_input_literals() {
        return _input_literals;
    }

    void set_output_literals(const vector<uint> &output_variables) {
        _output_literals.clear();
        for (uint i : output_variables) {
            _output_literals.push_back(i - 1);
        }
    }

    void activate(const vector<vector<int>> &vector, int max_var);

    void set_verbose(bool b) {
        this->verbose = b;
    }

    vector<uint64_t> count_det_compl();

    uint64_t count_sat(uint64_t max_count = -1);

    bool count_det_iter(vector<uint64_t> &previous);

    LabelList prepare_sync_counting(vector<bool>&,
                                    vector<uint64_t>&);

    bool count_sync(LabelList& labels,
                        vector<bool> &closed,
                        vector<uint64_t> &count_table);

    bool count_unstructured(uint64_t limit, vector<uint64_t> &count_table);

    bool count_unstructured(vector<uint64_t> &count_table);

    CounterMatrix countrand();

    void set_solver(SolverInterface *s) {
        if (this->solver != nullptr)
            delete this->solver;
        this->solver = s;
    }

    /**
     *
     */
    uint64_t interpret(vector<uint> variables) {
        uint64_t value = 0;

        for (long i = variables.size() - 1; i >= 0; --i) {
            uint v = variables.at(i);

            if (v == 0) {
                value <<= 1;
            } else if (v == -1) {
                value = (value << 1) | 1;
            } else {
                int q = solver->model_value(Var(v - 1)) == l_True ? 1 : 0;
                value = (value << 1) | q;
            }
        }
        return value;
    }

    /**
     *
     */
    uint64_t interpret(vector<Var> variables) {
        uint64_t value = 0;
        for (long i = variables.size() - 1; i >= 0; --i) {
            Var v = variables.at(i);
            int q = solver->model_value(v) == l_True ? 1 : 0;
            value = (value << 1) | q;
        }
        return value;
    }

    void set_output_variables(const vector<CBMCVariable> &_output_variables) {
        PICounter::_output_variables = _output_variables;
    }

    void set_input_variables(const vector<CBMCVariable> &_input_variables) {
        PICounter::_input_variables = _input_variables;
    }

    void set_output_model(const string &filename) {
        model_stream.open(filename);
    }

    void set_seed_variables(const vector<CBMCVariable> &seedvar) {
        _seed_variables = seedvar;
    }

    bool count_det_iter_sharp(vector<uint64_t>& previous,
                              const std::string& dimacs_filename);

private:
    SolverInterface *solver;


    std::vector<CBMCVariable> _output_variables;
    std::vector<CBMCVariable> _input_variables;
    std::vector<CBMCVariable> _seed_variables;

    std::vector<Var> _input_literals;
    std::vector<Var> _output_literals;
    std::vector<Var> _seed_literals;

    std::ofstream model_stream;

    bool verbose;

    void write_input();

    void write_output();

};
