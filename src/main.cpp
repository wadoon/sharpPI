#include <iostream>

#include "termbox.h"
#include "CommandLineArguments.h"
#include "cbmc/cbmcparser.h"
#include "util.h"
#include "PICounter.h"

using namespace std;

//globals
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


int det_naive(CommandLineArguments &cli, PICounter &counter) {
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

    vector<vector<string>> results = {
            {"Input Space Size",       atos(SI)},
            {"Shannon Entropy H(h|l)", atos(shannon_e)},
            {"Min Entropy",            atos(min_e)},
            {"Leakage",                atos(leakage)},
    };

    cout << TB.create_table(results) << endl;
}

int ndet(CommandLineArguments &arguments, PICounter &counter) {
    CounterMatrix m = counter.countrand();

    unsigned long SI = m.input_count();
    long double shannon_e = m.shannon_entropy();
    long double min_e = m.min_entropy();
    long double leakage = SI - shannon_e;

    vector<vector<string>> results = {
            {"Input Space Size",       atos(SI)},
            {"Shannon Entropy H(h|l)", atos(shannon_e)},
            {"Min Entropy",            atos(min_e)},
            {"Leakage",                atos(leakage)},
    };

    cout << TB.create_table(results) << endl;
    return 0;
}

int det_shuffle(const CommandLineArguments &cli, PICounter &counter) {
    cout << "Operation Mode: DETERMINISTIC SHUFFLE" << endl;
    uint outputs = 1 << counter.get_output_literals().size();

    if (outputs == 0) {
        throw logic_error("to much possible outputs (>32)");
    }

    vector<bool> closed(outputs, false);
    vector<uint64_t> ret(outputs, 0);

    cout << "Count Limit: " << cli.limit() << endl;

    bool b = counter.count_unstructured(cli.limit(), closed, ret);


    if (b) {
        cout << "There are sill more input/output relations" << endl;
    }
    else {
        cout << "Search was exhaustive" << endl;
    }

    if (cli.verbose())
        cout << "Result: " << ret << "\n";

    //Assume we have 2^|INPUTVARS| possible inputs
    unsigned long int SI = 1 << counter.get_input_literals().size();


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

    return 0;
}


struct Statistics {
    template<typename T>
    struct Guess {
        T lower_bound, guess, upper_bound;
    };

    uint num_of_iteration, number_of_inputs, number_of_outputs;
    double cpu_time_consumed;

    Guess<double> shannon_entropy, min_entropy;

    void header() {
        if (fstatistics.is_open()) {
            fstatistics
            << "number_of_iteration"
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

    void write() {
        if (fstatistics.is_open()) {
            fstatistics
            << num_of_iteration
            << "\t" << cpu_time_consumed
            << "\t" << shannon_entropy.lower_bound
            << "\t" << shannon_entropy.guess
            << "\t" << shannon_entropy.upper_bound
            << "\t" << min_entropy.lower_bound
            << "\t" << min_entropy.guess
            << "\t" << min_entropy.upper_bound
            << "\t" << number_of_inputs
            << "\t" << number_of_outputs
            << endl;


        }
    }

} statistics;


int det_iter(const CommandLineArguments &cli, PICounter &counter) {
    cout << "Operation Mode: Deterministic ITER" << endl;
    //Assume we have 2^|INPUTVARS| possible inputs
    unsigned long int SI = (unsigned long) (1 << counter.get_input_literals().size());
    vector<uint64_t> ret(SI);

    bool b = true;
    for (int k = 1; k<=cli.limit()&&b; k++) {

        double start = get_cpu_time();
        b = counter.count_det_iter(ret);
        double end = get_cpu_time();

        if (cli.verbose())
            cout << k << "# Result: " << ret << "\n";

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
            statistics.num_of_iteration = k;
            statistics.number_of_inputs = SI;
            statistics.number_of_outputs = ret.size();
            statistics.cpu_time_consumed = end - start;
            statistics.shannon_entropy.guess = shannon_e;
            statistics.min_entropy.guess = min_e;
            statistics.write();
        }
    }
    return 0;
}

int det_succ(const CommandLineArguments &cli, PICounter &counter) {
    cout << "OperationMode: DETERMINISTIC SUCCESSIVE" << endl;

    const long output_sz = 1 << counter.get_output_literals().size();
    const unsigned long SI = 1 << counter.get_input_literals().size();

    vector<bool> closed(output_sz, false);
    vector<uint64_t> ret(output_sz, 0);

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

        vector<vector<string>> results = {
                {"Input Space Size",       atos(SI)},
                {"Shannon Entropy H(h|l)", atos(shannon_e)},
                {"Min Entropy",            atos(min_e)},
                {"Leakage",                atos(leakage)},
        };
        cout << TB.create_table(results) << endl;


        if (cli.statistics()) {
            statistics.num_of_iteration = i;
            statistics.number_of_inputs = SI;
            statistics.number_of_outputs = ret.size();
            statistics.cpu_time_consumed = end - start;
            statistics.shannon_entropy.guess = shannon_e;
            statistics.min_entropy.guess = min_e;
            statistics.write();
        }
    }

    if (!b) {
        cout << "Search was exhaustive" << endl;
    }

    return 0;
}


int run(CommandLineArguments &cli) {
    CbmcDimacsParser parser(cli.input_filename(),
                            cli.input_variables(),
                            cli.seed_variables(),
                            cli.output_variables());
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

    counter.activate(parser.clauses(), parser.max_var);

    if (cli.verbose()) {
        std::cout << "Number of Variables: " << parser.max_var << std::endl;
        std::cout << "Number of Clauses: " << parser.clauses().size() << std::endl;

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
            return det_naive(cli, counter);

        case OPERATION_MODE_NDETERMINISTIC:
            return ndet(cli, counter);

        case OPERATION_MODE_DETERMINISTIC_ITERATIVE:
            return det_iter(cli, counter);

        case OPERATION_MODE_DETERMINISTIC_SUCCESSIVE:
            return det_succ(cli, counter);

        case OPERATION_MODE_DETERMINISTIC_SHUFFLE:
            return det_shuffle(cli, counter);
    }

    if (cli.statistics()) {
        fstatistics.close();
    }

    return 0;
}

int main(int argc, char *argv[]) {
    CommandLineArguments commandLineArguments;
    commandLineArguments.initialize(argc, argv);

    if (commandLineArguments.help()) {
        commandLineArguments.printUsage();
        return 0;
    }

    return run(commandLineArguments);
}