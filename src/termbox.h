#pragma once

#include <vector>
#include <sstream>

using namespace std;

const string SINGLE_HEDGE = "─";
const string SINGLE_EDGE_TOP_LEFT = "┌";
const string SINGLE_EDGE_TOP_RIGHT = "┐";
const string SINGLE_EDGE_BOT_LEFT = "└";
const string SINGLE_EDGE_BOT_RIGHT = "┘";
const string SINGLE_CROSS_TOP = "┬";
const string SINGLE_CROSS_BOT = "┴";
const string SINGLE_CROSS_INNER = "┼";
const string SINGLE_CROSS_LEFT = "├";
const string SINGLE_CROSS_RIGHT = "┤";
const string SINGLE_VEDGE = "│";

class TermBox {
public:
    TermBox();

    string create_box(const vector<string> &lines,
                      const string &title = "",
                      size_t width = 80,
                      const string &footer = "") const;

    string create_table(const vector<int> &sizes,
                        const vector<string> &header,
                        const vector<vector<string>> &content) const;

    string HEDGE = "─";
    string EDGE_TOP_LEFT = "┌";
    string EDGE_TOP_RIGHT = "┐";
    string EDGE_BOT_LEFT = "└";
    string EDGE_BOT_RIGHT = "┘";
    string CROSS_TOP = "┬";
    string CROSS_BOT = "┴";
    string CROSS_INNER = "┼";
    string CROSS_LEFT = "├";
    string CROSS_RIGHT = "┤";
    string VEDGE = "│";

    string create_table(const vector<int> &sizes, const vector<vector<string>> &content) const;

    string create_table(const vector<vector<string>> &content) const {
        vector<int> sizes(content.at(0).size());

        for (int i = 0; i < content.size(); ++i) {
            for (int j = 0; j < content.at(i).size(); ++j) {
                sizes[j] = max(sizes.at(j), 2+(int) content.at(i).at(j).length());
            }
        }
        return create_table(sizes, content);
    };

};

extern TermBox TB;

/*
┌─────────────────────────┬───────────────┬───────────────────────────┐
│Interface                │ Attribute     │ Value                     │
├─────────────────────────┼───────────────┼───────────────────────────┤
│getopt(), getopt_long(), │ Thread safety │ MT-Unsafe race:getopt env │
│getopt_long_only()       │               │                           │
└─────────────────────────┴───────────────┴───────────────────────────┘
*/
