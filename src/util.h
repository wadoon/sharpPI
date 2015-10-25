//
// Created by weigl on 10.10.15.
//


#ifndef SHARPPIC_UTIL_H
#define SHARPPIC_UTIL_H

#include "minisat/utils/System.h"
#include <string>
#include <vector>
#include <cmath>
#include <sstream>


using namespace std;

#define START_TIME(name) double __timer##name cpuTime();
#define END_TIME(name) printf("%s: %f s\n", str, cpuTime() - __START__)

double get_wall_time();
double get_cpu_time();

//#define TIME_START() double __START__ = clock();
//#define TIME_END(str) printf("%s: %f s\n", str, (clock() - __START__)/(double) CLOCKS_PER_SEC);
//#define TIME_START() time_t __START__ = time(NULL);
//#define TIME_END(str) printf("%s: %f s\n", str, (time(NULL) - __START__));

long int input_space(const vector<uint64_t> &preimages);

long double shannon_entropy(long int SI, const vector<uint64_t> &C);

long double min_entropy(long int SI, long n);

template<typename T>
std::string atos(const T &anything) {
    stringstream ss;
    ss << anything;
    return ss.str();
}

#endif //SHARPPIC_UTIL_H
