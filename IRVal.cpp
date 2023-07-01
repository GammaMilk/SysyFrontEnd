//
// Created by gao on 6/29/23.
//

#include "IRVal.h"

#include <utility>
#include <ios>
#include <sstream>


namespace IRCtrl
{

void IntCVal::unary()
{
    this->ival = -this->ival;
}
string IntCVal::toString()
{
    return std::to_string(this->ival);
}

void FloatCVal::unary()
{
    this->fval = -this->fval;
}
string FloatCVal::toString()
{
    stringstream ss;
    //    ss<<"0x"<<std::hex<<reinterpret_cast<uint32_t>(this->fval);
    // Method above was not allowed
    // In LLVM-IR, a float is 64 bit.
    double thisValue = this->fval;
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

}   // namespace IRCtrl
