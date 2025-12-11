#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
using namespace std;

struct Token {
    string type;
    string value;
};

void parse(const vector<Token>& tokens);

#endif