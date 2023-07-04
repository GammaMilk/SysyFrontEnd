//
// Created by gao on 6/29/23.
//

#include "IRVal.h"
#include "IRLogger.h"

#include <utility>
#include <ios>
#include <sstream>


namespace IRCtrl
{

void IntCVal::unary()
{
    this->iVal = -this->iVal;
}
string IntCVal::toString()
{
    return std::to_string(this->iVal);
}

void FloatCVal::unary()
{
    this->fVal = -this->fVal;
}
string FloatCVal::toString()
{
    stringstream ss;
    //    ss<<"0x"<<std::hex<<reinterpret_cast<uint32_t>(this->fVal);
    // Method above was not allowed
    // In LLVM-IR, a float is 64 bit.
    double thisValue = this->fVal;
    ss << "0x" << std::hex << *(uint64_t*)(&thisValue);
    return ss.str();
}

string InitListVal::toString()
{
    return {};
}
bool InitListVal::empty() const
{
    return this->cVal.empty() && this->initList.empty();
}
size_t InitListVal::dim() const
{
    if (!cVal.empty()) return 1;
    if (initList.empty()) return 0;
    size_t maxDim = 0;
    for (auto& x : initList) { maxDim = std::max(maxDim, x->dim()); }
    return maxDim + 1;
}
string CArr::toString()
{
    stringstream ss;
    string       elementTypeString = (this->containedType == IRValType::Float) ? "float" : "i32";
    // [3 x [3 x i32]] [[3 x i32] [i32 1, i32 2, i32 3], [3 x i32] zeroinitializer, [3 x i32]
    // zeroinitializer]
    ss << shapeString();
    if (this->isZero) {
        ss << " zeroinitializer";
        return ss.str();
    }
    ss << " [";
    // vector<shared_ptr<CArr>> _childArrs;
    // vector<shared_ptr<CVal>> _childVals;
    if (!this->_childArrs.empty()) {
        for (auto& x : this->_childArrs) { ss << x->toString() << ", "; }
    }
    if (!this->_childVals.empty()) {
        for (auto& x : this->_childVals) {
            ss << elementTypeString << " " << x->toString() << ", ";
        }
    }
    // Here we have a problem: at the end of the string, there is a ", " which is not allowed.
    // So we need to remove it.
    string s = ss.str();
    s.pop_back();
    s.pop_back();
    s += "]";
    LOGD(s);
    return s;
}
string CArr::shapeString()
{
    stringstream ss;
    // returns like [3 x [3 x i32]]
    // a 3 dimensional array likes [5 x [3 x [3 x i32]]]
    // this->_shape = {3,3} (vector<size_t>)
    // element type is i32 or float
    string elementTypeString = (this->containedType == IRValType::Float) ? "float" : "i32";

    for (auto& x : this->_shape) { ss << "[" << x << " x "; }
    ss << elementTypeString;
    for (auto& x : this->_shape) { ss << "]"; }
    return ss.str();
}

string IntVal::toString()
{
    return std::to_string(this->iVal);
}
string FloatVal::toString()
{
    stringstream ss;
    //    ss<<"0x"<<std::hex<<reinterpret_cast<uint32_t>(this->fVal);
    // Method above was not allowed
    // In LLVM-IR, a float is 64 bit.
    double thisValue = this->fVal;
    ss << "0x" << std::hex << *(uint64_t*)(&thisValue);
    return ss.str();
}
string VArr::toString()
{
    stringstream ss;
    string       elementTypeString = (this->containedType == IRValType::Float) ? "float" : "i32";
    // [3 x [3 x i32]] [[3 x i32] [i32 1, i32 2, i32 3], [3 x i32] zeroinitializer, [3 x i32]
    // zeroinitializer]
    ss << shapeString();
    if (this->isZero) {
        ss << " zeroinitializer";
        return ss.str();
    }
    ss << " [";
    // vector<shared_ptr<CArr>> _childArrs;
    // vector<shared_ptr<CVal>> _childVals;
    if (!this->_childArrs.empty()) {
        for (auto& x : this->_childArrs) { ss << x->toString() << ", "; }
    }
    if (!this->_childVals.empty()) {
        for (auto& x : this->_childVals) {
            ss << elementTypeString << " " << x->toString() << ", ";
        }
    }
    // Here we have a problem: at the end of the string, there is a ", " which is not allowed.
    // So we need to remove it.
    string s = ss.str();
    s.pop_back();
    s.pop_back();
    s += "]";

    return s;
}
string VArr::shapeString()
{
    stringstream ss;
    // returns like [3 x [3 x i32]]
    // a 3 dimensional array likes [5 x [3 x [3 x i32]]]
    // this->_shape = {3,3} (vector<size_t>)
    // element type is i32 or float
    string elementTypeString = (this->containedType == IRValType::Float) ? "float" : "i32";

    for (auto& x : this->_shape) { ss << "[" << x << " x "; }
    ss << elementTypeString;
    for (auto& x : this->_shape) { ss << "]"; }
    return ss.str();
}
string FPVar::toString()
{
    stringstream ss;
    auto         pType = this->fpType->type;
    if (pType == IRValType::Float) {
        ss << "float";
    } else if (pType == IRValType::Int) {
        ss << "i32";
    } else if (pType == IRValType::Arr) {
        auto   fpArrType   = std::dynamic_pointer_cast<ArrayType>(this->fpType);
        string elemTypeStr = (fpArrType->innerType == IRValType::Float) ? "float" : "i32";
    }
}
string BoolVal::toString()
{
    return (iVal == 0) ? "0" : "1";
}
string LocalInt::toString()
{
    return std::string();
}
string LocalFloat::toString()
{
    return std::string();
}
}   // namespace IRCtrl
