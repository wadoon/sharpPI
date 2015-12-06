//
// Created by weigl on 11.11.15.
//

#include "sharpsat.h"

using namespace std;

#include "pstreams-0.8.1/pstream.h"

const string DSHARP = "dsharp -noIBCP ";

uint64_t DSharpSAT::run(const string &filename) {
    auto cmd = DSHARP + filename;
    uint64_t count;
    string FIND = "Models counted after projection:";

    cout << ">>> " << cmd << endl;
    redi::ipstream proc(cmd, redi::pstreams::pstdout);
    std::string line;

    // read child's stderr
    while (std::getline(proc.err(), line)) {
        std::cout << "dsharp: " << line << '\n';

        if (line.find(FIND) != string::npos) {
            count = (uint64_t) atoi(line.substr(FIND.length()).c_str());
        }
    }

    // read child's stdout
    while (std::getline(proc.out(), line))
        std::cout << "dsharp: " << line << '\n';

    return (uint64_t) count;
}