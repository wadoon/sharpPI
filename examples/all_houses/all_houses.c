#include <stdlib.h>

// N is the total number of houses
/* define N as a macro on the command line */
// static int N=150;

#ifndef N
#error define N as a macro on the command line
#endif

#ifdef CASE_B
// each size correspond to a different level of consumption
const int small_consumption  = 1;
const int medium_consumption = 3;
const int large_consumption  = 5;
#endif

#ifdef CASE_A
// each size correspond to a different level of consumption
const int small_consumption  = 1;
const int medium_consumption = 2;
const int large_consumption  = 3;
#endif

#ifndef CASE_A
#ifndef CASE_B
#error "Please provide »-D CASE_A« or »-D CASE_B«"
#endif
#endif


// the observable is the global consumption of the system


// The secret is the presence of people in each house


_Bool nondet_bool();

int main(){
    int global_consumption = 0;

    int i = 0;
    // We consider different sizes of houses. S, M and L
    // indicate the number of houses of each size.

    int S=N/3 ;
    int M=N/3 ;
    int L=N-S-M ;

    __CPROVER_bool presence[N];

    __CPROVER_bool I = 0;

    while ( i < N) {
        presence[i] = nondet_bool();
        I = presence[i];

#ifdef __CPROVER__
        __CPROVER_assume(presence[i]==0||presence[i]==1);
#endif
        if (I) {
            if (i<S) {
                global_consumption = global_consumption + small_consumption ;
            }
            else if (i<S+M) {
                global_consumption = global_consumption + medium_consumption ;
            }
            else{
                global_consumption = global_consumption + large_consumption ;
            }
        }
        i= i + 1;
    }

    assert(0);

}
