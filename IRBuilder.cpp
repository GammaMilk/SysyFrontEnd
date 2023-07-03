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
    program = make_shared<IRProgram>();
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

    // Functions
    for (auto& f : this->program->getFuncs()) { ss << f->toString() << "\n"; }

    // TODO Var and IRFunction
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
void IRCtrl::IRBuilder::createFunction(IRCtrl::FuncType& type1, const string& name1)
{
    this->thisFunction = make_shared<IRFunction>(type1, name1);
}
void IRCtrl::IRBuilder::finishFunction()
{
    assert(this->thisFunction != nullptr);
    this->program->addFunction(thisFunction);
    this->thisFunction = nullptr;
}
const shared_ptr<IRCtrl::IRProgram>& IRCtrl::IRBuilder::getProgram() const
{
    return program;
}
