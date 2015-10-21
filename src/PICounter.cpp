//
// Created by weigl on 11.10.15.
//

#include "PICounter.h"
#include <iostream>
#include "entropy.h"
/**
 *
 *
 **/
void PICounter::prohibit_project(const vector<Var> &variables) {
    vector<Lit> clause = project_model(variables);

    vector<Lit> nclause;
    for (const Lit l:clause) {
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
    for (const Var v: variables) {
        lbool a = solver->model_value(v);
        if (a == l_Undef) {
            printf("variable undefined: %d\n", v);
        }
        else {
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

vector<uint64_t> PICounter::count() {
    vector<uint64_t> found_preimage_sizes;
    vector<Lit> assum;

    vector<Var> seed_and_input;
    seed_and_input.insert(seed_and_input.cbegin(),
                          _input_literals.cbegin(), _input_literals.cend());
    seed_and_input.insert(seed_and_input.cbegin(),
                          _seed_literals.cbegin(), _seed_literals.cend());


    CounterMatrix cm(_input_literals.size(), _output_literals.size());


    while (solver->solve(assum)) {
        write_output();
        if (verbose) {
            std::cout << "Output: " << std::endl;
            for (auto &var : _output_variables) {
                std::cout << "\t" << var.variable_name << "[" << var.time << "] = " <<
                interpret(var.positions) << std::endl;
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
                    std::cout << "\t\t\t" << var.variable_name << "[" << var.time << "] = " <<
                    interpret(var.positions) << std::endl;
                }

                for (auto &var : _seed_variables) {
                    std::cout << "\t\t\t" << var.variable_name << "[" << var.time << "] = " <<
                    interpret(var.positions) << std::endl;
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


    return found_preimage_sizes;
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

void PICounter::set_solver(SolverInterface *s) {
    this->solver = s;
}

void MinisatInterface::ensure_variables(int max_var) {
    while (solver.nVars() <= max_var)
        solver.newVar();
}

