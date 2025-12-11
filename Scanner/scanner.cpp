#include <bits/stdc++.h>
#include "parser.h"

using namespace std;

bool isKeyword(const string& word) {
    static unordered_set<string> keywords = {
            "int", "float", "if", "else", "while", "for", "return", "void", "double"
    };
    return keywords.count(word);
}

bool isOperatorChar(char c) {
    string ops = "+-*/=<>&|!%";
    return ops.find(c) != string::npos;
}

bool isSpecialChar(char c) {
    string special = "(){},;[]";
    return special.find(c) != string::npos;
}

int main() {
    string filename = "input.txt";
    ifstream fin(filename);
    ofstream fout("tokens.txt");

    if (!fin.is_open()) {
        cerr << "Error: Cannot open file " << filename << endl;
        return 1;
    }

    string code((istreambuf_iterator<char>(fin)), istreambuf_iterator<char>());
    vector<Token> tokens;

    for (size_t i = 0; i < code.size();) {
        if (isspace(code[i])) { i++; continue; }

        if (isdigit(code[i])) {
            string num;
            bool hasDot = false;
            while (i < code.size() && (isdigit(code[i]) || code[i] == '.')) {
                if (code[i] == '.') {
                    if (hasDot) break;
                    hasDot = true;
                }
                num += code[i];
                i++;
            }
            tokens.push_back({"Number", num});
            continue;
        }

        if (isalpha(code[i]) || code[i] == '_') {
            string word;
            while (i < code.size() && (isalnum(code[i]) || code[i] == '_')) {
                word += code[i];
                i++;
            }
            if (isKeyword(word))
                tokens.push_back({"Keyword", word});
            else
                tokens.push_back({"Identifier", word});
            continue;
        }

        if (code[i] == '/' && i + 1 < code.size()) {
            if (code[i + 1] == '/') {
                tokens.push_back({"Operator", "//"});
                i += 2;
                string comment;
                while (i < code.size() && code[i] != '\n') {
                    if (isalpha(code[i]) || code[i] == '-' || code[i] == '_') {
                        string word;
                        while (i < code.size() && (isalpha(code[i]) || code[i] == '-' || code[i] == '_')) {
                            word += code[i];
                            i++;
                        }
                        tokens.push_back({"String", word});
                    } else {
                        i++;
                    }
                }
                continue;
            }
            else if (code[i + 1] == '*') {
                tokens.push_back({"Operator", "/*"});
                i += 2;
                string word;
                while (i + 1 < code.size() && !(code[i] == '*' && code[i + 1] == '/')) {
                    if (isalpha(code[i]) || code[i] == '-' || code[i] == '_') {
                        string temp;
                        while (i < code.size() && (isalpha(code[i]) || code[i] == '-' || code[i] == '_')) {
                            temp += code[i];
                            i++;
                        }
                        tokens.push_back({"String", temp});
                    } else {
                        i++;
                    }
                }
                tokens.push_back({"Operator", "*/"});
                i += 2;
                continue;
            }
        }

        if (isOperatorChar(code[i])) {
            string op(1, code[i]);
            if (i + 1 < code.size() && isOperatorChar(code[i + 1])) {
                string two = op + code[i + 1];
                if (two == "==" || two == "!=" || two == "<=" || two == ">=") {
                    op = two;
                    i++;
                }
            }
            tokens.push_back({"Operator", op});
            i++;
            continue;
        }

        if (isSpecialChar(code[i])) {
            string sym(1, code[i]);
            tokens.push_back({"Special Char", sym});
            i++;
            continue;
        }

        i++;
    }

    for (auto &t : tokens) {
        cout << left << setw(16) << t.type << " : " << t.value << endl;
        fout << left << setw(16) << t.type << " : " << t.value << endl;
    }

    cout << "\nTokens written to tokens.txt successfully.\n";

    parse(tokens);

    fin.close();
    fout.close();
    return 0;
}