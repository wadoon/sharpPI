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
using namespace std::chrono;

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

uint64_t get_time() {
    const auto now = chrono::high_resolution_clock::now();
    const auto duration = now - PROGRAM_START_TIME;
    return duration_cast<milliseconds>(duration).count();
}

uint64_t get_time_diff(chrono::high_resolution_clock::time_point& p) {
    const auto now = chrono::high_resolution_clock::now();
    const auto duration = now - p;
    p = now;
    return duration_cast<nanoseconds>(duration).count();
}
