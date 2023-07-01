//
// Created by gao on 6/26/23.
//

#include "IRSen.h"



std::string IRCtrl::ConstSen::toString()
{
    return {};
}

std::string IRCtrl::VarSen::toString()
{
    return {};
}


string IRCtrl::GlobalValDeclSen::toString()
{
    stringstream ss;
    ss << "@" << this->val->name << " = ";
    if (this->val->isConst)
        ss << "constant";
    else
        ss << "global";
    // constant float 0x4041475CE0000000
    switch (this->val->type) {
    case IRValType::Func: break;   // Unreachable
    case IRValType::Int: ss << " i32 " << this->val->toString(); break;
    case IRValType::Float: ss << " float " << this->val->toString(); break;
    case IRValType::Arr: {
        auto x = std::dynamic_pointer_cast<CArr>(this->val);
        if (x->containedType == IRValType::Float) {
            ss << " float " << this->val->toString();
        } else {
            ss << " i32 " << this->val->toString();
        }
    } break;
    case IRValType::Void: break;
    case IRValType::Unknown: break;
    case IRValType::Pointer: break;
    case IRValType::FloatArr: break;
    case IRValType::IntArr: break;
    }
    return ss.str();
}
