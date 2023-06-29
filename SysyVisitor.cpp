

#include "SysyVisitor.h"
#include "IRBuilder.h"
#include "IRLogger.h"
#include "IRUtils.h"
#include "IRExceptions.h"
#include "IRGlobalSwitch.h"

std::shared_ptr<IRCtrl::IRBuilder> g_builder;
std::shared_ptr<IRCtrl::IRLayerController> g_lc;
std::shared_ptr<IRCtrl::IRGlobalSwitch> g_sw;

using namespace IRCtrl;

/// compUnit: compUnitItem* EOF;
/// \param context
/// \return
std::any Visitor::visitCompUnit(SysyParser::CompUnitContext *context)
{
    LOGD("Enter CompUnit");

    // Init builder and layer controller
    g_builder = std::make_shared<IRCtrl::IRBuilder>(IRCtrl::IRBuilder());
    g_lc = std::make_shared<IRCtrl::IRLayerController>(IRCtrl::IRLayerController());
    g_sw = std::make_shared<IRCtrl::IRGlobalSwitch>(IRCtrl::IRGlobalSwitch());

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
    g_sw->isConst.push();
    g_sw->isConst.set(true);
    for (auto &x: context->constDef())
    {
        x->accept(this);
    }
    g_sw->isConst.pop();
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

/// constDef: Ident (Lbracket exp Rbracket)* Assign initVal;
/// constDecl: Const bType constDef (Comma constDef)* Semicolon;
/// \param context
/// \return
std::any Visitor::visitConstDef(SysyParser::ConstDefContext *context)
{
    LOGD("Enter VisitConstDef");
    auto p = context->parent;
    auto parent = dynamic_cast<SysyParser::ConstDeclContext *>(p);
    if (parent != nullptr)
    {
        auto btype = parent->bType();
        IRCtrl::IRValType type;
        if (dynamic_cast<SysyParser::FloatContext *>(btype))
        {
            type = IRCtrl::IRValType::Float;
            LOGD("Type: Float");
        } else
        {
            type = IRCtrl::IRValType::Int;
            LOGD("Type: Int");
        }
        LOGD("Name: " << context->Ident()->getText());

        auto initVal = context->initVal();

        // check if array
        if (context->exp().empty())
        {
            // single val
            // initVal:
            //	init	 # init
            auto init_ = dynamic_cast<SysyParser::InitContext *>(initVal);

            // Single value(not array) shouldn't have initList rather than init
            if (init_ == nullptr) throw std::runtime_error("Fuck!");

            auto number_any_val = init_->accept(this); // Accept
            auto number_val = std::any_cast<std::shared_ptr<IntOrFloatCVal>>(number_any_val);

            auto int_val = std::dynamic_pointer_cast<IntVal>(number_val);
            auto float_val = std::dynamic_pointer_cast<FloatVal>(number_val);
            // TODO do convert between int and float constants.
            if (type == IRCtrl::IRValType::Int)
            {
                // Int
                int num;
                if (int_val != nullptr) num = int_val->ival;
                else num = (int) float_val->fval;
                LOGD(num);
                // TODO: Judge if in Global
                // TODO: Insert into builder queue
            } else
            {
                // Float
                float num;
                if (float_val != nullptr) num = float_val->fval;
                else num = (float) int_val->ival;
                LOGD(num);
                // TODO: insert
            }

        } else
        {
            // array val
            // TODO
            LOGD("Array");
            LOGD(initVal->getText());
        }

    }
    LOGD("Exit  VisitConstDef");
    return 0;
}

std::any Visitor::visitVarDecl(SysyParser::VarDeclContext *context)
{
    return 0;
}

std::any Visitor::visitVarDef(SysyParser::VarDefContext *context)
{
    throw IRCtrl::NotImplementedException();
}

std::any Visitor::visitInit(SysyParser::InitContext *context)
{
    return context->exp()->accept(this);
}

std::any Visitor::visitInitList(SysyParser::InitListContext *context)
{
    return 0;
}

/// funcDef: funcType Ident Lparen funcFParams? Rparen block;
/// \param context
/// \return
std::any Visitor::visitFuncDef(SysyParser::FuncDefContext *context)
{
    LOGD("Enter FuncDef");
    // TODO
    context->block()->accept(this);
    LOGD("Exit  FuncDef");
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
    LOGD("Enter Block");
    // TODO visitBlock

    LOGD("Exit  Block");
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
    // FINAL.
    return context->addExp()->accept(this);
}

std::any Visitor::visitCond(SysyParser::CondContext *context)
{
    return 0;
}

std::any Visitor::visitLVal(SysyParser::LValContext *context)
{
    return 0;
}

// number
std::any Visitor::visitPrimaryExp_(SysyParser::PrimaryExp_Context *context)
{
    // FINAL
    return context->number()->accept(this);
}

std::any Visitor::visitLValExpr(SysyParser::LValExprContext *context)
{
    return 0;
}

std::any Visitor::visitDecIntConst(SysyParser::DecIntConstContext *context)
{
    // FINAL
    return context->DecIntConst()->getText();
}

std::any Visitor::visitOctIntConst(SysyParser::OctIntConstContext *context)
{
    // FINAL
    return context->OctIntConst()->getText();
}

std::any Visitor::visitHexIntConst(SysyParser::HexIntConstContext *context)
{
    // FINAL
    return context->HexIntConst()->getText();
}

std::any Visitor::visitDecFloatConst(SysyParser::DecFloatConstContext *context)
{
    // FINAL
    return context->DecFloatConst()->getText();
}

std::any Visitor::visitHexFloatConst(SysyParser::HexFloatConstContext *context)
{
    // FINAL
    return context->HexFloatConst()->getText();
}

/// A Const literal number
/// \param context
/// \return std::shared_ptr<IntOrFloatCVal>
std::any Visitor::visitNumber(SysyParser::NumberContext *context)
{
    std::string num;
    if (context->intConst())
    {
        auto x = context->intConst()->accept(this);
        num = std::any_cast<std::string>(x);
        int trueNum;
        try
        {
            trueNum = IRCtrl::Utils::parseInteger(num);
        } catch (const std::invalid_argument &e)
        {
            trueNum = (int) std::strtof(num.c_str(), nullptr);
        }
        std::shared_ptr<IntOrFloatCVal> n = std::make_shared<IntVal>("", trueNum);
        n->isConst = g_sw->isConst.get();
        n->isGlobal = g_builder->isInGlobal();
        return n;
    } else
    {
        auto x = context->floatConst()->accept(this);
        num = std::any_cast<std::string>(x);
        float trueNum;
        trueNum = std::strtof(num.c_str(), nullptr);
        std::shared_ptr<IntOrFloatCVal> n = std::make_shared<FloatVal>("", trueNum);
        n->isConst = g_sw->isConst.get();
        n->isGlobal = g_builder->isInGlobal();
        return n;
    }
}

std::any Visitor::visitUnaryExp_(SysyParser::UnaryExp_Context *context)
{
    return context->primaryExp()->accept(this);
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
    // Const decl, we should pass the value as a true value
    if (g_sw->isConst.get())
    {
        auto n = std::any_cast<std::shared_ptr<IntOrFloatCVal>>(context->unaryExp()->accept(this));
        n->unary();
        return (n);
    }
        // TODO: Impl non-const declare
    else
    {
        // Hope that you can remind yourself what this is...
    }
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

// mulExp:
//	unaryExp				# mulExp_
std::any Visitor::visitMulExp_(SysyParser::MulExp_Context *context)
{
    return context->unaryExp()->accept(this);
}

/// addExp:
//	mulExp				# addExp_
std::any Visitor::visitAddExp_(SysyParser::AddExp_Context *context)
{
    // Actually it is mulExp
    return context->mulExp()->accept(this);
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

/// addExp:
//	| addExp Add mulExp	# add
/// \param context
/// \return
std::any Visitor::visitAnd(SysyParser::AndContext *context)
{
    if (g_sw->isConst.get())
    {
        // return a const number.
    }
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

