#include <iostream>

#include "version.h"
#include "CommandLineArguments.h"
#include "cbmcparser.h"
#include "util.h"
#include "entropy.h"
#include "PICounter.h"
#include "sharpsat.h"

#include "stat.h"

using namespace std;

/**
 * true if the user want to terminate this program.
 */
bool _user_want_terminate = false;


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


void write_final_result(const Buckets& buckets) {
    auto input_space_size = sum_buckets(buckets);
    auto output_space_size = sum_outputs(buckets);
    auto se = shannon_entropy(input_space_size, buckets);
    auto me = min_entropy(input_space_size, output_space_size);
    auto leakage = log2(input_space_size) - se;

    console() << "Input Space Size       " <<  input_space_size << endl;
    console() << "Shannon Entropy H(h|l) " <<  se << endl;
    console() << "Min Entropy            " <<  me << endl;
    console() << "Leakage                " <<  leakage << endl;
}

void write_final_result(const CounterMatrix& m) {
    unsigned long SI = m.input_count();
    long double se = m.shannon_entropy();
    long double me = m.min_entropy();
    long double leakage = SI - se;

    console() << "Input Space Size"       <<  SI << endl;
    console() << "Shannon Entropy H(h|l)" <<  se << endl;
    console() << "Min Entropy"            <<  me << endl;
    console() << "Leakage"                <<  leakage << endl;
}

/**
 *
 */
int count_deterministic(CommandLineArguments &cli, PICounter &counter) {
    console() << "Operation Mode: Deterministic NAIVE" << endl;
    auto ret = counter.count_bucket_all();
    if (cli.verbose())
        console() << "Result: " << ret << "\n";
    write_final_result(ret);
    return 0;
}

/**
 *
 */
int ndet(CommandLineArguments &arguments, PICounter &counter) {
    console() << "Operation Mode: non-deterministic" << endl;
    CounterMatrix m = counter.count_rand();
    write_final_result(m);
    return 0;
}

int det_unguided(const CommandLineArguments &cli, PICounter &counter) {
    console() << "Operation Mode: Unguided (deterministic)" << endl;
    const auto SO = space_size(counter.get_output_literals().size());
    const auto SI = space_size(counter.get_input_literals().size());

    if (SO == 0) {
        throw logic_error("to much possible outputs (>2^32)");
    }

    Buckets ret(SO, {0,false});

    counter.stat().SO = SO;
    counter.stat().SI = SI;

    bool b;
    for (uint i = 0; true; i++) {
        if (_user_want_terminate) // user requested to terminate
            break;

        b = counter.count_unguided(ret);

	if(cli.verbose()) {
            console() << "Round: " << i << ":" << endl;
            write_final_result(ret);
	}

	if(!b) {
            break;
        }
    }

    if (b) {
        console() << "There are sill more input/output relations" << endl;
    } else {
        console() << "Search was exhaustive" << endl;
    }

    write_final_result(ret);

    return 0;
}

int det_bucket(const CommandLineArguments &cli, PICounter &counter) {
        console() << "Operation Mode: Bucket-wise (deterministic)" << endl;
	const auto SI = space_size(counter.get_input_literals().size());
	const auto SO = space_size(counter.get_output_literals().size());

	counter.stat().SO = SO;
	counter.stat().SI = SI;

        Buckets ret(SO, {0, false});


	bool b = true;

	for (int k = 1; true; k++) {
            if (_user_want_terminate) {
                console() << "Terminate on user request" << endl;
                break;
            }

            b = counter.count_one_bucket(ret);

            if(cli.verbose()) {
                console() << "Round: " << k << ":" << endl;
                write_final_result(ret);
            }

            if(!b) {
                break;
            }
	}

        if (b) {
            console() << "There are sill more input/output relations" << endl;
        } else {
            console() << "Search was exhaustive" << endl;
        }

        write_final_result(ret);

	return 0;
}

int det_sync(const CommandLineArguments &cli, PICounter &counter) {
    cout << "OperationMode: Synced Counting" << endl;
    const auto SO = 1u << counter.get_output_literals().size();
    const auto SI = 1u << counter.get_input_literals().size();

    counter.stat().SO = SO;
    counter.stat().SI = SI;

    Buckets ret;
    bool b = true;

    auto labels = counter.prepare_sync_counting(ret);

    for (uint i = 0; true /*  i < cli.limit() && b */; i++) {
        b = counter.count_sync(labels, ret);

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
    console() << "Operation Mode: Bucket with #SAT-p" << endl;
    const uint64_t SI = space_size(counter.get_input_literals().size());
    const uint64_t SO = space_size(counter.get_output_literals().size());

    counter.stat().SO = SO;
    counter.stat().SI = SI;

    Buckets ret(SO, {0,false});

    SharpSAT ssat( cli.input_filename(), cli.sharpsat_indicator(), cli.sharpsat_command());

    bool b = true;
    for (int k = 1; /* k <= cli.limit() && b */ true; k++) {
        b = counter.count_one_bucket_sharp(ret, ssat);

        if (cli.verbose()) {
            console() << k << "# Result: " << ret << "\n";
        }

        if (_user_want_terminate) {
            console() << "Terminate on user request" << endl;
            break;
        }

        if(!b) {
            cout << "Search was exhaustive!" << endl;
            break;
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

    if(_user_want_terminate) {
        console() << "User terminated " << endl;
    }

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

    counter.set_input_variables(parser.input_variables());
    counter.set_output_variables(parser.output_variables());
    counter.set_seed_variables(parser.seed_variables());

    counter.set_input_literals(parser.ivars());
    counter.set_output_literals(parser.ovars());
    counter.set_seed_literals(parser.svars());

    counter.activate(parser.clauses(), parser.max_variable());

    counter.set_verbose(cli.verbose());
    counter.set_tolerance(cli.tolerance());


    if (cli.verbose()) {
        console() << "Number of Variables: " << parser.max_variable() << std::endl;
        console() << "Number of Clauses: "   << parser.clauses().size() << std::endl;
        console() << "Input Variables: "     << parser.ivars() << std::endl;
        console() << "Output Variables: "    << parser.ovars() << std::endl;
        console() << "Seed Variables: "      << parser.svars() << std::endl;
        console() << "Output limit: "        << cli.max_models() << std::endl;
        console() << "Tolerance: "           << cli.tolerance() << std::endl;
    }

    if (cli.has_statistic()) {
        console() << "write statistics to " << cli.statistic_filename() << endl;
        counter.enable_stat(cli.statistic_filename());
    }


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

    case OperationMode::BUCKETSHARP:
        return det_bucket_sharp(cli, counter);

        //    case OperationMode::DSHARP:
        //cout << "PROGRAMING ERROR THIS CASE SHOULD HANDLE BEFORE." << endl;
        //break;
    }


    return 0;
}


/**
 *
 */
int main(int argc, char *argv[]) {
    cout << "sharpPI -- " << SHARP_PI_VERSION << " from " << SHARP_PI_DATE << endl;

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
