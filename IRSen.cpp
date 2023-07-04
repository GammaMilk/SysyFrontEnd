//
// Created by gao on 6/26/23.
//

#include "IRSen.h"


namespace IRCtrl
{

string ConstSen::toString()
{
    return {};
}


string GlobalValDeclSen::toString()
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
        if (x) {
            if (x->containedType == IRValType::Float) {
                ss << " " << this->val->toString();
            } else {
                ss << " " << this->val->toString();
            }
        } else {
            auto y = std::dynamic_pointer_cast<VArr>(this->val);
            if (y->containedType == IRValType::Float) {
                ss << " " << this->val->toString();
            } else {
                ss << " " << this->val->toString();
            }
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
IROp IRSen::getOp() const
{
    return _op;
}
string LocalSen::toString()
{
    return std::string();
}
string AllocaSen::toString()
{
    // 之所以这里没有使用%百分号，是因为我需要统一名称。名称在之前生成对象时保证%或@
    return m_lv + " = alloca " + irType->toString();
}

}   // namespace IRCtrl
