//
// Created by gao on 6/26/23.
//

#ifndef SYSYLEX_IRSEN_H
#define SYSYLEX_IRSEN_H

#include <string>

namespace IRCtrl
{
enum class IROP {
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
    PHI = 200
};

class IRSen
{
public:
    IRSen();

    std::string name;

    virtual std::string toString() = 0;

protected:
    IROP _op;
};

class IntAlgoSen : public IRSen
{
public:
    virtual std::string toString() override;
};

class LogiSen : public IRSen
{
public:
    virtual std::string toString() override;
};

class FunctionSen : public IRSen
{
public:
    virtual std::string toString() override;
};

class VarSen : public IRSen
{
public:
    virtual std::string toString() override;
};

class ConstSen : public IRSen
{
public:
    virtual std::string toString() override;
};
}   // namespace IRCtrl

// 能不能在遍历语法树的同时写出三地址代码？能吗？
// 比如在int a = (5*b)+c/2;
// varDecl(INT "a" exp(add mul 5 b dev c 2)
//?好像可以
// 所以应该不需要参考代码的一些自己另外定义的操作？

#endif   // SYSYLEX_IRSEN_H
