//
// Created by weigl on 10.10.15.
//

#include "util.h"

#include <sstream>
#include <iomanip>
#include <locale>
#include <chrono>
#include <iostream>

using namespace std;

//TODO move to entropy.h
uint64_t input_space(const vector<uint64_t> &preimages) {
    long sum = 0;
    for (int i = 0; i < preimages.size(); ++i) {
        sum += preimages[i];
    }
    return sum;
}

long double shannon_entropy(long int SI, const vector<uint64_t> &C) {
    long double sum = 0;
    for (auto v : C)
        if (v != 0 && v != 1)
            sum += v * log2(v);
    return (1.0 / SI) * sum;
}

long double min_entropy(long int SI, long n) {
    return log2(SI) - log2(n);
}


const auto PROGRAM_START_TIME = chrono::high_resolution_clock::now();
ostream& console() {
    auto current_time = chrono::high_resolution_clock::now();
    auto difference = chrono::duration_cast<std::chrono::milliseconds>(
                         current_time - PROGRAM_START_TIME);

    ios init(NULL);
    init.copyfmt(cout);
    cout.imbue(std::locale(""));
    cout << '['
         << setw(8) << setfill(' ') << difference.count()
         <<']'
         << " ";
    cout.copyfmt(init);
    return cout;
}


const bool DEBUG = true;
ostream& debug() {
    if(DEBUG) {
        return console();
    } else {
        //TODO
        return console();
    }
}


#ifdef _WIN32
#include <Windows.h>
double get_wall_time(){
    LARGE_INTEGER time,freq;
    if (!QueryPerformanceFrequency(&freq)){
        //  Handle error
        return 0;
    }
    if (!QueryPerformanceCounter(&time)){
        //  Handle error
        return 0;
    }
    return (double)time.QuadPart / freq.QuadPart;
}
double get_cpu_time(){
    FILETIME a,b,c,d;
    if (GetProcessTimes(GetCurrentProcess(),&a,&b,&c,&d) != 0){
        //  Returns total user time.
        //  Can be tweaked to include kernel times as well.
        return
            (double)(d.dwLowDateTime |
            ((unsigned long long)d.dwHighDateTime << 32)) * 0.0000001;
    }else{
        //  Handle error
        return 0;
    }
}

//  Posix/Linux
#else

#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>

double get_wall_time() {
    struct timeval time;
    if (gettimeofday(&time, NULL)) {
        //  Handle error
        return 0;
    }
    return (double) time.tv_sec + (double) time.tv_usec * .000001;
}

/*double get_cpu_time(){
    return (double)clock() / CLOCKS_PER_SEC;
}*/

double get_cpu_time() {
    struct rusage *usage = new rusage();
    int r = getrusage(RUSAGE_SELF, usage);
    return (double) usage->ru_utime.tv_sec + (double) usage->ru_utime.tv_usec * .000001;
}

template<typename T>
T sum_buckets(vector<T> buckets) {
    T sum = 0;
    for(auto i : buckets) {
        sum += i;
    }
    return sum;
}

template uint64_t sum_buckets<uint64_t>(vector<uint64_t> buckets); // explicit instantiation.


long double shannon_entropy_max(uint64_t SI, uint64_t inputs, uint64_t outputs) {
    long double sum = 0;

    if (inputs > outputs) {
        long double q = inputs / outputs;
        long double residum = inputs - q;

        long double item_in_box = floor(q);

        //assert(residum < outputs);

        sum = residum * (item_in_box + 1) * log2(item_in_box + 1);
        sum += (outputs - residum) * (item_in_box * log2(item_in_box));
    }
    return 1.0 / SI * sum;
}


#endif
