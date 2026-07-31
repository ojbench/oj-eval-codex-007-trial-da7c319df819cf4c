/*
 * File: parser.cpp
 * ----------------
 * Implements the parser.h interface.
 */

#include "parser.hpp"

Expression *parseExp(TokenScanner &scanner) {
    Expression *exp = readE(scanner);
    if (scanner.hasMoreTokens()) error("parseExp: Found extra token: " + scanner.nextToken());
    return exp;
}

Expression *readE(TokenScanner &scanner, int prec) {
    Expression *exp = readT(scanner);
    std::string token;
    while (true) {
        token = scanner.nextToken();
        int newPrec = precedence(token);
        if (newPrec <= prec) break;
        Expression *rhs = readE(scanner, newPrec);
        exp = new CompoundExp(token, exp, rhs);
    }
    scanner.saveToken(token);
    return exp;
}

Expression *readT(TokenScanner &scanner) {
    std::string token = scanner.nextToken();
    TokenType type = scanner.getTokenType(token);
    if (type == WORD) return new IdentifierExp(token);
    if (type == NUMBER) return new ConstantExp(stringToInteger(token));
    if (token == "-") return new CompoundExp(token, new ConstantExp(0), readE(scanner));
    if (token != "(") error("Illegal term in expression");
    Expression *exp = readE(scanner);
    if (scanner.nextToken() != ")") error("Unbalanced parentheses in expression");
    return exp;
}

int precedence(std::string token) {
    if (token == "=") return 1;
    if (token == "+" || token == "-") return 2;
    if (token == "*" || token == "/") return 3;
    return 0;
}
