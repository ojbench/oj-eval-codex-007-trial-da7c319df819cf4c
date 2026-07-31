/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include "program.hpp"

Program::Program() = default;
Program::~Program() { clear(); }

void Program::clear() {
    for (auto &entry : lines) delete entry.second.statement;
    lines.clear();
}

void Program::addSourceLine(int lineNumber, const std::string &line) {
    auto &entry = lines[lineNumber];
    delete entry.statement;
    entry.statement = NULL;
    entry.source = line;
}

void Program::removeSourceLine(int lineNumber) {
    auto it = lines.find(lineNumber);
    if (it == lines.end()) return;
    delete it->second.statement;
    lines.erase(it);
}

std::string Program::getSourceLine(int lineNumber) {
    auto it = lines.find(lineNumber);
    if (it == lines.end()) return "";
    return it->second.source;
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
    auto it = lines.find(lineNumber);
    if (it == lines.end()) error("setParsedStatement: No such line number");
    delete it->second.statement;
    it->second.statement = stmt;
}

Statement *Program::getParsedStatement(int lineNumber) {
    auto it = lines.find(lineNumber);
    if (it == lines.end()) return NULL;
    return it->second.statement;
}

int Program::getFirstLineNumber() {
    if (lines.empty()) return -1;
    return lines.begin()->first;
}

int Program::getNextLineNumber(int lineNumber) {
    auto it = lines.upper_bound(lineNumber);
    if (it == lines.end()) return -1;
    return it->first;
}
