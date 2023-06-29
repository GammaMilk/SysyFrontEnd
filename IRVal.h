//
// Created by gao on 6/29/23.
//

#ifndef SYSYLEX_IRVAL_H
#define SYSYLEX_IRVAL_H

#include <string>
#include <vector>
#include "IRValType.h"

namespace IRCtrl
{

    class IRVal
    {
    public:
        std::string name;
        IRValType type;

        IRVal() : name(std::string("")), type(IRValType::Unknown) {}

        IRVal(const std::string &name1, IRValType type1) : name(std::string(name1)), type(type1) {}

        explicit IRVal(const std::string &name1) : name(std::string(name1)), type(IRValType::Unknown) {}
    };

    class NumberVal : public IRVal
    {
    public:
        bool isGlobal;
        bool isConst;

        virtual void unary() {}

        explicit NumberVal(const std::string &name1) : isConst(false), isGlobal(false), IRVal(name1) {}

        NumberVal(bool is_global, bool is_const) : isGlobal(is_global), isConst(is_const) {}
    };

    class IntOrFloatCVal : public NumberVal
    {
    public:
        union
        {
            float fval;
            int ival{};
        };

        explicit IntOrFloatCVal(const std::string &name1) : NumberVal(name1) {}

        IntOrFloatCVal(const std::string &name1, float v) : NumberVal(name1), fval(v) {}

        IntOrFloatCVal(const std::string &name1, int v) : NumberVal(name1), ival(v) {}
    };

    class IntVal : public IntOrFloatCVal
    {
    public:
        explicit IntVal(const std::string &name1) : IntOrFloatCVal(name1, 0) {}

        IntVal(const std::string &name1, int val) : IntOrFloatCVal(name1, val) {}

        void unary() override;
    };

    class FloatVal : public IntOrFloatCVal
    {
    public:
        explicit FloatVal(const std::string &name1) : IntOrFloatCVal(name1, 0.0f) {}

        FloatVal(const std::string &name1, float val) : IntOrFloatCVal(name1, val) {}

        void unary() override;
    };

    class IntArrayVal : public NumberVal
    {
    public:
        std::vector<int> val;
        std::vector<int> shape;

        explicit IntArrayVal(const std::string &name1) : NumberVal(name1) {}
    };

    class FloatArrayVal : public NumberVal
    {
    };
}


#endif //SYSYLEX_IRVAL_H
