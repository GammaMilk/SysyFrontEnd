//
// Created by gao on 6/26/23.
//

#ifndef SYSYLEX_IRSEN_H
#define SYSYLEX_IRSEN_H

#include <string>
#include <utility>
#include "IRTypes.h"
#include "IRVal.h"

namespace IRCtrl
{
enum class IROp {
    // INT
    ADD = 100,
    SUB,
    MUL,
    SDIV,
    SREM,
    UDIV,
    UREM,
    // FLOAT
    FADD,
    FSUB,
    FMUL,
    FDIV,
    FNEG,
    // LOGICAL
    SHL,
    LSHR,
    ASHR,
    AND,
    OR,
    XOR,
    // MEM
    ALLOCA,
    LOAD,
    STORE,
    GETELEMENTPTR,
    // CONTROL
    RET,
    BR,
    CALL,
    // CONVERT INT <-> FLOAT AND EXPAND
    ZEXT,
    SITOFP,
    FPTOSI,
    BITCAST,
    // COMPARE
    ICMP,
    FCMP,

    // MISC
    PHI = 200,

    // DONTMIND
    UNKNOWN = 114514
};

class IRSen
{
public:
    IRSen() = default;
    explicit IRSen(string name)
        : name(std::move(name))
    {
    }
    std::string         name;
    virtual std::string toString() = 0;

protected:
    IROp _op = IROp::UNKNOWN;

public:
    [[nodiscard]] IROp getOp() const;
};

class ConstSen : public IRSen
{
public:
    virtual std::string toString() override;
};

/// Global Val: Including i32/float global single/array. Excluding func def.
class GlobalValDeclSen : public IRSen
{
public:
    shared_ptr<NumberVal> val;
    GlobalValDeclSen(const std::string& name, const shared_ptr<NumberVal>& val)
        : IRSen(name)
        , val(val)
    {
    }
    string toString() override;
};

class LocalSen : public IRSen
{

public:
    string toString() override;
};

class AllocaSen : public LocalSen
{
    AllocaSen(const string& lv, IRType& irType1)
        : irType(irType1)
    {
        _op = IROp::ALLOCA;
    }

protected:
    IRType& irType;

public:
    string toString() override;
};

}   // namespace IRCtrl


#endif   // SYSYLEX_IRSEN_H
