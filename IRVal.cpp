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
void CArr::setShape(vector<int> shape)
{
    _shape = std::move(shape);
}

}   // namespace IRCtrl
