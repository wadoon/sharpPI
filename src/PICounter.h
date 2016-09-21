//
// Created by weigl on 11.10.15.
//

#pragma once

#include <errno.h>
#include <zlib.h>
#include <vector>
#include <chrono>
#include "cbmcparser.h"
#include "entropy.h"
#include <assert.h>
#include "core/Solver.h"
#include "utils/System.h"
#include <limits>
<<<<<<< HEAD
#include "sat.h"
#include "stat.h"
#include "sharpsat.h"
=======
#include <iostream>

//#include "pstreams-0.8.1/pstream.h"
>>>>>>> print input/seed/ouput pairs


#ifdef GLUCOSE
using namespace Glucose;
#else
using namespace Minisat;
#endif


using namespace std;


using LabelList = std::vector<std::pair<uint64_t, Var>>;

class PICounter; // forward

class BucketListWriter {
public:
  BucketListWriter() 
    : active(false) {
    
  }

  void activate(const std::string& filename) {
    out.open(filename);
    active = true;
  }

  void header(const PICounter*);
  void write(MinisatInterface*);


public:
  bool active;

private:
    
    void write_header(const std::vector<CBMCVariable>& vars, 
		      string type) {     
      for(auto var : vars) {
	out << "% "  << type << " " << var.variable_name << " " << var.time;
	for(auto pvar : var.variables) {
	  out << " " << pvar;
	  variables.push_back(pvar);
	}
	out << "\n";
      }       
    }


  ofstream out;
  std::vector<uint> variables;
};

class PICounter {
  friend class BucketListWriter;
public:

    PICounter() :
    solver(nullptr)
    { 
      last = chrono::high_resolution_clock::now(); 


    }

    ~PICounter() {
            if(_stat.active) {
                console() << "write statistic file: " << statistic_filename << endl;
                ofstream s(statistic_filename);
                s << _stat.fileout.str();
            } else {
                console() << "statistic deactivated" << endl;
            }
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

    Buckets count_bucket_all();

    bool count_one_bucket(Buckets& previous);

    bool count_one_bucket_sharp(Buckets& previous, SharpSAT& sharp);

    uint64_t count_sat(uint64_t max_count = -1);

    bool count_det_iter(vector<uint64_t> &previous);

    LabelList prepare_sync_counting(Buckets& buckets);

    bool count_sync(const LabelList& labels, Buckets& buckets);

    bool count_unguided(Buckets& buckets);

    bool count_unstructured_one(Buckets& buckets);

    CounterMatrix count_rand();

    void set_solver(MinisatInterface *s) {
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

    void set_seed_variables(const vector<CBMCVariable> &seedvar) {
        _seed_variables = seedvar;
    }

    //    bool count_det_iter_sharp(vector<uint64_t>& previous,
    //                          const std::string& dimacs_filename);


    void enable_stat(const string& filename) {
        _stat.active = true;
        statistic_filename = filename;
        _stat.header();
    }

    void enable_list(const string& filename) {
      _listwriter.activate(filename);
      _listwriter.header(this);
    }

    Statistic& stat() { return _stat; }

    void set_tolerance(double t) { tolerance = t; }

    bool tolerance_met(const Buckets& buckets) {
        if(tolerance == -INFINITY) //disabled
            return false;
        auto all_preimages = space_size(_input_literals.size());
        auto found_preimages = sum_buckets(buckets);
        auto l = shannon_entropy_lower_bound(buckets, all_preimages, found_preimages);
        auto u = shannon_entropy_upper_bound(buckets, all_preimages, found_preimages);
        assert( l - u > 0 );

        if(verbose){
            console() << "Upper bound:   " << u << endl;
            console() << "Lower bound:   " << l << endl;
            console() << "Tolerance met: " << (l - u <= tolerance) << endl;
        }

        return  l - u <= tolerance;
    }

private:
    Statistic _stat;
    BucketListWriter _listwriter;

    //SolverInterface *solver;
    MinisatInterface *solver;


    std::vector<CBMCVariable> _output_variables;
    std::vector<CBMCVariable> _input_variables;
    std::vector<CBMCVariable> _seed_variables;

    std::vector<Var> _input_literals;
    std::vector<Var> _output_literals;
    std::vector<Var> _seed_literals;

    bool verbose;
    
    std::string
      statistic_filename,
      bucketlist_filename;

    

    double tolerance = -INFINITY;

    chrono::high_resolution_clock::time_point last;
  
  void stat_point(const Buckets& result);
  void list() {
    _listwriter.write(solver);
  }
  
};
