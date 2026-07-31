/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include "statement.hpp"

int stringToInt(std::string str);

Statement::Statement() = default;
Statement::~Statement() = default;

RemStatement::RemStatement(std::string line) : line(std::move(line)) {}
void RemStatement::execute(EvalState &, Program &) {}

LetStatement::LetStatement(std::string var, Expression *exp) : var(std::move(var)), exp(exp) {}
LetStatement::~LetStatement() { delete exp; }
void LetStatement::execute(EvalState &state, Program &) {
    if (!exp) error("SYNTAX ERROR");
    state.setValue(var, exp->eval(state));
}

PrintStatement::PrintStatement(Expression *exp) : exp(exp) {}
PrintStatement::~PrintStatement() { delete exp; }
void PrintStatement::execute(EvalState &state, Program &) {
    std::cout << exp->eval(state) << std::endl;
}

InputStatement::InputStatement(std::string var) : var(std::move(var)) {}
void InputStatement::execute(EvalState &state, Program &) {
    while (true) {
        std::cout << " ? ";
        std::string line;
        std::getline(std::cin, line);
        std::istringstream stream(line);
        int value;
        char extra;
        if ((stream >> value) && !(stream >> extra)) {
            state.setValue(var, value);
            return;
        }
        std::cout << "INVALID NUMBER" << std::endl;
    }
}

EndStatement::EndStatement() = default;
void EndStatement::execute(EvalState &, Program &) { throw EndProgramException(); }

GotoStatement::GotoStatement(int lineNumber) : lineNumber(lineNumber) {}
int GotoStatement::getLineNumber() const { return lineNumber; }
void GotoStatement::execute(EvalState &, Program &) { throw lineNumber; }

IfStatement::IfStatement(Expression *left, std::string op, Expression *right, int targetLine)
    : left(left), op(std::move(op)), right(right), targetLine(targetLine) {}
IfStatement::~IfStatement() { delete left; delete right; }
void IfStatement::execute(EvalState &state, Program &) {
    int lhs = left->eval(state);
    int rhs = right->eval(state);
    bool condition = false;
    if (op == "=") condition = lhs == rhs;
    else if (op == "<") condition = lhs < rhs;
    else if (op == ">") condition = lhs > rhs;
    else error("SYNTAX ERROR");
    if (condition) throw targetLine;
}
