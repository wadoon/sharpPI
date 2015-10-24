#include <iostream>

#include "termbox.h"
#include "CommandLineArguments.h"
#include "cbmc/cbmcparser.h"
#include "util.h"
#include "PICounter.h"

using namespace std;

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
    uint outputs = 1 << counter.get_output_literals().size();
    vector<bool> closed(outputs, false);
    vector<uint64_t> ret(outputs, 0);
    bool b = counter.count_unstructured(10, closed, ret);
    return 0;
}

int det_iter(const CommandLineArguments &cli, PICounter &counter) {
    cout << "Operation Mode: Deterministic ITER" << endl;
    vector<uint64_t> ret((unsigned long) (1 << counter.get_input_literals().size()));

    bool b = true;
    for (int k = 1; b; k++) {
        b = counter.count_det_iter(ret);

        cout << k << "# Result: " << ret << "\n";

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
    return 0;
}

int det_succ(const CommandLineArguments &arguments, PICounter &counter) {
    cout << "OperationMode: DETERMINISTIC successive" << endl;
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