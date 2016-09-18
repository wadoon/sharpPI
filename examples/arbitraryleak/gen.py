#!/usr/bin/python3
from __future__ import print_function
import numpy as np
from scipy import stats

import sys
def main():
    D = { "bernoulli" : stats.bernoulli,
          "binom" : stats.binom,
          "boltzmann" : stats.boltzmann,
          "dlaplace" : stats.dlaplace,
          "geom" : stats.geom,
          "hypergeom" : stats.hypergeom,
          "logser" : stats.logser,
          "nbinom" : stats.nbinom,
          "planck" : stats.planck,
          "poisson" : stats.poisson,
          "randint" : stats.randint,
          "skellam" : stats.skellam,
          "zipf" : stats.zipf }

    try:
        if len(sys.argv) < 3:
            raise BaseException()

        distrib, bw_in, bw_out = D[sys.argv[3]], int(sys.argv[1]), int(sys.argv[2])
        size = 2**bw_out - 1
        args = map(float, sys.argv[4:])
    except BaseException as e:
        print("Usage: gen.py <insize> <outsize> <distrib> <*args>")
        print("\t<distrib> can be one of:", ', '.join(D))
        print("\t<size> is an integer >0")
        print(e)
        sys.exit(1)


    X = np.array(range(size))
    Y = distrib.pmf(X, *args)

    for x,y in zip(X,Y):
        print("// %03d |" % x,  '#'* int(y*50))
        if y == 0: break;


    def construct_table():
        table = []
        cdf = np.cumsum(Y)

        max_output = 2 ** bw_in - 1;

        value = lambda x: int(round(max_output * x))
        pre = 0
        for v in range(len(cdf)):
            if value(pre) != value(cdf[v]):
                table.append( [v, value(pre),
                               value(cdf[v])])
            pre = cdf[v]

        table[-1][2] += 1
        return table

    table = construct_table()

    print("// %s" % sys.argv)

    print("""

#include <stdint.h>
int main(int argc, char* argv[] ){
\tuint%d_t I;
\tuint%d_t O;
\tI &= 0b%s;
    """ % (next_size(bw_in), next_size(bw_out), '1'*bw_in));

    ifs = [
        "if( {1} <= I && I < {2} ) {{ O = {0}; }}".format(*x)
        for x in table
    ]

    print("\t", end="")
    print("\n\telse ".join(ifs))

    print("\tassert(0);")
    print("}")


from math import ceil, log
def next_size(x):
    return 2**ceil( log(x,2) )

__name__ == '__main__' and main()
