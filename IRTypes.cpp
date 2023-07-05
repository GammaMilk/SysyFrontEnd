//
// Created by gao on 6/29/23.
//

#include "IRTypes.h"
#include "IRUtils.h"

namespace IRCtrl
{
string IntType::toString()
{
    return "i32";
}
string FloatType::toString()
{
    return "float";
}
string ArrayType::toString()
{
    stringstream ss;
    // TODO
    size_t numberOfZero = 0;
    size_t bracket      = 0;
    for (auto& x : this->innerShape) {
        if (!x)
            numberOfZero += 1;
        else {
            ss << "[" << x << " x ";
            bracket += 1;
        }
    }
    ss << Utils::valTypeToStr(this->innerType);
    for (size_t i = 0; i < bracket; i++) { ss << "]"; }
    for (size_t i = 0; i < numberOfZero; i++) { ss << "*"; }
    return ss.str();
}
string FuncType::toString()
{
    return std::string();
}
string VoidType::toString()
{
    return "void";
}
SPType makeType(IRValType _t)
{
    if (_t == IRValType::Int) return make_shared<IntType>();
    if (_t == IRValType::Float) return make_shared<FloatType>();
    if (_t == IRValType::Void) return make_shared<VoidType>();
    return nullptr;
}

    SPType makePointer(const SPType &_t, size_t stars) {
        auto pointer = make_shared<PointerType>(_t);
        for (auto i = 1; i < stars; i++) {
            pointer = make_shared<PointerType>(pointer);
        }
        return pointer;
    }

    SPType IRCtrl::makePointer(const SPType &_t) {
        return makePointer(_t, 1);
    }

    string PointerType::toString() {
        return this->targetType->toString() + "*";
    }
}   // namespace IRCtrl