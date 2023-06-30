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
    union {
        float fval;
        int   ival{};
    };

    explicit CVal(const std::string& name1)
        : NumberVal(name1)
    {
        isConst = true;
    }

    CVal(const std::string& name1, float v)
        : CVal(name1)
    {
        fval = v;
    }

    CVal(const std::string& name1, int v)
        : CVal(name1)
    {
        ival = v;
    }
};

class IntCVal : public CVal
{
public:
    explicit IntCVal(const std::string& name1)
        : CVal(name1, 0)
    {
        type = IRValType::Int;
    }

    IntCVal(const std::string& name1, int val)
        : CVal(name1, val)
    {
        type = IRValType::Int;
    }

    void   unary() override;
    string toString() override;
};

class FloatCVal : public CVal
{
public:
    explicit FloatCVal(const std::string& name1)
        : CVal(name1, 0.0f)
    {
        type = IRValType::Float;
    }

    FloatCVal(const std::string& name1, float val)
        : CVal(name1, val)
    {
        type = IRValType::Float;
    }

    void   unary() override;
    string toString() override;
};

class CArr : public NumberVal
{
public:
    explicit CArr(const string& name1)
        : NumberVal(name1)
    {
        isConst = true;
    }

    void setShape(vector<int> shape);

protected:
    vector<int> _shape;
};

class IntCArr : public CArr
{
public:
    vector<int> initArr;
    IntCArr(const string& name1, vector<int> shape)
        : CArr(name1)
    {
        setShape(std::move(shape));
        int size = 1;
        for (int& x : _shape) { size *= x; }
        initArr.resize(size);
    }
};
class FloatCArr : public CArr
{
public:
    vector<float> initArr;
    FloatCArr(const string& name1, vector<int> shape)
        : CArr(name1)
    {
        setShape(std::move(shape));
        int size = 1;
        for (int& x : _shape) { size *= x; }
        initArr.resize(size);
    }
};

class IntArrayVal : public NumberVal
{
public:
    std::vector<int> val;
    std::vector<int> shape;

    explicit IntArrayVal(const std::string& name1)
        : NumberVal(name1)
    {
        type = IRValType::IntArr;
    }
};

class FloatArrayVal : public NumberVal
{
public:
    std::vector<float> val;
    std::vector<int>   shape;

    explicit FloatArrayVal(const std::string& name1)
        : NumberVal(name1)
    {
        type = IRValType::FloatArr;
    }
};

class Function : public IRVal
{
};




}   // namespace IRCtrl
#endif   // SYSYLEX_IRVAL_H
