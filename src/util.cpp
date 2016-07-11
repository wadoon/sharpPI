//
// Created by weigl on 10.10.15.
//

#include "util.h"

#include <sstream>
#include <iomanip>
#include <locale>
#include <chrono>
#include <iostream>
#include <algorithm>

using namespace std;

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
#endif
