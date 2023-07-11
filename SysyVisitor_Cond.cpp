//
// Created by gaome on 2023/7/11.
//
#include "SysyVisitor.h"
#include "IRBuilder.h"
#include "IRLogger.h"
#include "IRUtils.h"
#include "IRExceptions.h"
#include "IRGlobalSwitch.h"
#include "IRCondAndIterController.h"
#include "IRGlobal.h"

using namespace IRCtrl;
using IRCtrl::g_builder;
using IRCtrl::g_lc;
using IRCtrl::g_sw;
using IRCtrl::g_bbc;


/// stmt:
/// 	| If Lparen cond Rparen stmt (Else stmt)?	# ifElse
/// \param context
/// \return
std::any IRVisitor::visitIfElse(SysyParser::IfElseContext* context)
{
    // First we need to create 3 BBs
    SPBB trueBB = g_builder->createBB();
    SPBB falseBB = nullptr;
    SPBB afterBB = g_builder->createBB();

    // check if there is else
    if(context->Else() != nullptr)
    {
        falseBB = g_builder->createBB();
    } else {
        falseBB = afterBB;
    }

    // push to BBC
    g_bbc->pushIf(trueBB, falseBB, afterBB, g_lc->getCurLayerNum());

    // 1. add COND sen
    context->cond()->accept(this);
    g_sw->isBool=false; // reset
    auto [lt, ls] = getLastValue();
    auto lastCond = ls;

    // 2. add BR
    auto condBrSen = MU<BrSen>(lastCond, trueBB->getLabel(), falseBB->getLabel());
    g_builder->addSen(std::move(condBrSen));

    // 3. move to TrueBB
    g_builder->moveCurBBTo(trueBB);

    // 4. accept true stmt
    context->stmt()[0]->accept(this);

    // 5. add BR
    auto trueBrSen = MU<BrSen>(afterBB->getLabel());
    g_builder->addSen(std::move(trueBrSen));

    // 6-8 if there is else
    if (context->Else() != nullptr) {
        // 6. move to FalseBB
        g_builder->moveCurBBTo(falseBB);

        // 7. accept false stmt
        if(context->stmt().size() == 2)
            context->stmt()[1]->accept(this);

        // 8. add BR
        auto falseBrSen = MU<BrSen>(afterBB->getLabel());
        g_builder->addSen(std::move(falseBrSen));
    }

    // 9. move to AfterBB
    g_builder->moveCurBBTo(afterBB);

    return 0;
}


/// stmt:
/// | While Lparen cond Rparen stmt				# while
/// \param context
/// \return
std::any IRVisitor::visitWhile(SysyParser::WhileContext* context)
{
    return 0;
}


/// stmt:
/// \param context
/// \return
std::any IRVisitor::visitBreak(SysyParser::BreakContext* context)
{
    return 0;
}


/// stmt:
/// \param context
/// \return
std::any IRVisitor::visitContinue(SysyParser::ContinueContext* context)
{
    return 0;
}