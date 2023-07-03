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
    SPType      advancedType;   // Only function and array has advancedType

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

};

class IntCVal : public CVal
{
public:
    int iVal{};
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
        iVal = val;
        type = IRValType::Int;
    }

    void   unary() override;
    string toString() override;
};



class FloatCVal : public CVal
{
public:
    float fVal = .0f;
    explicit FloatCVal(const std::string& name1)
        : CVal(name1)
    {
        type = IRValType::Float;
        fVal = 0;
    }

    FloatCVal(const std::string& name1, float val)
        : CVal(name1)
    {
        type = IRValType::Float;
        fVal = val;
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

/// No need to init "type" in this constructor.
/// This var will filled by its inherits classes
class VVal : public NumberVal
{
public:
    explicit VVal(const string& name1)
        : NumberVal(name1)
    {
        isConst = false;
    }
    bool hasInit = false;
};

class IntVal : public VVal
{
public:
    int iVal{};
    explicit IntVal(const string& name1)
        : VVal(name1)
    {
        type = IRValType::Int;
    }
    IntVal(const string& name1, int val)
        : IntVal(name1)
    {
        iVal = val;
    }

    string toString() override;
};
class BoolVal : public IntVal
{
    int iVal{};
    explicit BoolVal(const string& name1)
        : IntVal(name1)
    {
        type = IRValType::Bool;
    }
    BoolVal(const string& name1, int val)
        : IntVal(name1)
    {
        iVal = val;
    }

    string toString() override;
};
class FloatVal : public VVal
{
public:
    float fVal{};
    explicit FloatVal(const string& name1)
        : VVal(name1)
    {
        type = IRValType::Float;
    }
    FloatVal(const string& name1, float val)
        : FloatVal(name1)
    {
        fVal = val;
    }
    string toString() override;
};

class VArr : public VVal
{
public:
    explicit VArr(const string& name1, IRValType contained)
        : VVal(name1)
        , containedType(contained)
    {
        isConst = false;
        type    = IRValType::Arr;
    }

    string toString() override;

    std::deque<size_t>       _shape;
    bool                     isZero = false;
    vector<shared_ptr<VArr>> _childArrs;
    vector<shared_ptr<CVal>> _childVals;
    IRValType                containedType = IRValType::Float;

protected:
    virtual string shapeString();
};

class LocalVar : public IRVal
{
    int id = -1;
};
using SPLocalVar = shared_ptr<LocalVar>;

class FPVar : public LocalVar
{
public:
    explicit FPVar(const string& name1) { name = name1; }
    string toString() override;
    SPType fpType;
};
using SPFPVar = shared_ptr<FPVar>;


}   // namespace IRCtrl
#endif   // SYSYLEX_IRVAL_H
