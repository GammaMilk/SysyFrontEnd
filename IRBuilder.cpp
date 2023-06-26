//
// Created by gao on 6/26/23.
//

#include "IRBuilder.h"

#include <utility>
#include <ostream>
#include <sstream>

IRBuilder::IRBuilder()
{
    _as("");
}

void IRBuilder::addVarDeclStmt(ssize_t t, std::string name)
{

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

IRBuilder::IRBuilder(std::string filename)
{
    _filename = std::move(filename);
    _as("source_filename = \"" + _filename + "\"");
}

void IRBuilder::_as(const std::string &s)
{
    _stmts.emplace_back(s);
}
