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

    shared_ptr<IRType> _retType = makeType(IRValType::Void);
    string             _label;   // -> %3 <- = addi %2, 1
};
using UPLocalSen = unique_ptr<LocalSen>;
using SPLocalSen = shared_ptr<LocalSen>;

class AllocaSen : public LocalSen
{
public:
    AllocaSen(string lv, SPType irType1)
        : irType(std::move(irType1))
        , m_lv(std::move((lv)))
    {
        _op      = IROp::ALLOCA;
        _retType = makePointer(irType);
    }

protected:
    SPType irType;
    string m_lv;

public:
    string toString() override;
};

class ReturnSen : public LocalSen
{
public:
    ReturnSen(string lv, SPType irType1)
        : irType(std::move(irType1))
        , m_lv(std::move((lv)))
    {
        _op = IROp::RET;
    }

protected:
    SPType irType;
    string m_lv;

public:
    string toString() override;
};

class LoadSen : public LocalSen
{
public:
    LoadSen(string retLabel, SPType ty, string sourceName_)
        : sourceName(std::move(sourceName_))
        , irType(std::move(ty))
    {
        _label   = std::move(retLabel);
        _retType = irType;
        _op      = IROp::LOAD;
    }

    string toString() override;

protected:
    SPType irType;
    string sourceName;
};

class StoreSen : public LocalSen
{
public:
    StoreSen(string targetLabel, SPType ty, string sourceName_)
        : sourceName(std::move(sourceName_))
        , irType(std::move(ty))
    {
        _label = std::move(targetLabel);
        _op    = IROp::STORE;
    }

    string toString() override;

protected:
    SPType irType;
    string sourceName;
};

/// %v16 = sitofp i32 5 to float
class SiToFpSen : public LocalSen
{
public:
    SiToFpSen(string retLabel, SPType ty, string sourceName_)
        : sourceName(std::move(sourceName_))
    {
        _label   = std::move(retLabel);
        _retType = std::move(ty);
        _op      = IROp::SITOFP;
    }

    string toString() override;

protected:
    string sourceName;
};

class FpToSiSen : public LocalSen
{
public:
    FpToSiSen(string retLabel, SPType ty, string sourceName_)
        : sourceName(std::move(sourceName_))
    {
        _label   = std::move(retLabel);
        _retType = std::move(ty);
        _op      = IROp::FPTOSI;
    }

    string toString() override;

protected:
    string sourceName;
};

class BiSen : public LocalSen
{
public:
    BiSen(string outLabel, IROp op_, string v1_, SPType ty, string v2_)
        : v1(v1_)
        , v2(v2_)
    {
        _label   = std::move(outLabel);
        _retType = std::move(ty);
        _op      = op_;
    }

    string toString() override;

protected:
    string v1, v2;
};

class GepSen : public LocalSen
{
public:
    GepSen(string outLabel_, SPType t_, string source_, vector<size_t> offset_)
        : t(std::move(t_)),sourceName(std::move(source_)),outLabel(std::move(outLabel_))
        , offset(offset_)
    {
    }
    GepSen(string outLabel_, SPType t_, string source_, vector<size_t> offset_, bool dr)
        : t(std::move(t_)),sourceName(std::move(source_)),outLabel(std::move(outLabel_))
        , offset(offset_), dimensionality_reduction(dr)
    {
    }
    string toString() override;

protected:
    SPType      t;
    vector<size_t> offset;
    string outLabel;
    string sourceName;
    bool dimensionality_reduction = false;
};

// declare void @llvm.memset.p0.i32(ptr, i8, i32, i1)
class Memset: public LocalSen {
public:
    Memset(string retLabel_, string sourceLabel, shared_ptr<IRType> pt_, unsigned char x_, int bytes_):
    pt(std::move(pt_)),sourceName(std::move(sourceLabel)),
        x(x_), bytes(bytes_){_label=std::move(retLabel_);}
protected:
    shared_ptr<IRType> pt;
    unsigned char x;
    int bytes;
    string sourceName;

public:
    string toString() override;
};

// tool functions:
static string opToStr(IROp op_);
}   // namespace IRCtrl


#endif   // SYSYLEX_IRSEN_H
