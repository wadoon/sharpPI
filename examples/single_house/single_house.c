
#ifndef N
#error Please provide the constant N (e.g., -D N=12)
#endif

#ifdef CASE_A
// each size correspond to a different level of consumption
const int small_consumption  = 1 ;
const int medium_consumption = 2 ;
const int large_consumption  = 3 ;
#endif

#ifdef CASE_B
// each size correspond to a different level of consumption
const int small_consumption  = 1 ;
const int medium_consumption = 3 ;
const int large_consumption  = 5 ;
#endif

#ifndef CASE_A
#ifndef CASE_B
#error "Please provide »-D CASE_A« or »-D CASE_B«"
#endif
#endif


#ifdef HOUSE_S
const int target_is_S = 1, target_is_M = 0, target_is_L = 0 ;
#endif

#ifdef HOUSE_M
const int target_is_S = 0, target_is_M = 1, target_is_L = 0 ;
#endif

#ifdef HOUSE_L
const int target_is_S = 0, target_is_M = 0, target_is_L = 1 ;
#endif

#ifndef HOUSE_S
#ifndef HOUSE_M
#ifndef HOUSE_L
#error "Please provide the target house HOUSE_SMALL, HOUSE_MEDIUM and HOUSE_LARGE"
#endif
#endif
#endif


__CPROVER_bool nondet_bool();


int main(void) {

    // indicates the size of the target. Only one of those should be one and all the
    // other 0.
    /*int target_is_S = 0 ;
      int target_is_M = 0 ;
      int target_is_L = 1 ;*/

    /*  int target_is_S;
        int target_is_M;
        int target_is_L; */

    assert(target_is_S >= 0 &&
           target_is_M >= 0 &&
           target_is_L >= 0 &&
           target_is_S + target_is_M + target_is_L == 1 );

    // We consider different sizes of houses. S, M and L indicate the number of houses of each size.
    int S= N / 3 - target_is_S ;
    int M= N / 3 - target_is_M ;
    int L= N / 3 - target_is_L ;

    // the observable is the global consumption of the system
    int global_consumption = 0;

    // the presence of people in each house
    __CPROVER_bool presence;//[N - 1];


    // the secret is the presence
    __CPROVER_bool presence_target = nondet_bool();


    if (presence_target == 1) {
        if (target_is_S == 1) {
            global_consumption= global_consumption +  small_consumption;
        }
        else if (target_is_M == 1) {
            global_consumption= global_consumption +  medium_consumption;
        }
        else {
            global_consumption= global_consumption + large_consumption;
        }
    }

    int i = 0;
    for(int i = 0;  i < N - 1; i++) {
        presence = nondet_bool();

        if (presence == 1) {
            if (i<S) {
                global_consumption = global_consumption + small_consumption;
            }
            else if (i<S+M) {
                global_consumption = global_consumption + medium_consumption;
            }
            else{
                global_consumption = global_consumption + large_consumption;
            }
        }
    }
    assert(0);
}
