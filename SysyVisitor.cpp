

#include "SysyVisitor.h"
#include "IRBuilder.h"
#include "IRLogger.h"

std::shared_ptr<IRCtrl::IRBuilder> builder;

/// compUnit: compUnitItem* EOF;
/// \param context
/// \return
std::any Visitor::visitCompUnit(SysyParser::CompUnitContext *context)
{
    LOGD("Enter CompUnit");
    builder = std::make_shared<IRCtrl::IRBuilder>(IRCtrl::IRBuilder());

    for (auto &x: context->compUnitItem())
    {
        x->accept(this);
    }

    LOGD("Exit CompUnit");
    return 0;
}

/// compUnitItem: decl | funcDef;
/// \param context
/// \return
std::any Visitor::visitCompUnitItem(SysyParser::CompUnitItemContext *context)
{
    if (context->decl() != nullptr)
    {
        context->decl()->accept(this);
    } else if (context->funcDef() != nullptr)
    {
        context->funcDef()->accept(this);
    }
    return 0;
}


/// decl: constDecl | varDecl;
/// \param context
/// \return
std::any Visitor::visitDecl(SysyParser::DeclContext *context)
{
    if (context->constDecl() != nullptr)
    {
        context->constDecl()->accept(this);
    } else if (context->varDecl() != nullptr)
    {
        context->varDecl()->accept(this);
    }
    return 0;
}

std::any Visitor::visitConstDecl(SysyParser::ConstDeclContext *context)
{
    return 0;
}

std::any Visitor::visitInt(SysyParser::IntContext *context)
{
    return 0;
}

std::any Visitor::visitFloat(SysyParser::FloatContext *context)
{
    return 0;
}

std::any Visitor::visitConstDef(SysyParser::ConstDefContext *context)
{
    return 0;
}

std::any Visitor::visitVarDecl(SysyParser::VarDeclContext *context)
{
    decltype(SysyParser::Float) bt;
    if (dynamic_cast<SysyParser::FloatContext *>(context->bType()))
    {
        bt = SysyParser::Float;
    } else
    {
        bt = SysyParser::Int;
    }
    if (bt == SysyParser::Float)
    {
        LOGD("Enter VarDecl Float");
    } else
    {
        LOGD("Enter VarDecl Int");
    }
    return 0;
}

std::any Visitor::visitVarDef(SysyParser::VarDefContext *context)
{
    return 0;
}

std::any Visitor::visitInit(SysyParser::InitContext *context)
{
    return 0;
}

std::any Visitor::visitInitList(SysyParser::InitListContext *context)
{
    return 0;
}

std::any Visitor::visitFuncDef(SysyParser::FuncDefContext *context)
{
    return 0;
}

std::any Visitor::visitFuncType_(SysyParser::FuncType_Context *context)
{
    return 0;
}

std::any Visitor::visitVoid(SysyParser::VoidContext *context)
{
    return 0;
}

std::any Visitor::visitFuncFParams(SysyParser::FuncFParamsContext *context)
{
    return 0;
}

std::any Visitor::visitScalarParam(SysyParser::ScalarParamContext *context)
{
    return 0;
}

std::any Visitor::visitArrayParam(SysyParser::ArrayParamContext *context)
{
    return 0;
}

std::any Visitor::visitBlock(SysyParser::BlockContext *context)
{
    return 0;
}

std::any Visitor::visitBlockItem(SysyParser::BlockItemContext *context)
{
    return 0;
}

std::any Visitor::visitAssign(SysyParser::AssignContext *context)
{
    return 0;
}

std::any Visitor::visitExprStmt(SysyParser::ExprStmtContext *context)
{
    return 0;
}

std::any Visitor::visitBlockStmt(SysyParser::BlockStmtContext *context)
{
    return 0;
}

std::any Visitor::visitIfElse(SysyParser::IfElseContext *context)
{
    return 0;
}

std::any Visitor::visitWhile(SysyParser::WhileContext *context)
{
    return 0;
}

std::any Visitor::visitBreak(SysyParser::BreakContext *context)
{
    return 0;
}

std::any Visitor::visitContinue(SysyParser::ContinueContext *context)
{
    return 0;
}

std::any Visitor::visitReturn(SysyParser::ReturnContext *context)
{
    return 0;
}

std::any Visitor::visitExp(SysyParser::ExpContext *context)
{
    return 0;
}

std::any Visitor::visitCond(SysyParser::CondContext *context)
{
    return 0;
}

std::any Visitor::visitLVal(SysyParser::LValContext *context)
{
    return 0;
}

std::any Visitor::visitPrimaryExp_(SysyParser::PrimaryExp_Context *context)
{
    return 0;
}

std::any Visitor::visitLValExpr(SysyParser::LValExprContext *context)
{
    return 0;
}

std::any Visitor::visitDecIntConst(SysyParser::DecIntConstContext *context)
{
    return 0;
}

std::any Visitor::visitOctIntConst(SysyParser::OctIntConstContext *context)
{
    return 0;
}

std::any Visitor::visitHexIntConst(SysyParser::HexIntConstContext *context)
{
    return 0;
}

std::any Visitor::visitDecFloatConst(SysyParser::DecFloatConstContext *context)
{
    return 0;
}

std::any Visitor::visitHexFloatConst(SysyParser::HexFloatConstContext *context)
{
    return 0;
}

std::any Visitor::visitNumber(SysyParser::NumberContext *context)
{
    return 0;
}

std::any Visitor::visitUnaryExp_(SysyParser::UnaryExp_Context *context)
{
    return 0;
}

std::any Visitor::visitCall(SysyParser::CallContext *context)
{
    return 0;
}

std::any Visitor::visitUnaryAdd(SysyParser::UnaryAddContext *context)
{
    return 0;
}

std::any Visitor::visitUnarySub(SysyParser::UnarySubContext *context)
{
    return 0;
}

std::any Visitor::visitNot(SysyParser::NotContext *context)
{
    return 0;
}

std::any Visitor::visitStringConst(SysyParser::StringConstContext *context)
{
    return 0;
}

std::any Visitor::visitFuncRParam(SysyParser::FuncRParamContext *context)
{
    return 0;
}

std::any Visitor::visitFuncRParams(SysyParser::FuncRParamsContext *context)
{
    return 0;
}

std::any Visitor::visitDiv(SysyParser::DivContext *context)
{
    return 0;
}

std::any Visitor::visitMod(SysyParser::ModContext *context)
{
    return 0;
}

std::any Visitor::visitMul(SysyParser::MulContext *context)
{
    return 0;
}

std::any Visitor::visitMulExp_(SysyParser::MulExp_Context *context)
{
    return 0;
}

std::any Visitor::visitAddExp_(SysyParser::AddExp_Context *context)
{
    return 0;
}

std::any Visitor::visitAdd(SysyParser::AddContext *context)
{
    return 0;
}

std::any Visitor::visitSub(SysyParser::SubContext *context)
{
    return 0;
}

std::any Visitor::visitGeq(SysyParser::GeqContext *context)
{
    return 0;
}

std::any Visitor::visitLt(SysyParser::LtContext *context)
{
    return 0;
}

std::any Visitor::visitRelExp_(SysyParser::RelExp_Context *context)
{
    return 0;
}

std::any Visitor::visitLeq(SysyParser::LeqContext *context)
{
    return 0;
}

std::any Visitor::visitGt(SysyParser::GtContext *context)
{
    return 0;
}

std::any Visitor::visitNeq(SysyParser::NeqContext *context)
{
    return 0;
}

std::any Visitor::visitEq(SysyParser::EqContext *context)
{
    return 0;
}

std::any Visitor::visitEqExp_(SysyParser::EqExp_Context *context)
{
    return 0;
}

std::any Visitor::visitLAndExp_(SysyParser::LAndExp_Context *context)
{
    return 0;
}

std::any Visitor::visitAnd(SysyParser::AndContext *context)
{
    return 0;
}

std::any Visitor::visitOr(SysyParser::OrContext *context)
{
    return 0;
}

std::any Visitor::visitLOrExp_(SysyParser::LOrExp_Context *context)
{
    return 0;
}

