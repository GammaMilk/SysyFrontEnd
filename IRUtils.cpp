//
// Created by gao on 6/29/23.
//

#include "IRUtils.h"
#include "IRLogger.h"

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
            _a = af->fVal;
        else
            _a = (float)ai->iVal;
        if (bf != nullptr)
            _b = bf->fVal;
        else
            _b = (float)bi->iVal;
        res->fVal = FLOP(_a, _b, op);
        return res;
    } else {
        int _a, _b;
        _a        = ai->iVal;
        _b        = bi->iVal;
        auto res  = std::make_shared<IntCVal>("");
        res->iVal = T1OP(_a, _b, op);
        return res;
    }
}
/// Build a completely CArr from AST
/// \param iList
/// \return
std::shared_ptr<CArr> Utils::buildAnCArrFromInitList(
    const shared_ptr<InitListVal>& iList, const std::deque<size_t>& shape
)
{
    // iList contains vector<shared_ptr<InitListVal>> initList and vector<shared_ptr<CVal>> cVal;

    // FUCk, Don't use a stack to simulate a recursively process
    auto r    = make_shared<CArr>("", iList->contained);
    r->_shape = shape;
    if (iList->empty()) {
        r->isZero = true;
        return r;
    } else {
        if (!iList->cVal.empty()) {
            r->_childVals = iList->cVal;
            return r;
        } else {
            // the most fuza no bufen
            // first cut shape to shape[1:]
            auto newShape = shape;
            newShape.pop_front();
            LOGD("new SHAPE size=" << newShape.size());
            for (auto& x : iList->initList) {
                auto t = buildAnCArrFromInitList(x, newShape);
                r->_childArrs.emplace_back(std::move(t));
            }
        }
    }
    return r;
}
std::shared_ptr<VArr> Utils::buildAnVArrFromInitList(
    const shared_ptr<InitListVal>& iList, const std::deque<size_t>& shape
)
{
    auto r    = make_shared<VArr>("", iList->contained);
    r->_shape = shape;
    if (iList->empty()) {
        r->isZero = true;
        return r;
    } else {
        if (!iList->cVal.empty()) {
            r->_childVals = iList->cVal;
            return r;
        } else {
            // the most fuza no bufen
            // first cut shape to shape[1:]
            auto newShape = shape;
            newShape.pop_front();
            LOGD("new SHAPE size=" << newShape.size());
            for (auto& x : iList->initList) {
                auto t = buildAnVArrFromInitList(x, newShape);
                r->_childArrs.emplace_back(std::move(t));
            }
        }
    }
    return r;
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