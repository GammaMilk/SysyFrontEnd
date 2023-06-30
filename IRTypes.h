//
// Created by gao on 6/29/23.
//

#ifndef SYSYLEX_IRTYPES_H
#define SYSYLEX_IRTYPES_H
#include <memory>
#include <any>
#include <vector>
#include <stack>
#include <sstream>
using std::any_cast;
using std::make_shared;
using std::make_unique;
using std::shared_ptr;
using std::stack;
using std::string;
using std::stringstream;
using std::unique_ptr;
using std::vector;

namespace IRCtrl
{

enum class IRValType {
    Func,
    Int,
    Float,
    IntArr,
    FloatArr,
    Void,

    Unknown,
};

enum class IRValOp { Add, Sub, Mul, Div, Mod };
class IPrintable
{
public:
    virtual string toString() = 0;
};
}   // namespace IRCtrl

#endif   // SYSYLEX_IRTYPES_H
