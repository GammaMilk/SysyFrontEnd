//
// Created by gao on 6/26/23.
//

#include "IRBuilder.h"
#include "SysyParser.h"
#include <utility>
#include <ostream>
#include <sstream>

IRBuilder::IRBuilder()
{
    _as("");
}

void IRBuilder::addGlobalVarDeclStmt(ssize_t t, const std::string &name)
{
    std::string ts;
    if (t == SysyParser::Int)
    {
        ts = "i32";
    } else if (t == SysyParser::Float)
    {
        ts = "float";
    }
    _as("@" + name + " = global " + ts + " 0, align 4");
}

void IRBuilder::build(std::ostream &os)
{
    for (auto &one: _stmts)
    {
        os << one;
        os << '\n';
    }
}

std::string IRBuilder::build()
{
    std::stringstream ss;
    for (auto &one: _stmts)
    {
        ss << one;
        ss << '\n';
    }
    return ss.str();
}

IRBuilder::IRBuilder(const std::string &filename)
{
    _filename = filename;
    _as("source_filename = \"" + _filename + "\"");
}

void IRBuilder::_as(const std::string &s)
{
    _stmts.emplace_back(s);
}

bool IRBuilder::isInGlobalScope() const
{
    return _inGlobalScope;
}

void IRBuilder::setInGlobalScope(bool x)
{
    _inGlobalScope = x;
}

void IRBuilder::addConstVarDeclStmt(ssize_t t, const std::string &name)
{

}

int IRBuilder::getNewLabel()
{
    _label += 1;
    return _label;
}

int IRBuilder::getLastLabel()
{
    return _label;
}
