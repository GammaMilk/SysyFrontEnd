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
    FunctionSen sen;
    _as("");
}

void IRCtrl::IRBuilder::addGlobalVarDeclStmt(ssize_t t, const std::string &name)
{
    std::string ts;
    if (t == SysyParser::Int)
    {
        ts = "i32";
    } else if (t == SysyParser::Float)
    {
        ts = "float";
    }
    _as("@" + name + " = global " + ts + " 0");
}

void IRCtrl::IRBuilder::build(std::ostream &os)
{
    for (auto &one: _stmts)
    {
        os << one;
        os << '\n';
    }
}

std::string IRCtrl::IRBuilder::build()
{
    std::stringstream ss;
    for (auto &one: _stmts)
    {
        ss << one;
        ss << '\n';
    }
    return ss.str();
}

IRCtrl::IRBuilder::IRBuilder(const std::string &filename)
{
    _filename = filename;
    _as("source_filename = \"" + _filename + "\"");
}

void IRCtrl::IRBuilder::_as(const std::string &s)
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

void IRCtrl::IRBuilder::addConstVarDeclStmt(ssize_t t, const std::string &name)
{

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

bool IRCtrl::IRBuilder::isInGlobal()
{
    return _lc.isInGlobal();
}
