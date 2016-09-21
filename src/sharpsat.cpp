//
// Created by weigl on 11.11.15.
//

#include "sharpsat.h"

using namespace std;

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include "util.h"

void exec(const string& cmd, stringstream& output) {
    char buffer[128];
    std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe)
        throw std::runtime_error("popen() failed!");

    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != NULL){
            debug() << buffer;
            output << buffer;
        }
    }
}


uint64_t find_models_in_output(stringstream& output, const string& find) {
    output.seekg(0, ios_base::beg); // reset pointer to the beginning
    std::string line;

    while (getline(output, line)) {
        if (line.find(find) != string::npos) {
            return (uint64_t) atoi(line.substr(find.length()).c_str());
        }
    }
    throw runtime_error("no model count found in output");
}

uint64_t SharpSAT::run(const string &filename) {
    auto cmd = command + " " + filename;
    console() << ">>> " << cmd << endl;
    stringstream buffer;
    exec(cmd, buffer);
    return find_models_in_output(buffer, model_size_indicator);
}
