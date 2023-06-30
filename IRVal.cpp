//
// Created by gao on 6/29/23.
//

#include "IRVal.h"

namespace IRCtrl
{

void IntVal::unary()
{
    this->ival = -this->ival;
}

void FloatVal::unary()
{
    this->fval = -this->fval;
}
}   // namespace IRCtrl
