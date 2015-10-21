//
// Created by weigl on 10.10.15.
//

#include "util.h"

#include <sstream>

using namespace std;

long input_space(const vector<uint64_t>& preimages) {
    long sum = 0;
    for (int i = 0; i < preimages.size(); ++i) {
        sum += preimages[i];
    }
    return sum;
}

long double shannon_entropy(long int SI, const vector<uint64_t>& C) {
    long long sum = 0;
    for (int i = 0; i < C.size(); ++i) {
        sum += C[i] * log2(C[i]);
    }
    return (1.0 / SI) * sum;
}

long double min_entropy(long int SI, long n) {
    return log2(SI) - log2(n);
}
