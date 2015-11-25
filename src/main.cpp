#include <iostream>

#include "termbox.h"
#include "CommandLineArguments.h"
#include "cbmc/cbmcparser.h"
#include "util.h"
#include "PICounter.h"

using namespace std;

/**
 * File Stream for writing statistics.
 * Opened if `--stat` is given on the command line.
 */
ofstream fstatistics;

template<typename T>
ostream &operator<<(ostream &stream, const vector<T> &v) {
    stream << "[";
    for (int i = 0; i < v.size(); ++i) {
        stream << v[i];
        if (i < v.size() - 1)
            stream << ", ";
    }
    stream << "]";

    return stream;
}

/**
 *
 */
struct Statistics {
    /**
     *
     */
    template<typename T>
    struct Guess {
        T guess, upper_bound;
        long double lower_bound;
    };


    /**
     * current iteration
     */
    uint64_t num_of_iteration;

    /**
     * Number of outputs found so far.
     */

    uint64_t number_of_outputs;

    /**
     * Number of inputs found so far.
     */
    uint64_t number_of_inputs;

    /**
     * The cpu time consumed in one iteration.
     */
    double cpu_time_consumed;

    /**
     * Guesses of shannon
     */
    Guess<long double> shannon_entropy;

    /**
     * Guess of min entropy (currently unused)
     */
    Guess<long double> min_entropy;

    /**
     * Write headers to the statistic file
     */
    void header() {
        if (fstatistics.is_open()) {
            fstatistics << "number_of_iteration"
                    "\tcpu_time_consumed"
                    "\tshannon_lower"
                    "\tshannon_guess"
                    "\tshannon_upper"
                    "\tmin_lower"
                    "\tmin_guess"
                    "\tmin_upper"
                    "\tnumber_of_inputs"
                    "\tnumber_of_outputs" << endl;
        }
    }

    /**
     * Write the current statistic values into `fstatistics`
     */
    void write() {
        if (fstatistics.is_open()) {
            fstatistics << num_of_iteration << "\t" << cpu_time_consumed << "\t"
            << shannon_entropy.lower_bound << "\t"
            << shannon_entropy.guess << "\t"
            << shannon_entropy.upper_bound << "\t"
            << min_entropy.lower_bound << "\t" << min_entropy.guess
            << "\t" << min_entropy.upper_bound << "\t"
            << number_of_inputs << "\t" << number_of_outputs << endl;

        }
    }

} statistics;

/**
 *
 */
int count_deterministic(CommandLineArguments &cli, PICounter &counter) {
    cout << "Operation Mode: Deterministic NAIVE" << endl;
    counter.set_verbose(cli.verbose());
    vector<uint_fast64_t> ret = counter.count_det_compl();

    if (cli.verbose())
        cout << "Result: " << ret << "\n";

    //Assume we have 2^|INPUTVARS| possible inputs
    unsigned long int SI = input_space(ret);

    auto shannon_e = shannon_entropy(SI, ret);
    auto min_e = min_entropy(SI, ret.size());
    auto leakage = log2(SI) - shannon_entropy(SI, ret);

    vector<vector<string>> results = {{"Input Space Size",       atos(SI)},
                                      {"Shannon Entropy H(h|l)", atos(shannon_e)},
                                      {"Min Entropy",            atos(min_e)},
                                      {"Leakage",                atos(leakage)},};

    cout << TB.create_table(results) << endl;
}

/**
 *
 */
int ndet(CommandLineArguments &arguments, PICounter &counter) {
    CounterMatrix m = counter.countrand();

    unsigned long SI = m.input_count();
    long double shannon_e = m.shannon_entropy();
    long double min_e = m.min_entropy();
    long double leakage = SI - shannon_e;

    vector<vector<string>> results = {{"Input Space Size",       atos(SI)},
                                      {"Shannon Entropy H(h|l)", atos(shannon_e)},
                                      {"Min Entropy",            atos(min_e)},
                                      {"Leakage",                atos(leakage)},};

    cout << TB.create_table(results) << endl;
    return 0;
}

int det_shuffle(const CommandLineArguments &cli, PICounter &counter) {
    cout << "Operation Mode: DETERMINISTIC SHUFFLE" << endl;
    uint SO = (uint) 1 << counter.get_output_literals().size();

    if (SO == 0) {
        throw logic_error("to much possible outputs (>32)");
    }

    vector<bool> closed(SO, false);
    vector<uint64_t> ret(SO, 0);

    cout << "Count Limit: " << cli.limit() << endl;

    for (uint i = 0; i < cli.limit(); i++) {

        double start = cpuTime();

        bool b = counter.count_unstructured(ret);

        double end = cpuTime();

        if (b) {
            cout << "There are sill more input/output relations" << endl;
        } else {
            cout << "Search was exhaustive" << endl;
            break;
        }

        if (cli.verbose()) {
            cout << "Result: " << ret << "\n";
        }

        //Assume we have 2^|INPUTVARS| possible inputs
        unsigned long int SI = (uint) 1 << counter.get_input_literals().size();

        auto shannon_e = shannon_entropy(SI, ret);
        auto min_e = min_entropy(SI, ret.size());
        auto leakage = log2(SI) - shannon_entropy(SI, ret);

        vector<vector<string>> results = {
                {"Input Space Size",       atos(SI)},
                {"Shannon Entropy H(h|l)", atos(shannon_e)},
                {"Min Entropy",            atos(min_e)},
                {"Leakage",                atos(leakage)},
        };

        cout << TB.create_table(results) << endl;

        if (cli.statistics()) {
            statistics.num_of_iteration = i;
            statistics.number_of_inputs = sum_buckets(ret);
            statistics.number_of_outputs = ret.size();
            statistics.cpu_time_consumed = end - start;

            vector<uint64_t> lowerbound(ret);
            size_t index = min_element(lowerbound.begin(), lowerbound.end())
                           - lowerbound.begin();
            lowerbound[index] += (SI - statistics.number_of_inputs);

            statistics.shannon_entropy.lower_bound = shannon_entropy(SI, lowerbound);
            statistics.shannon_entropy.upper_bound =
                    shannon_e + shannon_entropy_max(SI, (SI - statistics.number_of_inputs), SO);
            statistics.shannon_entropy.guess = shannon_e;
            statistics.min_entropy.guess = 0;
            statistics.write();
        }

    }

    return 0;
}

int det_iter(const CommandLineArguments &cli, PICounter &counter) {
    cout << "Operation Mode: Deterministic ITER" << endl;
    //Assume we have 2^|INPUTVARS| possible inputs
    unsigned long int SI = (unsigned long) (1
                                            << counter.get_input_literals().size());
    unsigned long int SO = (unsigned long) (1
                                            << counter.get_output_literals().size());

    vector<uint64_t> ret(SI);

    bool b = true;
    for (int k = 1; k <= cli.limit() && b; k++) {

        double start = get_cpu_time();
        b = counter.count_det_iter(ret);
        double end = get_cpu_time();

        if (cli.verbose())
            cout << k << "# Result: " << ret << "\n";

        auto shannon_e = shannon_entropy(SI, ret);
        auto min_e = min_entropy(SI, ret.size());
        auto leakage = log2(SI) - shannon_entropy(SI, ret);

        vector<vector<string>> results = {{"Input Space Size",       atos(SI)},
                                          {
                                           "Shannon Entropy H(h|l)", atos(shannon_e)},
                                          {"Min Entropy",
                                                                     atos(min_e)},
                                          {"Leakage",                atos(leakage)},};

        cout << TB.create_table(results) << endl;

        if (cli.statistics()) {
            statistics.num_of_iteration = k;
            statistics.number_of_inputs = sum_buckets(ret);
            statistics.number_of_outputs = ret.size();
            statistics.cpu_time_consumed = end - start;
            statistics.shannon_entropy.guess = shannon_e;

            long double lower_guess = 1.0 / SI
                                      * (SI - statistics.number_of_inputs)
                                      * log2((long double) (SI - statistics.number_of_inputs));

            statistics.shannon_entropy.lower_bound = shannon_e + lower_guess;

            statistics.shannon_entropy.lower_bound = shannon_e
                                                     + shannon_entropy_max(SI, SI - statistics.number_of_inputs,
                                                                           SO - statistics.number_of_outputs);

            statistics.min_entropy.guess = 0;
            statistics.write();
        }
    }
    return 0;
}

int det_succ(const CommandLineArguments &cli, PICounter &counter) {
    cout << "OperationMode: DETERMINISTIC SUCCESSIVE" << endl;

    const unsigned long SO = 1u << counter.get_output_literals().size();
    const unsigned long SI = 1u << counter.get_input_literals().size();

    vector<bool> closed(SO, false);
    vector<uint64_t> ret(SO, 0);

    bool b = true;
    for (uint i = 0; i < cli.limit() && b; i++) {
        double start = get_cpu_time();
        b = counter.count_det_succ(closed, ret);
        double end = get_cpu_time();

        auto shannon_e = shannon_entropy(SI, ret);
        auto min_e = min_entropy(SI, ret.size());
        auto leakage = log2(SI) - shannon_entropy(SI, ret);

        if (cli.verbose())
            cout << "Result: " << ret << "\n";

        vector<vector<string>> results = {{"Input Space Size",       atos(SI)},
                                          {"Shannon Entropy H(h|l)", atos(shannon_e)},
                                          {"Min Entropy",            atos(min_e)},
                                          {"Leakage",                atos(leakage)},};
        cout << TB.create_table(results) << endl;

        if (cli.statistics()) {
            statistics.num_of_iteration = i;
            statistics.number_of_inputs = sum_buckets(ret);
            statistics.number_of_outputs = ret.size();
            statistics.cpu_time_consumed = end - start;

            vector<uint64_t> lowerbound(ret);
            size_t index = min_element(lowerbound.begin(), lowerbound.end())
                           - lowerbound.begin();
            lowerbound[index] += (SI - statistics.number_of_inputs);

            statistics.shannon_entropy.lower_bound = shannon_entropy(SI, lowerbound);
            statistics.shannon_entropy.upper_bound = shannon_e + shannon_entropy_max(SI,
                                                                                     (SI - statistics.number_of_inputs),
                                                                                     SO);
            statistics.shannon_entropy.guess = shannon_e;
            statistics.min_entropy.guess = 0;
            statistics.write();
        }
    }

    if (!b) {
        cout << "Search was exhaustive" << endl;
    }

    return 0;
}

/**
 *
 */
int det_iter_sharp(const CommandLineArguments &cli, PICounter &counter) {
    cout << "Operation Mode: Deterministic ITER" << endl;
    //Assume we have 2^|INPUTVARS| possible inputs
    unsigned long int SI = (unsigned long) (1
                                            << counter.get_input_literals().size());
    unsigned long int SO = (unsigned long) (1
                                            << counter.get_output_literals().size());

    vector<uint64_t> ret(SI);

    bool b = true;
    for (int k = 1; k <= cli.limit() && b; k++) {

        double start = get_cpu_time();
        b = counter.count_det_iter_sharp(ret);
        double end = get_cpu_time();

        if (cli.verbose())
            cout << k << "# Result: " << ret << "\n";

        auto shannon_e = shannon_entropy(SI, ret);
        auto min_e = min_entropy(SI, ret.size());
        auto leakage = log2(SI) - shannon_entropy(SI, ret);

        vector<vector<string>> results = {{"Input Space Size",       atos(SI)},
                                          {
                                           "Shannon Entropy H(h|l)", atos(shannon_e)},
                                          {"Min Entropy",
                                                                     atos(min_e)},
                                          {"Leakage",                atos(leakage)},};

        cout << TB.create_table(results) << endl;

        if (cli.statistics()) {
            statistics.num_of_iteration = k;
            statistics.number_of_inputs = sum_buckets(ret);
            statistics.number_of_outputs = ret.size();
            statistics.cpu_time_consumed = end - start;
            statistics.shannon_entropy.guess = shannon_e;

            long double lower_guess = 1.0 / SI
                                      * (SI - statistics.number_of_inputs)
                                      * log2((long double) (SI - statistics.number_of_inputs));

            statistics.shannon_entropy.lower_bound = shannon_e + lower_guess;

            statistics.shannon_entropy.lower_bound = shannon_e
                                                     + shannon_entropy_max(SI, SI - statistics.number_of_inputs,
                                                                           SO - statistics.number_of_outputs);

            statistics.min_entropy.guess = 0;
            statistics.write();
        }
    }
    return 0;
}


/**
 *
 */
int count_sat(CommandLineArguments &cli) {
    DimacsParser parser(cli.input_filename());
    parser.read();
    PICounter counter;

    counter.set_output_model(cli.output_model_filename());
    counter.set_output_literals(parser.projection_corpus());

    auto solver = new MinisatInterface();
    counter.set_solver(solver);

    counter.activate(parser.clauses(), parser.max_variable());

    if (cli.verbose()) {
        std::cout << "Number of Variables: " << parser.max_variable() << std::endl;
        std::cout << "Number of Clauses: " << parser.clauses().size()
        << std::endl;

        std::cout << "Projection Variables: " << parser.projection_corpus() << std::endl;
    }

    counter.set_verbose(cli.verbose());

    auto count = counter.count_sat(cli.max_models());
    cout << "Model count: " << count << endl;
    return 0;
}

int run(CommandLineArguments &cli) {
    CbmcDimacsParser parser(cli.input_filename(), cli.input_variables(),
                            cli.seed_variables(), cli.output_variables());
    parser.read();

    PICounter counter;

    counter.set_output_model(cli.output_model_filename());

    counter.set_input_variables(parser.input_variables());
    counter.set_output_variables(parser.output_variables());
    counter.set_seed_variables(parser.seed_variables());

    counter.set_input_literals(parser.ivars());
    counter.set_output_literals(parser.ovars());
    counter.set_seed_literals(parser.svars());

    auto solver = new MinisatInterface();
    counter.set_solver(solver);

    counter.activate(parser.clauses(), parser.max_variable());

    if (cli.verbose()) {
        std::cout << "Number of Variables: " << parser.max_variable() << std::endl;
        std::cout << "Number of Clauses: " << parser.clauses().size()
        << std::endl;

        std::cout << "Input Variables: " << parser.ivars() << std::endl;
        std::cout << "Output Variables: " << parser.ovars() << std::endl;
        std::cout << "Seed Variables: " << parser.svars() << std::endl;

    }

    if (cli.statistics()) {
        fstatistics.open("statistics.csv");
        statistics.header();
    }

    counter.set_verbose(cli.verbose());

    switch (cli.mode()) {
        case OPERATION_MODE_DETERMINISTIC:
            return count_deterministic(cli, counter);

        case OPERATION_MODE_NDETERMINISTIC:
            return ndet(cli, counter);

        case OPERATION_MODE_DETERMINISTIC_ITERATIVE:
            return det_iter(cli, counter);

        case OPERATION_MODE_DETERMINISTIC_SUCCESSIVE:
            return det_succ(cli, counter);

        case OPERATION_MODE_DETERMINISTIC_SHUFFLE:
            return det_shuffle(cli, counter);

        case OPERATION_MODE_DETERMINISTIC_ITERATIVE_SHARP:
            return det_iter_sharp(cli, counter);
    }

    if (cli.statistics()) {
        fstatistics.close();
    }

    return 0;
}

/**
 *
 */
int main(int argc, char *argv[]) {
    CommandLineArguments commandLineArguments;
    commandLineArguments.initialize(argc, argv);

    if (commandLineArguments.help()) {
        commandLineArguments.printUsage();
        return 0;
    }

    if (commandLineArguments.mode() == OPERATION_MODE_SHARPSAT) {
        return count_sat(commandLineArguments);
    } else {
        return run(commandLineArguments);
    }
}
