//
// Created by gao on 6/26/23.
//

#include "IRBuilder.h"
#include "SysyParser.h"
#include <utility>
#include <ostream>
#include <sstream>

IRCtrl::IRBuilder::IRBuilder()
{
    program = make_unique<IRProgram>();
}


void IRCtrl::IRBuilder::build(std::ostream& os)
{
    os << build();
}

std::string IRCtrl::IRBuilder::build()
{
    std::stringstream ss;

    // Other Info about file
    ss << "; Filename = " << this->_filename << "\n";
    for (auto& one : _stmts) {
        ss << one;
        ss << '\n';
    }

    // Main Code
    // Global const
    for (auto& c : this->program->getGlobalConst()) {
        GlobalValDeclSen declSen(c->name, c);
        ss << declSen.toString() << "\n";
    }

    // Global Variables
    for (auto& c : this->program->getGlobalVar()) {
        GlobalValDeclSen declSen(c->name, c);
        ss << declSen.toString() << "\n";
    }

    // TODO Var and Function
    return ss.str();
}

IRCtrl::IRBuilder::IRBuilder(const std::string& filename)
    : IRBuilder()
{
    _filename = filename;
    _as("source_filename = \"" + _filename + "\"");
}

void IRCtrl::IRBuilder::_as(const std::string& s)
{
    _stmts.emplace_back(s);
}

bool IRCtrl::IRBuilder::isInGlobalScope() const
{
    return _inGlobalScope;
}

void IRCtrl::IRBuilder::setInGlobalScope(bool x)
{
    _inGlobalScope = x;
}


int IRCtrl::IRBuilder::getNewLabel()
{
    _label += 1;
    return _label;
}

int IRCtrl::IRBuilder::getLastLabel() const
{
    return _label;
}


void IRCtrl::IRBuilder::setFilename(const std::string& filename)
{
    this->_filename = filename;
}
