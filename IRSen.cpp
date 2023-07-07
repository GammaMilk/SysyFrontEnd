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

string ReturnSen::toString()
{
    return "ret " + this->irType->toString() + " " + this->m_lv;
}

string LoadSen::toString()
{
    return _label + " = load " + this->irType->toString() + ", " + makePointer(irType)->toString() +
           " " + this->sourceName;
}

string StoreSen::toString()
{
    stringstream ss;
    ss << "store " << irType->toString() << " " << sourceName << ", ";
    ss << makePointer(irType)->toString() << " " << _label;
    return ss.str();
}

/// %v16 = sitofp i32 5 to float
/// \return
string SiToFpSen::toString()
{
    return _label + " = sitofp " + _retType->toString() + " " + sourceName + " to float";
}
string FpToSiSen::toString()
{
    return _label + " = fptosi " + _retType->toString() + " " + sourceName + " to i32";
}
string BiSen::toString()
{
    return _label + " = " + opToStr(this->getOp()) + " " + _retType->toString() + " " + v1 + ", " +
           v2;
}
string opToStr(IROp op_)
{

    switch (op_) {

    case IROp::ADD: return "add";
    case IROp::SUB: return "sub";
    case IROp::MUL: break;
    case IROp::SDIV: break;
    case IROp::SREM: break;
    case IROp::UDIV: break;
    case IROp::UREM: break;
    case IROp::FADD: return "fadd";
    case IROp::FSUB: return "fsub";
    case IROp::FMUL: break;
    case IROp::FDIV: break;
    case IROp::FNEG: break;
    case IROp::SHL: break;
    case IROp::LSHR: break;
    case IROp::ASHR: break;
    case IROp::AND: break;
    case IROp::OR: break;
    case IROp::XOR: break;
    case IROp::ALLOCA: return "alloca";
    case IROp::LOAD: break;
    case IROp::STORE: break;
    case IROp::GETELEMENTPTR: break;
    case IROp::RET: break;
    case IROp::BR: break;
    case IROp::CALL: break;
    case IROp::ZEXT: break;
    case IROp::SITOFP: break;
    case IROp::FPTOSI: break;
    case IROp::BITCAST: break;
    case IROp::ICMP: break;
    case IROp::FCMP: break;
    case IROp::PHI: break;
    case IROp::UNKNOWN: break;
    }
    return "<<!!! IROP NOT IMPL !!!>>";
}

/// getelementptr <TYPE>, <TYPE>* <POINTER>, i32 <OFFSET>, ...
/// \return
string GepSen::toString()
{
    stringstream ss;

    return ss.str();
}
}   // namespace IRCtrl
