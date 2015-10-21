//
// Created by weigl on 15.10.15.
//

#include "entropy.h"

long double CounterMatrix::shannon_entropy(const VectorXd &py, const MatrixXd &p_xy) {
    long double shentropy = 0;
    for (int y = 0; y < py.rows(); ++y) {

        long double tmp = 0;

        //! H(X|Y=y) = \SUM {x \in X} : P(X=x|Y=y) * log2( 1 / P(X=X|Y=y)
        for (int x = 0; x < p_xy.rows(); ++x) {
            long double Pxy = p_xy(x, y);
            if (Pxy != 0 && Pxy != 1) {
                tmp = tmp + Pxy * (log2(1.0 / Pxy));
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