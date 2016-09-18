#include "stat.h"

std::ostream& operator<<(std::ostream& stream, stat s) {
    stream  << s.num_of_iteration
            << "\t" << s.cpu_time_consumed
            << "\t" << s.shannon_entropy.lower_bound
            << "\t" << s.shannon_entropy.guess
            << "\t" << s.shannon_entropy.upper_bound
            << "\t" << s.min_entropy.lower_bound
            << "\t" << s.min_entropy.guess
            << "\t" << s.min_entropy.upper_bound
            << "\t" << s.number_of_inputs
            << "\t" << s.number_of_outputs
            << "\t" << s.restarts
            << "\t" << s.conflict
            << "\t" << s.decisions
            << "\t" << s.decisions_rnd
            << "\t" << s.propagations
            << "\t" << s.conflict_literals
            << "\t" << s.sat_calls
            << "\t" << s.sat_time
            << std::endl;
    return stream;
}
