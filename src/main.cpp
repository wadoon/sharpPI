#include <iostream>

#include "CommandLineArguments.h"
#include "cbmcparser.h"
#include "util.h"
#include "entropy.h"
#include "PICounter.h"

using namespace std;

/**
 * true if the user want to terminate this program.
 */
bool _user_want_terminate = false;


template<typename T>
ostream& operator<<(ostream& stream, const _Bucket<T> &b) {
    stream << "{" << b.size << ", " << b.closed << "}";
    return stream;
}

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
        T guess, upper_bound, lower_bound;
    };

    /**
     *
     */
    ofstream fileout;

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
     * theoretical maximal inputs
     */
    uint64_t SI;

    /**
     * theoretical maximal outputs
     */
    uint64_t SO;

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
        if (fileout.is_open()) {
            cout << "number_of_iteration"
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

    void open(const string& filename) {
        fileout.open(filename);
        header();
    }

    void close() {
        if (fileout.is_open())
            { fileout.close(); }
    }

    void update(const Buckets& buckets) {
        shannon_entropy.guess       = ::shannon_entropy(number_of_inputs, buckets);
        shannon_entropy.lower_bound = shannon_entropy_lower_bound(buckets, SI, number_of_inputs);
        shannon_entropy.upper_bound = shannon_entropy_upper_bound(buckets, SI, number_of_inputs);
        number_of_inputs = sum_buckets(buckets);
        number_of_outputs = buckets.size();
        //TODO min entropy
    }

    /**
     *
     */
    void writeconsole() {
        cout << num_of_iteration
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

    /**
     * Write the current statistic values into `fstatistics`
     */
    void write() {
        if (fileout.is_open()) {
            cout  << num_of_iteration << "\t" << cpu_time_consumed << "\t"
                  << shannon_entropy.lower_bound << "\t"
                  << shannon_entropy.guess << "\t"
                  << shannon_entropy.upper_bound << "\t"
                  << min_entropy.lower_bound << "\t" << min_entropy.guess
                  << "\t" << min_entropy.upper_bound << "\t"
                  << number_of_inputs << "\t" << number_of_outputs << endl;
        }
    }

} statistics;


//region signal handler
#include <signal.h>

void my_sigterm(int signum) {
    if (signum == SIGTERM) {
        _user_want_terminate = true;
    }
}

void install_handler() {
    struct sigaction sa;
    sa.sa_handler = my_sigterm;
    sigaction(SIGTERM, &sa, NULL);
}
//endregion

/*class CountingMode {
    CountingMode(CommandLineArguments &cli, PICounter &counter)
        : counter(counter), cli(cli) {}

    int operator()() {
        return 0;
    }
    };*/

void write_final_result(uint64_t input_space_size,
                        long double shannon_entropy,
                        long double min_entropy) {
    console()
        << "Input Space Size"       <<  input_space_size << endl
        << "Shannon Entropy H(h|l)" <<  shannon_entropy << endl
        << "Min Entropy"            <<  min_entropy << endl
        ;//        << "Leakage"                <<  leakage << endl;
}

/**
 *
 */
int count_deterministic(CommandLineArguments &cli, PICounter &counter) {
    console() << "Operation Mode: Deterministic NAIVE" << endl;

    auto ret = counter.count_bucket_all();

    if (cli.verbose())
        console() << "Result: " << ret << "\n";

    unsigned long int SI = sum_buckets(ret);

    auto shannon_e = shannon_entropy(SI, ret);
    auto min_e = min_entropy(SI, ret.size());
    auto leakage = log2(SI) - shannon_entropy(SI, ret);

    write_final_result(SI, shannon_e, min_e);
    return 0;
}

/**
 *
 */
int ndet(CommandLineArguments &arguments, PICounter &counter) {
    console() << "Operation Mode: non-deterministic" << endl;

    CounterMatrix m = counter.count_rand();

    unsigned long SI = m.input_count();
    long double shannon_e = m.shannon_entropy();
    long double min_e = m.min_entropy();
    long double leakage = SI - shannon_e;

    write_final_result(SI, shannon_e, min_e);
    return 0;
}

int det_unguided(const CommandLineArguments &cli, PICounter &counter) {
    console() << "Operation Mode: Unguided (deterministic)" << endl;

    uint SO = space_size(counter.get_output_literals().size());
    uint SI = space_size(counter.get_input_literals().size());

    if (SO == 0) {
        throw logic_error("to much possible outputs (>2^32)");
    }

    Buckets ret(SO, {0,false});

    //cout << "Count Limit: " << cli.limit() << endl;

    statistics.SO = SO;
    statistics.SI = SI;

    for (uint i = 0; /*  i < cli.limit() */ true; i++) {

        if (_user_want_terminate) // user requested to terminate
            break;

        double start = cpuTime();
        bool       b = counter.count_unguided(ret);
	double   end = cpuTime();

	if (cli.verbose()) {
		if (b) {
                    console() << "There are sill more input/output relations" << endl;
		} else {
                    console() << "Search was exhaustive" << endl;
		}

	}

	if(cli.verbose() || !b) {
            auto shannon_e = shannon_entropy(SI, ret);
            auto min_e = min_entropy(SI, ret.size());
            auto leakage = log2(SI) - shannon_entropy(SI, ret);
            write_final_result(SI, shannon_e, min_e);
	}

	if (cli.has_statistic()) {
            statistics.num_of_iteration = i;
            statistics.number_of_inputs = sum_buckets(ret);
            statistics.number_of_outputs = ret.size();
            statistics.cpu_time_consumed = end - start;
            statistics.update(ret);
            statistics.write();
	}

	if(!b)
            break;
    }

    return 0;
}

int det_bucket(const CommandLineArguments &cli, PICounter &counter) {
        console() << "Operation Mode: Bucket-wise (deterministic)" << endl;
	//Assume we have 2^|INPUTVARS| possible inputs
	const auto SI = space_size(counter.get_input_literals().size());
	const auto SO = space_size(counter.get_output_literals().size());

	statistics.SO = SO;
	statistics.SI = SI;

	Buckets ret(min(SI,SO));

	bool b = true;

	for (int k = 1; true /*  k <= cli.limit() && b */; k++) {
            double start = get_cpu_time();
            b = counter.count_one_bucket(ret);
            double end = get_cpu_time();

            //            if (cli.verbose())
            //    cout << k << "# Result: " << ret << "\n";


            if(cli.verbose()){
                auto shannon_e = shannon_entropy(SI, ret);
                auto min_e = min_entropy(SI, ret.size());
                auto leakage = log2(SI) - shannon_entropy(SI, ret);

                cout << k << "# Result: " <<
                    "Shannon Entropy H(h|l)" << shannon_e
                     << endl;

                //cout << TB.create_table(results) << endl;
            }

            if (cli.has_statistic()) {
                statistics.num_of_iteration = k;
                statistics.cpu_time_consumed = end - start;
                statistics.update(ret);
                statistics.min_entropy.guess = 0;
                statistics.write();
            }


            if (_user_want_terminate) {
                cout << "Terminate on user request" << endl;
                break;
            }

            if(!b) {
                cout << "Search was exhaustive!" << endl;
                break;
            }
	}
	return 0;
}

int det_sync(const CommandLineArguments &cli, PICounter &counter) {
    cout << "OperationMode: Synced Counting" << endl;
    const unsigned long SO = 1u << counter.get_output_literals().size();
    const unsigned long SI = 1u << counter.get_input_literals().size();
    statistics.SO = SO;
    statistics.SI = SI;

    Buckets ret;
    bool b = true;

    auto labels = counter.prepare_sync_counting(ret);

    for (uint i = 0; true /*  i < cli.limit() && b */; i++) {
        double start = get_cpu_time();
        b = counter.count_sync(labels, ret);
        double end = get_cpu_time();

        auto shannon_e = shannon_entropy(SI, ret);
        auto min_e = min_entropy(SI, ret.size());
        auto leakage = log2(SI) - shannon_entropy(SI, ret);

        if (cli.verbose()){
            cout << "Result: " << ret << "\n";

            vector<vector<string>> results = {{"Input Space Size",       atos(SI)},
                                              {"Shannon Entropy H(h|l)", atos(shannon_e)},
                                              {"Min Entropy",            atos(min_e)},
                                              {"Leakage",                atos(leakage)},};
            //            cout << TB.create_table(results) << endl;
        }

        if (cli.has_statistic()) {
            statistics.num_of_iteration = i;
            statistics.number_of_inputs = sum_buckets(ret);
            statistics.number_of_outputs = ret.size();
            statistics.cpu_time_consumed = end - start;
            statistics.update(ret);
            statistics.min_entropy.guess = 0;
            statistics.write();
        }


        if (_user_want_terminate) {
            console() << "Terminate on user request" << endl;
            break;
        }
    }

    if (!b) {
        console() << "Search was exhaustive" << endl;
    }

    return 0;
}

/**
 *
 */
int det_bucket_sharp(CommandLineArguments &cli, PICounter &counter) {
    console() << "Operation Mode: Deterministic ITER" << endl;
    const uint64_t SI = space_size(counter.get_input_literals().size());
    const uint64_t SO = space_size(counter.get_output_literals().size());

    statistics.SO = SO;
    statistics.SI = SI;

    Buckets ret;

    bool b = true;
    for (int k = 1; /* k <= cli.limit() && b */ true; k++) {
        double start = get_cpu_time();
        b = counter.count_one_bucket_sharp(ret, cli.input_filename());
        double end = get_cpu_time();

        if (cli.verbose()) {
            console() << k << "# Result: " << ret << "\n";
        }

        auto shannon_e = shannon_entropy(SI, ret);
        auto min_e = min_entropy(SI, ret.size());
        auto leakage = log2(SI) - shannon_entropy(SI, ret);

        vector<vector<string>> results = {{"Input Space Size",       atos(SI)},
                                          {
                                              "Shannon Entropy H(h|l)", atos(shannon_e)},
                                          {"Min Entropy",
                                           atos(min_e)},
                                          {"Leakage",                atos(leakage)},};

        //cout << TB.create_table(results) << endl;

        if (cli.has_statistic()) {
            statistics.num_of_iteration = k;
            statistics.number_of_inputs = sum_buckets(ret);
            statistics.number_of_outputs = ret.size();
            statistics.cpu_time_consumed = end - start;
            statistics.update(ret);
            statistics.min_entropy.guess = 0;
            statistics.write();
        }


        if (_user_want_terminate) {
            console() << "Terminate on user request" << endl;
            break;
        }

        if(!b) {
            cout << "Search was exhaustive!" << endl;
        }
    }
    return 0;
}


/**
 *
 */
int count_sat(CommandLineArguments &cli) {
    console() << "Mode: #SAT" << endl;

    DimacsParser parser(cli.input_filename());
    parser.read();
    PICounter counter;

    counter.set_output_model(cli.statistic_filename());
    counter.set_output_literals(parser.projection_corpus());

    auto solver = new MinisatInterface();
    counter.set_solver(solver);

    counter.activate(parser.clauses(), parser.max_variable());

    if (cli.verbose()) {
        console() << "Number of Variables: "  << parser.max_variable()      << std::endl;
        console() << "Number of Clauses: "    << parser.clauses().size()    << std::endl;
        console() << "Projection Variables: " << parser.projection_corpus() << std::endl;
    }

    counter.set_verbose(cli.verbose());

    auto count = counter.count_sat(cli.max_models());
    console() << "Model count: " << count << endl;
    return 0;
}

int run(CommandLineArguments &cli) {
    CbmcDimacsParser parser(cli.input_filename(), cli.input_variables(),
                            cli.seed_variables(), cli.output_variables());
    parser.read();

    PICounter counter;
    auto solver = new MinisatInterface();
    counter.set_solver(solver);


    counter.set_output_model(cli.statistic_filename());

    counter.set_input_variables(parser.input_variables());
    counter.set_output_variables(parser.output_variables());
    counter.set_seed_variables(parser.seed_variables());

    counter.set_input_literals(parser.ivars());
    counter.set_output_literals(parser.ovars());
    counter.set_seed_literals(parser.svars());

    counter.activate(parser.clauses(), parser.max_variable());

    if (cli.verbose()) {
        console() << "Number of Variables: " << parser.max_variable() << std::endl;
        console() << "Number of Clauses: " << parser.clauses().size() << std::endl;
        console() << "Input Variables: " << parser.ivars() << std::endl;
        console() << "Output Variables: " << parser.ovars() << std::endl;
        console() << "Seed Variables: " << parser.svars() << std::endl;
        console() << "Output limit: " << cli.max_models() << std::endl;
    }

    if (cli.has_statistic()) {
        console() << "write statistics to " << cli.statistic_filename() << endl;
        statistics.open(cli.statistic_filename());
    }

    counter.set_verbose(cli.verbose());

    switch (cli.mode()) {
    case OperationMode::DBUCKETALL:
        return count_deterministic(cli, counter);

    case OperationMode::NRAND:
        return ndet(cli, counter);

    case OperationMode::DBUCKET:
        return det_bucket(cli, counter);

    case OperationMode::DSYNC:
        return det_sync(cli, counter);

    case OperationMode::DUNGUIDED:
        return det_unguided(cli, counter);

    case OperationMode::DISHARP:
        return det_bucket_sharp(cli, counter);

    case OperationMode::DSHARP:
        cout << "PROGRAMING ERROR THIS CASE SHOULD HANDLE BEFORE." << endl;
        break;
    }

    statistics.close();
    return 0;
}


#include "version.h"

/**
 *
 */
int main(int argc, char *argv[]) {
    cout
        << "sharpPI -- " << SHARP_PI_VERSION << " from " << SHARP_PI_DATE << endl
        << "           " << GIT_VERSION << "@" << REF_SPEC << endl;

    CommandLineArguments commandLineArguments;
    commandLineArguments.initialize(argc, argv);

    if (commandLineArguments.help()) {
        commandLineArguments.printUsage();
        return 0;
    }


    if (commandLineArguments.mode() == OperationMode::SHARPSAT) {
        return count_sat(commandLineArguments);
    } else {
        return run(commandLineArguments);
    }
}
