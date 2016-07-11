//
// Created by weigl on 10.10.15.
//
#pragma once

#include <string>
#include <vector>
#include <cmath>
#include <sstream>

#ifdef GLUCOSE
#include "glucose/core/Solver.h"
using namespace Glucose;
#else
#include "minisat/core/Solver.h"
using namespace Minisat;
#endif

using namespace std;

#define START_TIME(name) double __timer##name cpuTime();
#define END_TIME(name) printf("%s: %f s\n", str, cpuTime() - __START__)

/**
 */
double get_wall_time();


/**
 */
double get_cpu_time();


/**
 */
ostream& debug();
ostream& console();


//#define TIME_START() double __START__ = clock();
//#define TIME_END(str) printf("%s: %f s\n", str, (clock() - __START__)/(double) CLOCKS_PER_SEC);
//#define TIME_START() time_t __START__ = time(NULL);
//#define TIME_END(str) printf("%s: %f s\n", str, (time(NULL) - __START__));

/**
 *
 */
template<typename T>
struct _Bucket {
    /**
     *
     */
    T size;

    /**
     *
     */
    bool closed;
};

/**
 *
 */
typedef _Bucket<uint64_t> Bucket;

/**
 */
typedef std::vector<Bucket> Buckets;


/**
 *
 *
 */
uint64_t space_size(const uint64_t num_of_variables);


/**
 */
long double shannon_entropy(uint64_t SI, const vector<uint64_t> &C);


/**
 */
long double min_entropy(long int SI, long n);



/**
 */
template<typename T>
std::string atos(const T &anything) {
    stringstream ss;
    ss << anything;
    return ss.str();
}


/**
 */
template<typename T>
T sum_buckets(const vector<T> &buckets);

long double shannon_entropy_lower_bound(const vector<uint64_t>& buckets,
                                        const vector<bool> &closed,
                                        uint64_t input_space,
                                        uint64_t reached_inputs);


long double shannon_entropy_upper_bound(const vector<uint64_t>& buckets,
                                        const vector<bool> &closed,
                                        uint64_t input_space,
                                        uint64_t reached_inputs);
