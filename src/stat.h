#pragma once 

#include <stdint.h>
#include <iostream>

#include "sat.h"

/**
 *
 */
struct Statistics {
    /**
     *
     */
    template<typename T>
    struct Guess {
        T guess, upper_bound, lower_bound;
    };

    /**
     *
     */
    ofstream fileout;

    /**
     * current iteration
     */
    uint64_t num_of_iteration;

    /**
     * Number of outputs found so far.
     */

    uint64_t number_of_outputs;

    /**
     * Number of inputs found so far.
     */
    uint64_t number_of_inputs;


    /**
     * theoretical maximal inputs
     */
    uint64_t SI;

    /**
     * theoretical maximal outputs
     */
    uint64_t SO;

    /**
     * The cpu time consumed in one iteration.
     */
    double cpu_time_consumed;

    /**
     * Guesses of shannon
     */
    Guess<long double> shannon_entropy;

    /**
     * Guess of min entropy (currently unused)
     */
    Guess<long double> min_entropy;


    /**
     * TODO SAT solver statistics
     */
    int restarts,	
	conflict,
	decisions,
	decisions_rnd,
	propagations, 
	conflict_literals;
    

    /**
     * Write headers to the statistic file
     */
    void header() {
        if (fileout.is_open()) {
            cout << "number_of_iteration"
                    "\tcpu_time_consumed"
                    "\tshannon_lower"
                    "\tshannon_guess"
                    "\tshannon_upper"
                    "\tmin_lower"
                    "\tmin_guess"
                    "\tmin_upper"
                    "\tnumber_of_inputs"
                    "\tnumber_of_outputs" << endl;
        }
    }

    void open(const string& filename) {
        fileout.open(filename);
        header();
    }

    void close() {
        if (fileout.is_open())
            { fileout.close(); }
    }

    void update(const Buckets& buckets) {
        shannon_entropy.guess       = ::shannon_entropy(number_of_inputs, buckets);
        shannon_entropy.lower_bound = shannon_entropy_lower_bound(buckets, SI, number_of_inputs);
        shannon_entropy.upper_bound = shannon_entropy_upper_bound(buckets, SI, number_of_inputs);
        number_of_inputs = sum_buckets(buckets);
        number_of_outputs = buckets.size();
        //TODO min entropy
    }

    void update(Solver& solver) {
     restarts   =  solver.starts;
     conflict   = solver.conflicts;
     decisions  = solver.decisions; 
     decisions_rnd = solver.rnd_decisions;
     propagations    = solver.propagations;
     conflict_literals = solver.tot_literals;
    }

    /**
     *
     */
    void writeconsole() {
        cout << num_of_iteration
             << "\t" << cpu_time_consumed
             << "\t" << shannon_entropy.lower_bound
             << "\t" << shannon_entropy.guess
             << "\t" << shannon_entropy.upper_bound
             << "\t" << min_entropy.lower_bound
             << "\t" << min_entropy.guess
             << "\t" << min_entropy.upper_bound
             << "\t" << number_of_inputs
             << "\t" << number_of_outputs
             << endl;
    }

    /**
     * Write the current statistic values into `fstatistics`
     */
    void write() {
        if (fileout.is_open()) {
            cout  << num_of_iteration << "\t" << cpu_time_consumed << "\t"
                  << shannon_entropy.lower_bound << "\t"
                  << shannon_entropy.guess << "\t"
                  << shannon_entropy.upper_bound << "\t"
                  << min_entropy.lower_bound << "\t" << min_entropy.guess
                  << "\t" << min_entropy.upper_bound << "\t"
                  << number_of_inputs << "\t" << number_of_outputs << endl;
        }
    }

};

