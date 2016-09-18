//
// Created by weigl on 11.11.15.
//
#pragma once

#include <stdint.h>
#include <string>

#include "util.h"

#include "core/Solver.h"
#include "core/SolverTypes.h"
#include "utils/System.h"

#ifdef GLUCOSE
using namespace Glucose;
#else
using namespace Minisat;
#endif

#include <boost/filesystem.hpp>

#include "sat.h"
#include <fstream>
#include <iostream>


const std::string COMMAND_APPROXMC   = "approxmc-p.py";
const std::string INDICATOR_APPROXMC = "";

const std::string COMMAND_DHSARP     = "dsharp -noIBCP";
const std::string INDICATOR_DSHARP   = "Models counted after projection:";

const std::string COMMAND_SHARPSAT   = "sharpSAT";
const std::string INDICATOR_SHARPSAT = "";

const std::string COMMAND_CLASP      = "clasp";
const std::string INDICATOR_CLASP    = "Model Count: ";


class SharpSAT {
public:
    virtual ~SharpSAT() { }

    uint64_t operator()(std::string filename) {
        return (*this)(filename.c_str());
    }

    uint64_t operator()(//SolverInterface *solver,
                        const string dimacs,
                        const vector<Lit> &assumption,
                        const vector<Var> &_input_variables) {
        //        auto mi = dynamic_cast<MinisatInterface *>(solver);

        const boost::filesystem::path temp = boost::filesystem::unique_path(
                "/tmp/%%%-%%%.cnf"
        );
        auto tempstr = temp.native();  // optional

        /* code uses minisat toDIMACS functions,
           these functions are in a worse state and we need to take care of signature renaming.
        vec<Lit> assump;
        convert(assumption, assump);
        mi->solver.toDimacs(tempstr.c_str(), assump);

        ofstream file(tempstr, std::ios_base::openmode::_S_app);


        //we need to convert the variables
        file << "cr";
        for (Var v :  _input_variables) {
            file << ' ' << v;

            if (value(c[i]) != l_False)
            fprintf(f, "%s%d ", sign(c[i]) ? "-" : "", mapVar(var(c[i]), map, max)+1);
        }
        */

        ifstream in(dimacs);
        ofstream out(tempstr);

        char buffer[4096];
        while(!in.eof()) {
            in.getline(buffer, 4096);
            if(buffer[0] == 'c') {
                continue;
            }else {
                out << buffer << '\n';
            }
        }

        //assumption
        for(Lit l : assumption) {
            // 1+ because of correction between minisat and dimacs
            out << (sign(l) ? '-' : ' ') << (1+var(l)) << " 0\n";
        }

        out << "cr";
        for (Var v :  _input_variables) {
            // 1+ because of correction between minisat and dimacs
            out  << ' ' << (1+v);
        }
        out << '\n';

        out.close();
        in.close();

        console() << "Written #SAT file: " << tempstr << endl;

        return (*this)(tempstr);
    }

    uint64_t operator()(const char *filename) { return run(filename); }

private:
    virtual uint64_t run(const std::string &filename);

    std::string model_size_indicator;
    std::string command;
};
