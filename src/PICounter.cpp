//
// Created by weigl on 11.10.15.
//

#include "PICounter.h"
#include <iostream>
#include "entropy.h"

extern bool _user_want_terminate;


void create_assumption(const vector<Var> &vars, uint64_t value,
                       vector<Lit> &assum) {
    for (Var var : vars) {
        bool b = (value & 1) > 0;
        assum.push_back(mkLit(var, b));
        value >>= 1;
    }
}

/**
 *
 *
 **/
void PICounter::prohibit_project(const vector<Var> &variables) {
    vector<Lit> clause = project_model(variables);

    vector<Lit> nclause;
    for (const Lit l : clause) {
        nclause.push_back(~l);
    }

    solver->add_clause(nclause);

#ifdef DEBUG
    printf("Prohibit Model: ");
    for (int i = 0; i < clause.size(); i++)
    printf("% d ", (sign(clause[i]) ? 1 : -1) * var(clause[i]));
    printf("\n");
#endif
    return;
}

vector<Lit> PICounter::project_model(const vector<Var> &variables) {
    vector<Lit> ret;
    for (const Var v : variables) {
        lbool a = solver->model_value(v);
        if (a == l_Undef) {
            printf("variable undefined: %d\n", v);
        } else {
            // sign == true iff v is negated
            ret.push_back(mkLit(v, a == l_False));
        }
    }
#ifdef DEBUG
    /* printf("Assume Output: ");
     for (int i = 0; i < ret.size(); i++)
     printf("%d ", (sign(ret[i]) ? -1 : 1) * var(ret[i]));
     printf("\n");*/
#endif

    return ret;
}

void PICounter::write_output() {
    auto value = interpret(_output_literals);
    model_stream << "\n" << value << " ";
}

void PICounter::write_input() {
    auto value = interpret(_input_literals);
    model_stream << value << " ";
}

vector<uint64_t> PICounter::count_det_compl() {
    vector<uint64_t> found_preimage_sizes;
    bool b = true;
    while (b) {
        b = count_det_iter(found_preimage_sizes);
    }
    return found_preimage_sizes;
}

bool PICounter::count_det_iter(vector<uint64_t> &previous) {
    vector<Lit> assum;

    if (solver->solve(assum)) {
        write_output();
        if (verbose) {
            std::cout << "Output: " << std::endl;
            for (auto &var : _output_variables) {
                std::cout << "\t" << var.variable_name << "[" << var.time
                << "] = " << interpret(var.positions) << std::endl;
            }
        }

        // we fixate the found output
        assum = project_model(_output_literals);

        uint64_t pi = 0;

        bool a;
        do {

            if (verbose) {
                std::cout << "\t\tInput: " << std::endl;
                for (auto &var : _input_variables) {
                    std::cout << "\t\t\t" << var.variable_name << "["
                    << var.time << "] = " << interpret(var.positions)
                    << std::endl;
                }

                for (auto &var : _seed_variables) {
                    std::cout << "\t\t\t" << var.variable_name << "["
                    << var.time << "] = " << interpret(var.positions)
                    << std::endl;
                }
            }

            write_input();

            pi++; // we found the first model, already

            prohibit_project(_input_literals);

            //exclude the found input pattern
            a = solver->solve(assum);
        } while (a);

        previous.push_back(pi);

        //optional exclude output
        prohibit_project(_output_literals);
        return true;
    }
    return false;
}

#include "sharpsat.h"

bool PICounter::count_det_iter_sharp(vector<uint64_t> &previous,
                                     const string &filename) {
    vector<Lit> assum;

    DSharpSAT sharpSAT;

    if (solver->solve(assum)) {
        write_output();
        if (verbose) {
            std::cout << "Output: " << std::endl;
            for (auto &var : _output_variables) {
                std::cout << "\t" << var.variable_name << "[" << var.time
                << "] = " << interpret(var.positions) << std::endl;
            }
        }

        // we fixate the found output
        assum = project_model(_output_literals);

        uint64_t pi = sharpSAT(/*solver*/filename, assum, _input_literals);

        //        Bucket b = {pi, true};

        previous.push_back(pi);

        //optional exclude output
        prohibit_project(_output_literals);
        return true;
    }
    return false;
}

vector<Lit> negate_cube(vector<Lit> cube) {
    vector<Lit> clause;
    for (const Lit l : cube) {
        clause.push_back(~l);
    }
    return clause;
}


LabelList PICounter::prepare_sync_counting(vector<bool> &closed,
                                           vector<uint64_t> &count_table) {

    LabelList label_literals;
    std::vector<Lit> assump;
    //    std::fill(closed.begin(), closed.end(), true);

    while( solver->solve(assump) ) {
        // get the model
        vector<Lit> model = negate_cube(project_model(_output_literals));

        // define a label literal
        Var label = solver->new_variable();
        // label to clause
        model.push_back(mkLit(label,true));

        // add clause to solver
        solver->add_clause(model);

        //if label is true, that ~label is false and the
        //model clause takes action and prohibits
        assump.push_back(mkLit(label,false));

        auto output = interpret(_output_literals);

        closed.push_back(false);

        // book the found input right
        count_table.push_back(1);
        prohibit_project(_input_literals);

        //save label literals
        label_literals.push_back({output,label});
    }

    return label_literals;
}

bool PICounter::count_sync(LabelList &labels,
                           vector<bool> &closed,
                           vector<uint64_t> &count_table) {
    bool one_open = false;

    //forbid every clause, by adding positive label literal
    vector<Lit> assumption;
    for(auto const item :  labels) {
        assumption.push_back(mkLit(item.second, false));
    }


    for(int i = 0; i < labels.size(); i++) {
        if(closed[i])
            continue;

        //enable this output
        assumption[i] = ~ assumption[i];

        if(solver->solve(assumption)) {
            auto output_value = interpret(_output_literals);
            //cout << output_value << " " << labels[i].first << endl;

            //book input
            prohibit_project(_input_literals);
            count_table[i] += 1;

            one_open=true;
        }else{
            closed[i] = true;
        }

        //disable this  output
        assumption[i] = ~ assumption[i];
    }
    return one_open;
}

bool PICounter::count_unstructured(uint64_t limit,
                                   vector<uint64_t> &count_table) {
    vector<Lit> assum;
    for (uint64_t i = 0; i < limit; i++) {
        if (!count_unstructured(count_table))
            return false;
    }
    return true;
}

bool PICounter::count_unstructured(vector<uint64_t> &count_table) {
    if (solver->solve()) {
        uint output = (uint) interpret(_output_literals);
        prohibit_project(_input_literals);
        count_table.at(output) += 1;
        return true;
    }
    return false;
}

CounterMatrix PICounter::countrand() {
    vector<uint64_t> found_preimage_sizes;
    vector<Lit> assum;

    vector<Var> seed_and_input;

	for(auto& v: _input_literals) {
		seed_and_input.push_back(v);
	}

	for(auto& v: _seed_literals) {
		seed_and_input.push_back(v);
	}

	/*
		seed_and_input.insert(seed_and_input.cbegin(), _input_literals.cbegin(),
		_input_literals.cend());

		seed_and_input.insert(seed_and_input.cbegin(), _seed_literals.cbegin(),
		_seed_literals.cend());
		*/

	CounterMatrix cm(_input_literals.size(), _output_literals.size());

	while (solver->solve(assum)) {
		write_output();
        if (verbose) {
            std::cout << "Output: " << std::endl;
            for (auto &var : _output_variables) {
                std::cout << "\t" << var.variable_name << "[" << var.time
                << "] = " << interpret(var.positions) << std::endl;
            }
        }

        // we fixate the found output
        assum = project_model(_output_literals);

        uint64_t pi = 0;

        bool a;
        do {

            if (verbose) {
                std::cout << "\t\tInput: " << std::endl;
                for (auto &var : _input_variables) {
                    std::cout << "\t\t\t" << var.variable_name << "["
                    << var.time << "] = " << interpret(var.positions)
                    << std::endl;
                }

                for (auto &var : _seed_variables) {
                    std::cout << "\t\t\t" << var.variable_name << "["
                    << var.time << "] = " << interpret(var.positions)
                    << std::endl;
                }
            }

            write_input();

            cm.count(interpret(_input_literals), interpret(_output_literals));

            pi++; // we found the first model, already

            prohibit_project(seed_and_input);

            //exclude the found input pattern
            a = solver->solve(assum);
        } while (a);

        found_preimage_sizes.push_back(pi);

        //optional exclude output
        prohibit_project(_output_literals);

        //release assumption
        assum.clear();
    }

    return cm;
}

void PICounter::activate(const vector<vector<int>> &clauses, int max_var) {
    solver->ensure_variables(max_var);

    for (vector<int> clause : clauses) {
        vector<Lit> lclause;
        for (int i : clause) {
            int l = abs(i) - 1;
            lclause.push_back(mkLit(Var(l), i < 0));
        }
        solver->add_clause(lclause);
    }
}

void MinisatInterface::ensure_variables(int max_var) {
    while (solver.nVars() <= max_var)
        solver.newVar();
}

uint64_t PICounter::count_sat( uint64_t max_count ) {
    uint64_t pi = 0;

    if (_output_literals.size() == 0) {
        for (int i = 1; i < solver->num_variables(); i++)
            _output_literals.push_back(i);
    }

    while (solver->solve() && ! _user_want_terminate ) {
        ++pi;
        prohibit_project(_output_literals);

		if(pi >= max_count)
			break;
    }
    return pi;
}
