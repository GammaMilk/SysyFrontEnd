//
// Created by gaome on 2023/7/10.
//

#include "SysyVisitor.h"
#include "IRBuilder.h"
#include "IRLogger.h"
#include "IRUtils.h"
#include "IRGlobalSwitch.h"
#include "IRGlobal.h"

using namespace IRCtrl;
using IRCtrl::g_builder;
using IRCtrl::g_lc;
using IRCtrl::g_sw;

/// cond: lOrExp;
/// Here we must convert ALL types to bool(including CVal, Var, etc.)
/// \param context
/// \return
std::any IRVisitor::visitCond(SysyParser::CondContext* context)
{
    // check if the type is bool ( important )
    auto lOrExp = context->lOrExp()->accept(this);
    // 1. isBool. do nothing here. In "if", we will get last sentence.
    if (g_sw->isBool) {
        g_sw->isBool=false;
        return 0;
    }
    // 2. isCVal. just judge it NEQ 0?
    // 3. isVar. just judge it NEQ 0?
    auto [lt,ls] = getLastValue(lOrExp);
    if(lt==IRCtrl::IRValType::Float) {
        auto fcmp = MU<FcmpSen>(g_builder->getNewLabel(),FCMPOp::UNE, ls, "0x0000000000000000");
        g_builder->addSen(std::move(fcmp));
    } else if(lt==IRCtrl::IRValType::Int) {
        auto icmp = MU<IcmpSen>(g_builder->getNewLabel(),ICMPOp::NE, ls, "0");
        g_builder->addSen(std::move(icmp));
    } else {
        RUNTIME_ERROR("Unknown type in cond");
    }
    return 0;
}

std::any IRVisitor::visitGeq(SysyParser::GeqContext* context)
{
    return 0;
}

/// relExp:
//	| relExp Lt addExp	# lt
/// \param context
/// \return
std::any IRVisitor::visitLt(SysyParser::LtContext* context)
{
    // 3<b<c is not allowed. so let's assume that there is only one < in the expression
    // that means, relExp is always a single addExp
    auto relExp = context->relExp()->accept(this);
    auto [lt, ls]= getLastValue(relExp);
    auto addExp = context->addExp()->accept(this);
    auto [rt, rs] = getLastValue(addExp);
    if(lt!=rt) {
        g_builder->checkTypeAndCast(rt,lt,rs);
        rs=g_builder->getLastLabel();
    }
    if(lt==IRCtrl::IRValType::Float) {
        auto fcmp = MU<FcmpSen>(g_builder->getNewLabel(),FCMPOp::ULT, ls, rs);
        g_builder->addSen(std::move(fcmp));
    } else if(lt==IRCtrl::IRValType::Int) {
        auto icmp = MU<IcmpSen>(g_builder->getNewLabel(),ICMPOp::SLT, ls, rs);
        g_builder->addSen(std::move(icmp));
    } else {
        RUNTIME_ERROR("Unknown type in lt");
    }

    g_sw->isBool= true;
    return 0;
}

/// relExp:
//	addExp				# relExp_
/// \param context
/// \return
std::any IRVisitor::visitRelExp_(SysyParser::RelExp_Context* context)
{
    // FINAL
    return context->addExp()->accept(this);
}

std::any IRVisitor::visitLeq(SysyParser::LeqContext* context)
{
    return 0;
}

std::any IRVisitor::visitGt(SysyParser::GtContext* context)
{
    return 0;
}

std::any IRVisitor::visitNeq(SysyParser::NeqContext* context)
{
    return 0;
}

/// eqExp:
//	| eqExp Eq relExp	# eq
/// \param context
/// \return
std::any IRVisitor::visitEq(SysyParser::EqContext* context)
{
    return 0;
}

/// eqExp:
//	relExp				# eqExp_
/// \param context
/// \return
std::any IRVisitor::visitEqExp_(SysyParser::EqExp_Context* context)
{
    // FINAL
    return context->relExp()->accept(this);
}

/// lAndExp: eqExp # lAndExp_
/// \param context
/// \return
std::any IRVisitor::visitLAndExp_(SysyParser::LAndExp_Context* context)
{
    // FINAL
    return context->eqExp()->accept(this);
}

/// addExp:
//	| addExp Add mulExp	# add
/// \param context
/// \return
std::any IRVisitor::visitAnd(SysyParser::AndContext* context)
{

    return 0;
}


/// lOrExp:     | lOrExp Or lAndExp # or;
/// \param context
/// \return
std::any IRVisitor::visitOr(SysyParser::OrContext* context)
{
    auto lOrExp  = context->lOrExp()->accept(this);
    auto lAndExp = context->lAndExp()->accept(this);
    return 0;
}

/// lOrExp: lAndExp # lOrExp_
/// \param context
/// \return
std::any IRVisitor::visitLOrExp_(SysyParser::LOrExp_Context* context)
{
    // FINAL
    return context->lAndExp()->accept(this);
}
