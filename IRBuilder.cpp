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


void IRCtrl::IRBuilder::build(std::ostream &os)
{
    os << build();
}

std::string IRCtrl::IRBuilder::build()
{
    std::stringstream ss;
    ss << "; Filename = " << this->_filename;
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

void IRCtrl::IRBuilder::setFilename(const std::string &filename)
{
    this->_filename = filename;
}

IRCtrl::VarSen IRCtrl::IRBuilder::addSingleValDeclare(const IRCtrl::IRVal &initVal, bool isConst)
{
    std::stringstream ss;

}
