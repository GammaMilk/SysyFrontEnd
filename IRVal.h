//
// Created by gao on 6/29/23.
//

#ifndef SYSYLEX_IRVAL_H
#define SYSYLEX_IRVAL_H

#include <string>
#include <utility>
#include <vector>
#include "IRTypes.h"

namespace IRCtrl
{

class IRVal : public IPrintable
{
public:
    std::string name;
    IRValType   type;
    SPType      advancedType;

    IRVal()
        : name(std::string(""))
        , type(IRValType::Unknown)
    {
    }

    IRVal(const std::string& name1, IRValType type1)
        : name(std::string(name1))
        , type(type1)
    {
    }

    explicit IRVal(const std::string& name1)
        : name(std::string(name1))
        , type(IRValType::Unknown)
    {
    }
};

class NumberVal : public IRVal
{
public:
    bool isGlobal;
    bool isConst;

    virtual void unary() {}

    explicit NumberVal(const std::string& name1)
        : isConst(false)
        , isGlobal(false)
        , IRVal(name1)
    {
    }

    NumberVal(bool is_global, bool is_const)
        : isGlobal(is_global)
        , isConst(is_const)
    {
    }
};

class VVal : public NumberVal
{
};

/**
 * Const Value, including const int and const float.
 */
class CVal : public NumberVal
{
public:
    explicit CVal(const std::string& name1)
        : NumberVal(name1)
    {
        isConst = true;
    }

    //    CVal(const std::string& name1, float v)
    //        : CVal(name1)
    //    {
    //        fval = v;
    //    }
    //
    //    CVal(const std::string& name1, int v)
    //        : CVal(name1)
    //    {
    //        ival = v;
    //    }
};

class IntCVal : public CVal
{
public:
    union {
        float fval;
        int   ival{};
    };
    explicit IntCVal(const std::string& name1)
        : CVal(name1)
    {

        type = IRValType::Int;
    }

    IntCVal(const std::string& name1, int val)
        : CVal(name1)
    //        : CVal(name1, val)
    {
        name = name1;
        ival = val;
        type = IRValType::Int;
    }

    void   unary() override;
    string toString() override;
};

class FloatCVal : public CVal
{
public:
    union {
        float fval;
        int   ival{};
    };
    explicit FloatCVal(const std::string& name1)
        : CVal(name1)
    {
        type = IRValType::Float;
        fval = 0;
    }

    FloatCVal(const std::string& name1, float val)
        : CVal(name1)
    {
        type = IRValType::Float;
        fval = val;
    }

    void   unary() override;
    string toString() override;
};

class InitListVal : public IRVal
{
public:
    vector<shared_ptr<InitListVal>> initList;
    vector<shared_ptr<CVal>>        cVal;
    IRValType                       contained = IRValType::Unknown;
    string                          toString() override;
    [[nodiscard]] bool              empty() const;
    [[nodiscard]] size_t            dim() const;
};

class CArr : public CVal
{
public:
    explicit CArr(const string& name1, IRValType contained)
        : CVal(name1)
        , containedType(contained)
    {
        isConst = true;
        type    = IRValType::Arr;
    }
    ~CArr() = default;

    virtual string toString() override;

    std::deque<size_t>       _shape;
    bool                     isZero = false;
    vector<shared_ptr<CArr>> _childArrs;
    vector<shared_ptr<CVal>> _childVals;
    IRValType                containedType = IRValType::Float;

protected:
    virtual string shapeString();
};





}   // namespace IRCtrl
#endif   // SYSYLEX_IRVAL_H
