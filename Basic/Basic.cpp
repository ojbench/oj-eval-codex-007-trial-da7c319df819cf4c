/*
 * File: Basic.cpp
 * ---------------
 * This file is the starter project for the BASIC interpreter.
 */

#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <set>
#include "exp.hpp"
#include "parser.hpp"
#include "program.hpp"
#include "Utils/error.hpp"
#include "Utils/tokenScanner.hpp"
#include "Utils/strlib.hpp"

void processLine(std::string line, Program &program, EvalState &state);
static void runProgram(Program &program, EvalState &state);
static Statement *parseStatement(TokenScanner &scanner);
static bool isValidVar(const std::string &token);

int main() {
    EvalState state;
    Program program;
    while (true) {
        try {
            std::string input;
            if (!std::getline(std::cin, input)) break;
            if (input.empty()) continue;
            processLine(input, program, state);
        } catch (ErrorException &ex) {
            std::cout << ex.getMessage() << std::endl;
        } catch (...) {
            break;
        }
    }
    return 0;
}

static bool isIntegerToken(const std::string &token) {
    if (token.empty()) return false;
    size_t index = 0;
    if (token[0] == '+' || token[0] == '-') index = 1;
    if (index == token.size()) return false;
    for (; index < token.size(); ++index) if (!isdigit(token[index])) return false;
    return true;
}

static void listProgram(Program &program) {
    for (int line = program.getFirstLineNumber(); line != -1; line = program.getNextLineNumber(line)) {
        std::cout << program.getSourceLine(line) << std::endl;
    }
}

static void runProgram(Program &program, EvalState &state) {
    int line = program.getFirstLineNumber();
    while (line != -1) {
        Statement *statement = program.getParsedStatement(line);
        if (!statement) error("SYNTAX ERROR");
        int nextLine = program.getNextLineNumber(line);
        try {
            statement->execute(state, program);
            line = nextLine;
        } catch (EndProgramException &) {
            return;
        } catch (int targetLine) {
            if (program.getSourceLine(targetLine).empty()) {
                std::cout << "LINE NUMBER ERROR" << std::endl;
                return;
            }
            line = targetLine;
        }
    }
}

static bool isKeyword(const std::string &token) {
    static const std::set<std::string> keywords = {"REM", "LET", "PRINT", "INPUT", "END", "GOTO", "IF", "THEN", "RUN", "LIST", "CLEAR", "QUIT", "HELP"};
    return keywords.find(token) != keywords.end();
}

static Statement *parseStatement(TokenScanner &scanner) {
    std::string token = scanner.nextToken();
    if (token == "REM") return new RemStatement("");
    if (token == "LET") {
        std::string var = scanner.nextToken();
        if (!isValidVar(var)) error("SYNTAX ERROR");
        if (scanner.nextToken() != "=") error("SYNTAX ERROR");
        Expression *exp = parseExp(scanner);
        return new LetStatement(var, exp);
    }
    if (token == "PRINT") return new PrintStatement(parseExp(scanner));
    if (token == "INPUT") {
        std::string var = scanner.nextToken();
        if (!isValidVar(var)) error("SYNTAX ERROR");
        if (scanner.hasMoreTokens()) error("SYNTAX ERROR");
        return new InputStatement(var);
    }
    if (token == "END") {
        if (scanner.hasMoreTokens()) error("SYNTAX ERROR");
        return new EndStatement();
    }
    if (token == "REM") return new RemStatement("");
    if (token == "GOTO") {
        std::string lineToken = scanner.nextToken();
        if (!isIntegerToken(lineToken) || scanner.hasMoreTokens()) error("SYNTAX ERROR");
        return new GotoStatement(stringToInteger(lineToken));
    }
    if (token == "IF") {
        Expression *left = readE(scanner, 1);
        std::string op = scanner.nextToken();
        if (op != "=" && op != "<" && op != ">") error("SYNTAX ERROR");
        Expression *right = readE(scanner, 1);
        if (scanner.nextToken() != "THEN") error("SYNTAX ERROR");
        std::string lineToken = scanner.nextToken();
        if (!isIntegerToken(lineToken) || scanner.hasMoreTokens()) error("SYNTAX ERROR");
        int target = stringToInteger(lineToken);
        return new IfStatement(left, op, right, target);
    }
    error("SYNTAX ERROR");
    return NULL;
}

static bool isValidVar(const std::string &token) {
    if (token.empty()) return false;
    if (isKeyword(token)) return false;
    for (char ch : token) if (!isalnum(static_cast<unsigned char>(ch))) return false;
    return true;
}

void processLine(std::string line, Program &program, EvalState &state) {
    TokenScanner scanner;
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    scanner.setInput(line);

    std::string first = scanner.nextToken();
    if (first.empty()) return;

    if (isIntegerToken(first)) {
        int lineNumber = stringToInteger(first);
        if (!scanner.hasMoreTokens()) {
            program.removeSourceLine(lineNumber);
            return;
        }
        program.addSourceLine(lineNumber, line);
        Statement *statement = parseStatement(scanner);
        program.setParsedStatement(lineNumber, statement);
        return;
    }

    if (first == "RUN") {
        if (scanner.hasMoreTokens()) error("SYNTAX ERROR");
        runProgram(program, state);
        return;
    }
    if (first == "LIST") {
        if (scanner.hasMoreTokens()) error("SYNTAX ERROR");
        listProgram(program);
        return;
    }
    if (first == "CLEAR") {
        if (scanner.hasMoreTokens()) error("SYNTAX ERROR");
        program.clear();
        state.Clear();
        return;
    }
    if (first == "QUIT") {
        if (scanner.hasMoreTokens()) error("SYNTAX ERROR");
        std::exit(0);
    }

    scanner.saveToken(first);
    Statement *statement = parseStatement(scanner);
    try {
        statement->execute(state, program);
    } catch (EndProgramException &) {
    } catch (int targetLine) {
        if (program.getSourceLine(targetLine).empty()) std::cout << "LINE NUMBER ERROR" << std::endl;
    }
    delete statement;
}
