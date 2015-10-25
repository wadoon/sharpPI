//
// Created by weigl on 10.10.15.
//

#include "util.h"

#include <sstream>

using namespace std;

//TODO move to entropy.h
long input_space(const vector<uint64_t> &preimages) {
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

#endif