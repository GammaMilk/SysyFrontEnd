//
// Created by gao on 6/29/23.
//

#include "IRUtils.h"

namespace IRCtrl
{
int IRCtrl::Utils::parseInteger(const std::string& str)
{
    if (str.empty()) { throw std::invalid_argument("Empty string"); }

    size_t startPos = 0;
    int    base     = 10;

    if (str.size() > 2 && (str.substr(0, 2) == "0x" || str.substr(0, 2) == "0X")) {
        startPos = 2;
        base     = 16;
    } else if (str[0] == '0') {
        startPos = 1;
        base     = 8;
    }

    try {
        size_t pos = 0;
        int    num = std::stoi(str.substr(startPos), &pos, base);

        // Check Invalid Chars
        if (pos != str.size() - startPos) {
            throw std::invalid_argument("Invalid characters in the string");
        }

        return num;
    } catch (const std::exception& e) {
        throw std::invalid_argument("Failed to parse the integer");
    }
}

int Utils::tryParseInteger(const std::string& str)
{
    int a = 0;
    try {
        a = parseInteger(str);
    } catch (const std::invalid_argument& e) {
        float f = std::strtof(str.c_str(), nullptr);
        a       = (int)f;
    }
    return a;
}


/// Do a calc between 2 literal/const number
/// \param a
/// \param b
/// \param op
/// \return
std::shared_ptr<CVal>
Utils::constBiCalc(const std::shared_ptr<CVal>& a, const std::shared_ptr<CVal>& b, IRValOp op)
{
    auto af         = std::dynamic_pointer_cast<FloatCVal>(a);
    auto ai         = std::dynamic_pointer_cast<IntCVal>(a);
    auto bf         = std::dynamic_pointer_cast<FloatCVal>(b);
    auto bi         = std::dynamic_pointer_cast<IntCVal>(b);
    bool isResFloat = false;
    isResFloat      = (af != nullptr) || (bf != nullptr);
    if (isResFloat) {
        auto  res = std::make_shared<FloatCVal>("");
        float _a, _b;
        if (af != nullptr)
            _a = af->fval;
        else
            _a = (float)ai->ival;
        if (bf != nullptr)
            _b = bf->fval;
        else
            _b = (float)bi->ival;
        res->fval = FLOP(_a, _b, op);
        return res;
    } else {
        int _a, _b;
        _a        = ai->ival;
        _b        = bi->ival;
        auto res  = std::make_shared<IntCVal>("");
        res->ival = T1OP(_a, _b, op);
        return res;
    }
}
template<class T1, class T2>
T1 Utils::T1OP(T1 v1, T2 v2, IRValOp op)
{
    switch (op) {
    case IRValOp::Add: return (T1)v1 + v2;
    case IRValOp::Sub: return (T1)v1 - v2;
    case IRValOp::Mul: return (T1)v1 * v2;
    case IRValOp::Div: return (T1)v1 / v2;
    case IRValOp::Mod: return (T1)v1 % v2;
    }
}
template<class T1, class T2>
T2 Utils::T2OP(T1 v1, T2 v2, IRValOp op)
{
    switch (op) {
    case IRValOp::Add: return (T2)v1 + v2;
    case IRValOp::Sub: return (T2)v1 - v2;
    case IRValOp::Mul: return (T2)v1 * v2;
    case IRValOp::Div: return (T2)v1 / v2;
    case IRValOp::Mod: return (T2)v1 % v2;
    }
}
template<class T1, class T2>
float Utils::FLOP(T1 v1, T2 v2, IRValOp op)
{
    switch (op) {
    case IRValOp::Add: return (float)v1 + v2;
    case IRValOp::Sub: return (float)v1 - v2;
    case IRValOp::Mul: return (float)v1 * v2;
    case IRValOp::Div: return (float)v1 / v2;
    case IRValOp::Mod: throw std::invalid_argument("float cannot %");
    }
}
}   // namespace IRCtrl