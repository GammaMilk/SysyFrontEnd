//
// Created by gao on 6/29/23.
//

#include "IRTypes.h"

namespace IRCtrl
{
string IntType::toString()
{
    return "i32";
}
string FloatType::toString()
{
    return "float";
}
string ArrayType::toString()
{
    stringstream ss;
    // TODO
    ss << "[ARRAY NOT IMPL]";
    return ss.str();
}
string FuncType::toString()
{
    return std::string();
}
string VoidType::toString()
{
    return "void";
}
}   // namespace IRCtrl