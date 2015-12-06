//
// Created by weigl on 11.11.15.
//

#ifndef SHARPPIC_SHARPSAT_CPP_H
#define SHARPPIC_SHARPSAT_CPP_H

#include <stdint.h>
#include <string>


#include "core/Solver.h"
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

class SharpSAT {
public:
    virtual ~SharpSAT() { }

    uint64_t operator()(std::string filename) {
        return (*this)(filename.c_str());
    }

    uint64_t operator()(SolverInterface *solver, const vector<Lit> &assumption,
                        const vector<Var> &_input_variables) {
        auto mi = dynamic_cast<MinisatInterface *>(solver);

        const boost::filesystem::path temp = boost::filesystem::unique_path(
                "/tmp/%%%-%%%.cnf"
        );
        auto tempstr = temp.native();  // optional

        vec<Lit> assump;
        convert(assumption, assump);
        mi->solver.toDimacs(tempstr.c_str(), assump);

        ofstream file(tempstr, std::ios_base::openmode::_S_app);

        file << "cr";
        for (Var v :  _input_variables) {
            file << ' ' << v;
        }

        cout << "Written #SAT file: " << tempstr << endl;

        file.close();

        return (*this)(tempstr);
    }

    uint64_t operator()(const char *filename) { return run(filename); }

private:
    virtual uint64_t run(const std::string &filename) = 0;
};

class DSharpSAT : public SharpSAT {
public:
    virtual ~DSharpSAT() {}
private:
    virtual  uint64_t run(const std::string &filename) override;
};


#endif //SHARPPIC_SHARPSAT_CPP_H
