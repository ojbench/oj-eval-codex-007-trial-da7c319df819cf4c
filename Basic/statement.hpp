/*
 * File: statement.h
 * -----------------
 * This file defines the Statement abstract type.  In
 * the finished version, this file will also specify subclasses
 * for each of the statement types.  As you design your own
 * version of this class, you should pay careful attention to
 * the exp.h interface, which is an excellent model for
 * the Statement class hierarchy.
 */

#ifndef _statement_h
#define _statement_h

#include <string>
#include <sstream>
#include <iostream>
#include "evalstate.hpp"
#include "exp.hpp"
#include "Utils/tokenScanner.hpp"
#include "program.hpp"
#include "parser.hpp"
#include "Utils/error.hpp"
#include "Utils/strlib.hpp"

class Program;

class Statement {
public:
    Statement();
    virtual ~Statement();
    virtual void execute(EvalState &state, Program &program) = 0;
};

struct EndProgramException {};

class RemStatement : public Statement {
public:
    explicit RemStatement(std::string line);
    void execute(EvalState &state, Program &program) override;
private:
    std::string line;
};

class LetStatement : public Statement {
public:
    LetStatement(std::string var, Expression *exp);
    ~LetStatement() override;
    void execute(EvalState &state, Program &program) override;
private:
    std::string var;
    Expression *exp;
};

class PrintStatement : public Statement {
public:
    explicit PrintStatement(Expression *exp);
    ~PrintStatement() override;
    void execute(EvalState &state, Program &program) override;
private:
    Expression *exp;
};

class InputStatement : public Statement {
public:
    explicit InputStatement(std::string var);
    void execute(EvalState &state, Program &program) override;
private:
    std::string var;
};

class EndStatement : public Statement {
public:
    EndStatement();
    void execute(EvalState &state, Program &program) override;
};

class GotoStatement : public Statement {
public:
    explicit GotoStatement(int lineNumber);
    int getLineNumber() const;
    void execute(EvalState &state, Program &program) override;
private:
    int lineNumber;
};

class IfStatement : public Statement {
public:
    IfStatement(Expression *left, std::string op, Expression *right, int targetLine);
    ~IfStatement() override;
    void execute(EvalState &state, Program &program) override;
private:
    Expression *left;
    std::string op;
    Expression *right;
    int targetLine;
};

#endif
