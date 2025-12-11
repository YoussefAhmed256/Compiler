#include <bits/stdc++.h>
#include "parser.h"
using namespace std;

static vector<Token> tokens;
static int posTok = 0;

Token peek(int offset = 0);
Token getTok();
[[noreturn]] void error(const string& msg);
bool matchKeyword(const string& kw);
bool matchTypeKeyword();
bool matchOp(const string& op);
bool matchChar(const string& ch);
bool matchID();
bool matchNumber();
bool isRelOp(const Token& t);

void Program();
void FuncDecl();
void Block();
void BlockItems();
void BlockItem();
void Decl();
void DeclList();
void Stmt();
void IfStmt();
void ReturnStmt();
void ExprStmt();
void Expr();
void AssignExpr();
void AddExpr();
void AddExprPrime();
void MulExpr();
void MulExprPrime();
void Primary();
void CondExpr();

Token peek(int offset) {
    int idx = posTok + offset;
    if (idx >= 0 && idx < (int)tokens.size()) return tokens[idx];
    return {"EOF", ""};
}

Token getTok() {
    if (posTok < (int)tokens.size()) return tokens[posTok++];
    return {"EOF", ""};
}

[[noreturn]] void error(const string& msg) {
    Token t;
    if (!tokens.empty())
        t = tokens.back();
    else
        t = {"EOF", ""};

    cerr << "Syntax Error at end of input: " << msg << "\n";
    exit(1);
}

bool matchKeyword(const string& kw) {
    if (peek().type == "Keyword" && peek().value == kw) {
        getTok();
        return true;
    }
    return false;
}

bool matchTypeKeyword() {
    if (peek().type == "Keyword") {
        string v = peek().value;
        if (v == "int" || v == "float" || v == "double" || v == "void") {
            getTok();
            return true;
        }
    }
    return false;
}

bool matchOp(const string& op) {
    if (peek().type == "Operator" && peek().value == op) {
        getTok();
        return true;
    }
    return false;
}

bool matchChar(const string& ch) {
    if (peek().type == "Special Char" && peek().value == ch) {
        getTok();
        return true;
    }
    return false;
}

bool matchID() {
    if (peek().type == "Identifier") {
        getTok();
        return true;
    }
    return false;
}

bool matchNumber() {
    if (peek().type == "Number") {
        getTok();
        return true;
    }
    return false;
}

bool isRelOp(const Token& t) {
    if (t.type != "Operator") return false;
    string v = t.value;
    return (v == "==" || v == "!=" || v == "<" || v == ">" || v == "<=" || v == ">=");
}

void Program() {
    FuncDecl();
    if (peek().type != "EOF")
        error("extra tokens after end of program");
}

void FuncDecl() {
    if (!matchTypeKeyword())
        error("expected function return type");
    if (!matchID())
        error("expected function name");
    if (!matchChar("("))
        error("expected '('");
    if (!matchChar(")"))
        error("expected ')'");
    Block();
}

void Block() {
    if (!matchChar("{"))
        error("expected '{'");
    BlockItems();
    if (!matchChar("}"))
        error("expected '}'");
}

void BlockItems() {
    while (true) {
        Token t = peek();
        if (t.type == "Keyword" &&
            (t.value == "int" || t.value == "float" ||
             t.value == "double" || t.value == "void")) {
            BlockItem();
        }
        else if ((t.type == "Keyword" && (t.value == "if" || t.value == "return")) ||
                 t.type == "Identifier" ||
                 (t.type == "Special Char" && t.value == "{")) {
            BlockItem();
        }
        else {
            break;
        }
    }
}

void BlockItem() {
    Token t = peek();
    if (t.type == "Keyword" &&
        (t.value == "int" || t.value == "float" ||
         t.value == "double" || t.value == "void")) {
        Decl();
        if (!matchChar(";"))
            error("missing ';'");
    } else {
        Stmt();
    }
}

void Decl() {
    if (!matchTypeKeyword())
        error("expected type");
    DeclList();
}

void DeclList() {
    if (!matchID())
        error("expected identifier");
    while (matchChar(",")) {
        if (!matchID())
            error("expected identifier");
    }
}

void Stmt() {
    Token t = peek();
    if (t.type == "Keyword" && t.value == "if") {
        IfStmt();
    } else if (t.type == "Keyword" && t.value == "return") {
        ReturnStmt();
        if (!matchChar(";"))
            error("missing ';'");
    } else if (t.type == "Special Char" && t.value == "{") {
        Block();
    } else {
        ExprStmt();
        if (!matchChar(";"))
            error("missing ';'");
    }
}

void IfStmt() {
    if (!matchKeyword("if"))
        error("expected 'if'");
    if (!matchChar("("))
        error("expected '('");
    CondExpr();
    if (!matchChar(")"))
        error("expected ')'");
    Stmt();
    if (peek().type == "Keyword" && peek().value == "else") {
        getTok();
        Stmt();
    }
}

void ReturnStmt() {
    if (!matchKeyword("return"))
        error("expected 'return'");
    Expr();
}

void ExprStmt() {
    Expr();
}

void Expr() {
    AssignExpr();
}

void AssignExpr() {
    if (peek().type == "Identifier" &&
        peek(1).type == "Operator" && peek(1).value == "=") {
        matchID();
        matchOp("=");
        AssignExpr();
    } else {
        AddExpr();
    }
}

void CondExpr() {
    AddExpr();
    Token t = peek();
    if (isRelOp(t)) {
        getTok();
        AddExpr();
    }
}

void AddExpr() {
    MulExpr();
    AddExprPrime();
}

void AddExprPrime() {
    while (peek().type == "Operator" &&
           (peek().value == "+" || peek().value == "-")) {
        getTok();
        MulExpr();
    }
}

void MulExpr() {
    Primary();
    MulExprPrime();
}

void MulExprPrime() {
    while (peek().type == "Operator" &&
           (peek().value == "*" || peek().value == "/")) {
        getTok();
        Primary();
    }
}

void Primary() {
    if (matchID()) {
        return;
    } else if (matchNumber()) {
        return;
    } else if (matchChar("(")) {
        Expr();
        if (!matchChar(")"))
            error("expected ')'");
    } else {
        error("expected identifier, number or '('");
    }
}

void parse(const vector<Token>& inputTokens) {
    tokens.clear();
    posTok = 0;

    for (const auto& t : inputTokens) {
        if (t.type == "Operator" &&
            (t.value == "//" || t.value == "/*" || t.value == "*/"))
            continue;
        if (t.type == "String")
            continue;

        tokens.push_back(t);
    }

    tokens.push_back({"EOF", ""});

    Program();

    cout << "No syntax errors.\n";
}