//
// Created by weigl on 11.10.15.
//
#include <iostream>

#include "PICounter.h"
#include "entropy.h"
#include "util.h"
#include "stat.h"
#include "sharpsat.h"


extern bool _user_want_terminate;


void BucketListWriter::header(const PICounter* counter) {
  write_header(counter->_input_variables, "i");
  write_header(counter->_seed_variables, "s");
  write_header(counter->_output_variables, "o");
}


void BucketListWriter::write(MinisatInterface* solver) {
  if(!active) return;

  for(auto v : variables) {
    lbool val =  solver->model_value(v);
    if(val == l_True) 
      out << '1';
    else 
      if(val == l_False)
	out << '0';
      else if(val == l_Undef)
	out << '0';
  }
  out << "\n";
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


Buckets PICounter::count_bucket_all() {
    Buckets found_preimage_sizes(space_size(_output_literals.size()), {0, false});
    bool b = true;
    while (b) {
        b = count_one_bucket(found_preimage_sizes);
    }
    return found_preimage_sizes;
}


void PICounter::stat_point(const Buckets& result) {
    _stat.cpu_time_consumed = get_time_diff(last);
    _stat.update(result);
    _stat.update(solver);
    _stat.write();
    if(_listwriter.active)
      _listwriter.write(solver);
}

bool PICounter::count_one_bucket(Buckets& previous) {
    vector<Lit> assum;

    if(solver->solve(assum)) {
        if (verbose) {
            std::cout << "Output: " << std::endl;
            for (auto &var : _output_variables) {
                std::cout <<
                    "\t" << var.variable_name
                         << "[" << var.time << "] = "
                         << interpret(var.positions)
                         << std::endl;
            }
        }
        // we fixate the found output
        assum = project_model(_output_literals);


        Bucket& bucket = previous.at(interpret(_output_literals));
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

            bucket.size++; // we found the first model, already
            prohibit_project(_input_literals);
            stat_point(previous);

            if(_user_want_terminate) {
                console() << "User termination. " << endl;
                break;
            }

            //exclude the found input pattern
            a = solver->solve(assum);
        } while (a);

        if(tolerance_met(previous)) {
            console() << "Tolerance condition met" << endl;
            return false;
        }

        //optional exclude output
        prohibit_project(_output_literals);
        return true;
    }
    return false;
}


bool PICounter::count_one_bucket_sharp(Buckets& buckets,
                                       SharpSAT& sharpSAT) {
    vector<Lit> assum;

    if (solver->solve(assum)) {
      if (verbose) {
	std::cout << "Output: " << std::endl;
	for (auto &var : _output_variables) {
	  std::cout << "\t" << var.variable_name << "[" << var.time
		    << "] = " << interpret(var.positions) << std::endl;
	}
      }

      // we fixate the found output
      assum = project_model(_output_literals);

      uint64_t pi = sharpSAT(assum, _input_literals);
      buckets.at(interpret(_output_literals)).size = pi;

      // *not* optional exclude output
      prohibit_project(_output_literals);

      stat_point(buckets);

      if(_user_want_terminate) {
	console() << "User termination. " << endl;
	return false;
      }

      if(tolerance_met(buckets)) {
	console() << "Tolerance condition met" << endl;
	return false;
      }

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

LabelList PICounter::prepare_sync_counting(Buckets& buckets) {
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

        // book the found input right
        buckets.push_back({1,false});
        prohibit_project(_input_literals);

        //save label literals
        label_literals.push_back({output,label});
    }

    return label_literals;
}

bool PICounter::count_sync(const LabelList &labels, Buckets& buckets) {
    bool one_open = false;

    //forbid every clause, by adding positive label literal
    vector<Lit> assumption;
    for(auto const item :  labels) {
        assumption.push_back(mkLit(item.second, false));
    }


    for(int i = 0; i < labels.size(); i++) {
        if(buckets[i].closed)
            continue;

        //enable this output
        assumption[i] = ~ assumption[i];

        if(solver->solve(assumption)) {
            auto output_value = interpret(_output_literals);
            //cout << output_value << " " << labels[i].first << endl;

            //book input
            prohibit_project(_input_literals);
            buckets[i].size += 1;

            one_open = true;
        }else{
            buckets[i].closed = true;
        }

        //disable this  output
        assumption[i] = ~ assumption[i];


        if(_user_want_terminate) {
            return false;
        }

        if(tolerance_met(buckets)) {
            console() << "Tolerance condition met" << endl;
            return false;
        }
    }
    return one_open;
}

bool PICounter::count_unguided(Buckets& buckets) {
    vector<Lit> assum;
    stat_point(buckets);
    while(true){
        if (!count_unstructured_one(buckets))
            return false;
    }
    stat_point(buckets);
    return true;
}

bool PICounter::count_unstructured_one(Buckets& buckets) {
    if (solver->solve()) {
        uint output = (uint) interpret(_output_literals);
        prohibit_project(_input_literals);
        buckets[output].size++;

        if(_user_want_terminate) {
            return false;
        }

        if(tolerance_met(buckets)) {
            console() << "Tolerance condition met" << endl;
            return false;
        }

        return true;
    }

    return false;
}

CounterMatrix PICounter::count_rand() {
    vector<Lit> assum;
    vector<Var> seed_and_input;

    for(auto& v: _input_literals) {
        seed_and_input.push_back(v);
    }

    for(auto& v: _seed_literals) {
        seed_and_input.push_back(v);

        CounterMatrix cm(_input_literals.size(), _output_literals.size());

        while (solver->solve(assum)) {
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

                cm.count(interpret(_input_literals), interpret(_output_literals));

                pi++; // we found the first model, already

                prohibit_project(seed_and_input);

                //exclude the found input pattern
                a = solver->solve(assum);

            } while (a);

            //optional exclude output
            prohibit_project(_output_literals);

            //release assumption
            assum.clear();
        }

        return cm;
    }
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

#ifdef DECISION_NOT_ON_INPUTS
    MinisatInterface* solver = dynamic_cast<MinisatInterface*>(this->solver);
    for(Var v : _input_literals)
    	solver->solver.setDecisionVar(v, false);
#endif
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

        if(verbose) {
            console() << "No projection found. Project all variables!" << endl;
        }
    }

    while (solver->solve() && ! _user_want_terminate ) {
        ++pi;
        prohibit_project(_output_literals);
        if(pi >= max_count)
            break;
    }

    return pi;
}
