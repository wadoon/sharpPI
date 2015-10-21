#include "termbox.h"

TermBox TB;

TermBox::TermBox() {

}

std::string TermBox::create_box(const vector<string> &lines,
                                const string &title,
                                size_t width, const string &footer) const {
    stringstream ss;

    ss << EDGE_TOP_LEFT << HEDGE << HEDGE << HEDGE;

    if (title.size() > 0) {
        ss << ' ' << title << ' ';
    } else {
        ss << HEDGE << HEDGE;
    }
    for (int i = 0; i < width - 5 - title.size() - 2; ++i) {
        ss << HEDGE;
    }
    ss << EDGE_TOP_RIGHT << '\n';

    for (const string &l:lines) {
        ss << VEDGE << ' ' << l;
        for (int i = 2 + l.size(); i < width - 2; ++i) {
            ss << ' ';
        }
        ss << ' ' << VEDGE << '\n';
    }

    ss << EDGE_BOT_LEFT;
    for (int i = 0; i < width - 1 - 1 - footer.size() - 1 - 1 - 3; ++i) {
        ss << HEDGE;
    }

    if (footer.size() > 0) {
        ss << ' ' << footer << ' ';
    } else {
        ss << HEDGE << HEDGE;
    }

    ss << HEDGE << HEDGE << HEDGE << EDGE_BOT_RIGHT << '\n';

    return ss.str();
}


void append(stringstream &ss, const string &fill, int size) {
    for (int i = 0; i < size; ++i) {
        ss << fill;
    }
}

void cell(stringstream &ss, const string &fill, int size) {
    ss << fill.substr(0, size);
}

string TermBox::create_table(const vector<int> &sizes, const vector<vector<string>> &content) const {
    vector<string> header;
    return create_table(sizes, header, content);
}

string TermBox::create_table(const vector<int> &sizes, const vector<string> &header,
                             const vector<vector<string>> &content) const {
    stringstream ss;


    ss << EDGE_TOP_LEFT;
    int sum_sizes = 0;
    for (int i = 0; i < sizes.size(); ++i) {
        int size = sizes.at(i);
        sum_sizes += size;

        append(ss, HEDGE, size);
        if (i <= sizes.size() - 2) {
            ss << CROSS_TOP;
        } else {
            ss << EDGE_TOP_RIGHT << "\n";
            break;
        }
    }


    if (header.size() >= 0) {


    }

    for (int row = 0; row < content.size(); ++row) {
        const auto &line = content.at(row);

        ss << VEDGE << ' ';
        for (int i = 0; i < sizes.size(); ++i) {
            const string &c = line.at(i);
            int complete = sizes.at(i);
            int sz_text = min((int) c.length(), complete - 1);
            int sz_filled = complete - sz_text;
            cell(ss, c, sz_text);
            append(ss, " ", sz_filled);
        }
        ss << VEDGE << "\n";

        if(row<content.size()-1) {
            ss << CROSS_LEFT;
            append(ss, HEDGE, sum_sizes + 1);
            ss << CROSS_RIGHT << "\n";
        }
    }

    ss << EDGE_BOT_LEFT;
    for (int i = 0; i < sizes.size(); ++i) {
        int size = sizes.at(i);
        append(ss, HEDGE, size);
        if (i <= sizes.size() - 2) {
            ss << CROSS_BOT;
        } else {
            ss << EDGE_BOT_RIGHT << "\n";
            break;
        }
    }
    return ss.str();
}

