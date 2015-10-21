#include <iostream>

#include "termbox.h"
#include "CommandLineArguments.h"
#include "cbmc/cbmcparser.h"
#include "util.h"
#include "PICounter.h"

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

using namespace std;

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

    counter.set_verbose(cli.verbose());
    std::vector<uint64_t> ret = counter.count();

    std::cout << "Result: " << ret << "\n";

    //Assume we have 2^|INPUTVARS| possible inputs
    unsigned long int SI = input_space(ret);


    auto shannon_e = shannon_entropy(SI, ret);
    auto min_e = min_entropy(SI, ret.size());
    auto leakage = log2(SI) - shannon_entropy(SI, ret);

    std::vector<std::vector<std::string>> results = {
            {"Input Space Size",       atos(SI)},
            {"Shannon Entropy H(h|l)", atos(shannon_e)},
            {"Min Entropy",            atos(min_e)},
            {"Leakage",                atos(leakage)},
    };

    std::cout << TB.create_table(results) << std::endl;

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