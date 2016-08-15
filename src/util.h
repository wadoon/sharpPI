//
// Created by weigl on 10.10.15.
//
#pragma once

#include <string>
#include <vector>
#include <cmath>
#include <sstream>
#include <boost/chrono.hpp>


#ifdef GLUCOSE
#include "glucose/core/Solver.h"
using namespace Glucose;
#else
#include "minisat/core/Solver.h"
using namespace Minisat;
#endif

using namespace std;


uint64_t get_time();

ostream& debug();
ostream& console();


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


/**
 * I/O
 */

template<typename T>
ostream& operator<<(std::ostream& stream, const _Bucket<T> &b) {
    stream << "{" << b.size << ", " << b.closed << "}";
    return stream;
}

template<typename T>
ostream &operator<<(std::ostream &stream, const std::vector<T> &v) {
    stream << "[";
    for (int i = 0; i < v.size(); ++i) {
        stream << v[i];
        if (i < v.size() - 1)
            stream << ", ";
    }
    stream << "]";

    return stream;
}
