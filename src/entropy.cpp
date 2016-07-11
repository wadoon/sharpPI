//
// Created by weigl on 15.10.15.
//

#include "entropy.h"


uint64_t space_size(const uint64_t num_of_variables) {
    return ((uint) 1 ) << num_of_variables;
}

long double shannon_entropy(uint64_t SI, const vector<uint64_t> &C) {
    long double sum = 0;
    for (auto v : C)
        if (v != 0 && v != 1)
            sum += v * log2(v);
    return (1.0 / SI) * sum;
}

long double min_entropy(long int SI, long n) {
    return log2(SI) - log2(n);
}

long double shannon_entropy(uint64_t SI, const Buckets& buckets) {
    long double sum = 0;
    for (auto v : C)
        if (v.size != 0 && v.size != 1)
            sum += v.size * log2(v.size);
    return (1.0 / SI) * sum;
}

long double shannon_entropy_upper_bound(const Buckets& buckets,
                                        uint64_t allpreimages,
                                        uint64_t countedpreimages) {
    vector<uint64_t> cpy(buckets);
    auto max = distance(cpy.begin(), max_element(cpy.begin(), cpy.end()));
    auto residum=  allpreimages - countedpreimages;
    cpy[max] += residum;
    return shannon_entropy(input_space, cpy);
}


long double shannon_entropy_lower_bound(const Buckets& buckets,
                                        uint64_t allpreimages,
                                        uint64_t countedpreimages) {

    vector<uint64_t> cpy(buckets);
    priority_queue<uint64_t, vector<uint64_t>, greater<uint64_t>> q(cpy.begin(), cpy.end());

    auto residum=  allpreimages - countedpreimages;

    while(residum > 0) {
        auto min1 = q.top();
        q.pop();

        auto min2 = q.top();

        auto add = min(residum, min2 - min1 + 1);

        q.push(min1+add);
        residum -= add;
    }

    return shannon_entropy(input_space, cpy);
}



long double CounterMatrix::shannon_entropy(const VectorXd &py, const MatrixXd &p_xy) {
    long double shentropy = 0;
    for (int y = 0; y < py.rows(); ++y) {

        long double tmp = 0;

        //! H(X|Y=y) = \SUM {x \in X} : P(X=x|Y=y) * log2( 1 / P(X=X|Y=y)
        for (int x = 0; x < p_xy.rows(); ++x) {
            long double Pxy = p_xy(x, y);
            if (Pxy != 0 && Pxy != 1) {
                tmp = tmp + Pxy * (-log2(Pxy));
            }
        }

        //! \SUM { y \in Y } : P(y) * H(X|Y=y)
        shentropy = shentropy + (py[y] * tmp);
    }
    return shentropy;
}

long double CounterMatrix::shannon_entropy() {
    auto joint = joint_probability();
    auto py = joint.colwise().sum();

    MatrixXd p_xy(joint.rows(), joint.cols());
    for (int x = 0; x < p_xy.rows(); ++x) {
        for (int y = 0; y < p_xy.cols(); ++y) {
            // P( X=x | Y=y) = P(X=x,Y=y) / P(Y=y)
            p_xy(x, y) = joint(x, y) / py[y];
        }
    }


    if (verbose) {
        ofstream countmatrix("matrix.csv");
        countmatrix << matrix << endl;

        ofstream a("py.csv");
        a << py << endl;

        ofstream b("joint.csv");
        b << joint << endl;

        ofstream c("p_xy.csv");
        c << p_xy << endl;
    }

    return shannon_entropy(py, p_xy);
}

long double CounterMatrix::min_entropy() {

    return 0;
}
