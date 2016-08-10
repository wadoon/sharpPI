//
// Created by weigl on 15.10.15.
//

#pragma once

#include <vector>
#include <cmath>
#include <fstream>
#include "util.h"

#include <Eigen/Core>

#include <numeric>

using Eigen::MatrixXd;
using Eigen::MatrixXi;
using Eigen::VectorXd;
using namespace Eigen;

using namespace std;

uint64_t space_size(const uint64_t num_of_variables);


/**
 *
 *
 */
long double shannon_entropy(uint64_t SI, const vector<uint64_t> &C);

/**
 *
 *
 */
long double shannon_entropy(uint64_t SI, const Buckets&);


/**
 *
 */
long double min_entropy(long int SI, long n);


/**
 *
 *
 */
template<typename T>
T sum_buckets(const vector<T> &buckets) {
    return accumulate(buckets.begin(), buckets.end(), 0);
}
template uint64_t sum_buckets<uint64_t>(const vector<uint64_t> &buckets); // explicit instantiation.

template<typename T>
T sum_buckets(const vector<_Bucket<T>> &buckets) {
    T sum;
    for(auto& b : buckets) {
        sum+=b.size;
    }
    return sum;

}



long double shannon_entropy_upper_bound(const Buckets& buckets,
                                        uint64_t allpreimages,
                                        uint64_t countedpreimages);

long double shannon_entropy_lower_bound(const Buckets& buckets,
                                        uint64_t allpreimages,
                                        uint64_t countedpreimages);


class CounterMatrix {

public:
    //CounterMatrix(int inputs, int outputs) : matrix(1 << inputs, 1 << outputs) { }
    CounterMatrix(int inputs, int outputs) : matrix(2, 36), found_pairs(0) { }

    void ensure_size(uint64_t rows, uint64_t cols) {
        long old_rows = matrix.rows();
        long old_cols = matrix.cols();

        if (old_rows < rows || old_cols < cols) {
            matrix.conservativeResize(rows, cols);
            for (long r = old_rows; r < rows; ++r) {
                for (long c = old_cols; c < cols; ++c) {
                    matrix(r, c) = 0;
                }
            }
        }
    }

    void count(uint64_t input_value, uint64_t output_value) {
        ensure_size(input_value + 1, output_value + 1);
        matrix(input_value, output_value) += 1;
        found_pairs++;
    }

    MatrixXd joint_probability() const {
        return matrix.cast<double>() * (1.0 / (double) found_pairs);
    }

    VectorXd py() const {
        return joint_probability().colwise().sum();
    }


    /**
     *
     */
    long double shannon_entropy();

    unsigned long input_count() const { return this->found_pairs;}

    long double min_entropy();

protected:
    long double shannon_entropy(const VectorXd &py, const MatrixXd &p_xy);

    /**
     * Count of found pairs of input - output relations
     */
    uint64_t  found_pairs;

    /**
     * P( X = row | Y = col )
     */
    MatrixXi matrix;


    /**
     *  If true, the instance outputs the matrices
     */
    bool verbose = false;
};
