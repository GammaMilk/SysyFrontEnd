//
// Created by gao on 6/26/23.
//

#include "IRSen.h"

IRCtrl::IRSen::IRSen() = default;

std::string IRCtrl::IRSen::toString()
{
    return {};
}

std::string IRCtrl::ConstSen::toString()
{
    return {};
}

std::string IRCtrl::VarSen::toString()
{
    return {};
}

std::string IRCtrl::FunctionSen::toString()
{
    return {};
}

std::string IRCtrl::LogiSen::toString()
{
    return std::string();
}

std::string IRCtrl::IntAlgoSen::toString()
{
    return std::string();
}
