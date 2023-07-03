//
// Created by gao on 6/29/23.
//

#ifndef SYSYLEX_IRTYPES_H
#define SYSYLEX_IRTYPES_H
#include <memory>
#include <any>
#include <utility>
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
    Arr,
    Void,
    Pointer,
    FloatArr,   // compatibility
    IntArr,     // compatibility
    Bool,

    Unknown
};

enum class IRValOp { Add, Sub, Mul, Div, Mod };
class IPrintable
{
public:
    virtual string toString() = 0;
};

/// Used for Functions FParams, Defines
class IRType : public IPrintable
{
public:
    IRValType type = IRValType::Unknown;
    explicit IRType(IRValType t)
        : type(t)
    {
    }
    string toString() override { return {}; }
};
using SPType = shared_ptr<IRType>;
using UPType = unique_ptr<IRType>;

class IntType : public IRType
{
public:
    IntType()
        : IRType(IRValType::Int)
    {
    }
    string toString() override;
};
class FloatType : public IRType
{
public:
    FloatType()
        : IRType(IRValType::Float)
    {
    }
    string toString() override;
};
class ArrayType : public IRType
{
public:
    explicit ArrayType(const IRValType& inner, vector<size_t> shape)
        : IRType(IRValType::Arr)
        , innerType(inner)
        , innerShape(std::move(shape))
    {
    }
    IRValType      innerType;
    vector<size_t> innerShape;
    string         toString() override;
};
class FuncType : public IRType
{
public:
    explicit FuncType(IRValType retT, const vector<SPType>& paramsT)
        : IRType(IRValType::Func)
        , retType(retT)
    {
        for (auto& x : paramsT) paramsType.emplace_back(x);
    }
    IRValType      retType;
    vector<SPType> paramsType;
    string         toString() override;
};
class VoidType : public IRType
{
public:
    VoidType()
        : IRType(IRValType::Void)
    {
    }
    string toString() override;
};
class PointerType : public IRType
{
public:
    explicit PointerType(IRType target)
        : IRType(IRValType::Pointer)
        , targetType(target)
    {
    }
    IRType targetType;
};

}   // namespace IRCtrl

#endif   // SYSYLEX_IRTYPES_H
