//
// Created by weigl on 25.11.15.
//
#pragma once

#include "core/Solver.h"
#include "utils/System.h"

#include <errno.h>
#include <zlib.h>
#include <vector>
#include "cbmcparser.h"
#include "entropy.h"
#include "util.h"


#ifdef GLUCOSE
using namespace Glucose;
#else
using namespace Minisat;
#endif

using namespace std;

class SolverInterface {
public:
    SolverInterface() {
    };

    virtual ~SolverInterface() {
    };

    virtual void add_clause(const vector<Lit> &clause) = 0;

    virtual bool solve(const vector<Lit> &assumptions) = 0;

    virtual bool solve() = 0;

    virtual int num_variables() = 0;

    virtual lbool model_value(Var v) = 0;

    virtual void ensure_variables(int max_var) = 0;

    virtual Var  new_variable() = 0;

};

template<typename T=Lit>
inline Var convert(const vector<T> &a, vec<T> &b) {
    Var v = 0;
    b.clear();
    for (const T &tmp : a) {
        v = max(v, var(tmp));
        b.push(tmp);
    }
    return v;
}

class MinisatInterface : public SolverInterface {
public:
    MinisatInterface() :
            solver() {
    }

    ~MinisatInterface() {
    }

    void ensure(Var max_var) {
        while (solver.nVars() <= max_var) {
            solver.newVar(false, false);
        }
    }

    virtual void add_clause(const vector<Lit> &clause) {
        vec<Lit> v;
        ensure(convert(clause, v));
        solver.addClause(v);
    }

    virtual bool solve(const vector<Lit> &assumptions) {
        sat_calls++;
        vec<Lit> v;
        ensure(convert(assumptions, v));
        auto start = get_time();
        bool b =  solver.solve(v);
        last_sat_time = start - get_time();
        return b;
    }

    virtual bool solve() {
        sat_calls++;
        return solver.solve();
    }

    virtual lbool model_value(Var v) {
        return solver.modelValue(v);
    }

    virtual int num_variables() {
        return solver.nVars();
    }

    virtual Var new_variable() override {
        return solver.newVar(false,false);
    }


public:
    uint64_t sat_calls, last_sat_time;
    Solver solver;
    void ensure_variables(int max_var);
};
