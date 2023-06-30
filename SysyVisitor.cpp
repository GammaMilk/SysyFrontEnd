

#include "SysyVisitor.h"
#include "IRBuilder.h"
#include "IRLogger.h"
#include "IRUtils.h"
#include "IRExceptions.h"
#include "IRGlobalSwitch.h"

std::shared_ptr<IRCtrl::IRBuilder>         g_builder;
std::shared_ptr<IRCtrl::IRLayerController> g_lc;
std::shared_ptr<IRCtrl::IRGlobalSwitch>    g_sw;

using namespace IRCtrl;

/// compUnit: compUnitItem* EOF;
/// \param context
/// \return
std::any Visitor::visitCompUnit(SysyParser::CompUnitContext* context)
{
    LOGD("Enter CompUnit");

    // Init builder and layer controller
    g_builder = std::make_shared<IRCtrl::IRBuilder>(IRCtrl::IRBuilder());
    g_lc      = std::make_shared<IRCtrl::IRLayerController>(IRCtrl::IRLayerController());
    g_sw      = std::make_shared<IRCtrl::IRGlobalSwitch>(IRCtrl::IRGlobalSwitch());

    for (auto& x : context->compUnitItem()) { x->accept(this); }

    LOGD("Exit CompUnit");
    return 0;
}

/// compUnitItem: decl | funcDef;
/// \param context
/// \return
std::any Visitor::visitCompUnitItem(SysyParser::CompUnitItemContext* context)
{
    if (context->decl() != nullptr) {
        context->decl()->accept(this);
    } else if (context->funcDef() != nullptr) {
        context->funcDef()->accept(this);
    }
    return 0;
}


/// decl: constDecl | varDecl;
/// \param context
/// \return
std::any Visitor::visitDecl(SysyParser::DeclContext* context)
{
    if (context->constDecl() != nullptr) {
        context->constDecl()->accept(this);
    } else if (context->varDecl() != nullptr) {
        context->varDecl()->accept(this);
    }
    return 0;
}

std::any Visitor::visitConstDecl(SysyParser::ConstDeclContext* context)
{
    // Set this layer to const.
    g_sw->isConst.push();
    g_sw->isConst.set(true);
    for (auto& x : context->constDef()) { x->accept(this); }
    g_sw->isConst.pop();   // resume
    return 0;
}

std::any Visitor::visitInt(SysyParser::IntContext* context)
{
    return 0;
}

std::any Visitor::visitFloat(SysyParser::FloatContext* context)
{
    return 0;
}

/// constDef: Ident (Lbracket exp Rbracket)* Assign initVal;
/// constDecl: Const bType constDef (Comma constDef)* Semicolon;
/// \param context
/// \return
std::any Visitor::visitConstDef(SysyParser::ConstDefContext* context)
{
    LOGD("Enter VisitConstDef");
    auto p      = context->parent;
    auto parent = dynamic_cast<SysyParser::ConstDeclContext*>(p);
    if (parent != nullptr) {
        auto              btype = parent->bType();
        IRCtrl::IRValType type;
        if (dynamic_cast<SysyParser::FloatContext*>(btype)) {
            type = IRCtrl::IRValType::Float;
            LOGD("Type: Float");
        } else {
            type = IRCtrl::IRValType::Int;
            LOGD("Type: Int");
        }
        auto idName = string(context->Ident()->getText());
        LOGD("Name: " << idName);

        auto initVal = context->initVal();

        // check if array
        if (context->exp().empty()) {
            // single val
            // initVal:
            //	init	 # init
            auto init_ = dynamic_cast<SysyParser::InitContext*>(initVal);

            // Single value(not array) shouldn't have initList rather than init
            if (init_ == nullptr) throw std::runtime_error("Fuck!");

            auto number_any_val = init_->accept(this);   // Accept
            auto number_val     = std::any_cast<std::shared_ptr<CVal>>(number_any_val);
            auto int_val        = std::dynamic_pointer_cast<IntCVal>(number_val);
            auto float_val      = std::dynamic_pointer_cast<FloatCVal>(number_val);
            if (type == IRCtrl::IRValType::Int) {
                // Int
                int num = (int_val != nullptr) ? int_val->ival : (int)float_val->fval;
                LOGD(num);
                // Judge if in Global
                if (g_lc->isInGlobal()) {
                    // global const int
                    auto thisIntConstVal = std::make_shared<IntCVal>(idName, num);
                    g_lc->push(thisIntConstVal);
                    g_builder->program->addGlobalConst(thisIntConstVal);
                } else {
                    // TODO local const int
                }
            } else {
                // Float
                float num;
                num = (float_val != nullptr) ? float_val->fval : (float)int_val->ival;
                LOGD(num);
                // const float in global?
                if (g_lc->isInGlobal()) {
                    // const float global
                    auto thisFloatConstVal = make_shared<FloatCVal>(idName, num);
                    g_lc->push(thisFloatConstVal);
                    g_builder->program->addGlobalConst(thisFloatConstVal);
                } else {
                    // TODO const float local
                }
            }

        } else {
            // array val
            // TODO  ARRAY CONST GEN
            LOGD("Array");
            LOGD(initVal->getText());
        }
    }
    LOGD("Exit  VisitConstDef");
    return 0;
}

std::any Visitor::visitVarDecl(SysyParser::VarDeclContext* context)
{
    return 0;
}

std::any Visitor::visitVarDef(SysyParser::VarDefContext* context)
{
    return 0;
}

std::any Visitor::visitInit(SysyParser::InitContext* context)
{
    // FINAL
    return context->exp()->accept(this);
}

std::any Visitor::visitInitList(SysyParser::InitListContext* context)
{
    return 0;
}

/// funcDef: funcType Ident Lparen funcFParams? Rparen block;
/// \param context
/// \return
std::any Visitor::visitFuncDef(SysyParser::FuncDefContext* context)
{
    LOGD("Enter FuncDef");
    // TODO
    context->block()->accept(this);
    LOGD("Exit  FuncDef");
    return 0;
}

std::any Visitor::visitFuncType_(SysyParser::FuncType_Context* context)
{
    return 0;
}

std::any Visitor::visitVoid(SysyParser::VoidContext* context)
{
    return 0;
}

std::any Visitor::visitFuncFParams(SysyParser::FuncFParamsContext* context)
{
    return 0;
}

std::any Visitor::visitScalarParam(SysyParser::ScalarParamContext* context)
{
    return 0;
}

std::any Visitor::visitArrayParam(SysyParser::ArrayParamContext* context)
{
    return 0;
}

std::any Visitor::visitBlock(SysyParser::BlockContext* context)
{
    LOGD("Enter Block");
    g_lc->enter();
    // TODO visitBlock

    LOGD("Exit  Block");
    g_lc->exit();
    return 0;
}

std::any Visitor::visitBlockItem(SysyParser::BlockItemContext* context)
{
    return 0;
}

std::any Visitor::visitAssign(SysyParser::AssignContext* context)
{
    return 0;
}

std::any Visitor::visitExprStmt(SysyParser::ExprStmtContext* context)
{
    return 0;
}

std::any Visitor::visitBlockStmt(SysyParser::BlockStmtContext* context)
{
    return 0;
}

std::any Visitor::visitIfElse(SysyParser::IfElseContext* context)
{
    return 0;
}

std::any Visitor::visitWhile(SysyParser::WhileContext* context)
{
    return 0;
}

std::any Visitor::visitBreak(SysyParser::BreakContext* context)
{
    return 0;
}

std::any Visitor::visitContinue(SysyParser::ContinueContext* context)
{
    return 0;
}

std::any Visitor::visitReturn(SysyParser::ReturnContext* context)
{
    return 0;
}

std::any Visitor::visitExp(SysyParser::ExpContext* context)
{
    // FINAL.
    return context->addExp()->accept(this);
}

std::any Visitor::visitCond(SysyParser::CondContext* context)
{
    return 0;
}

/// lVal: Ident (Lbracket exp Rbracket)*;
/// \param context
/// \return
std::any Visitor::visitLVal(SysyParser::LValContext* context)
{
    if (g_sw->isConst.get()) {
        // Const, we need to actually calc the true value
        string                   name  = context->Ident()->getText();
        const shared_ptr<IRVal>& query = g_lc->query(name, false);
        auto                     cval  = std::dynamic_pointer_cast<CVal>(query);
        return cval;
    } else {
        // Non const, we need
    }
    return 0;
}

/// primaryExp:
//	Lparen exp Rparen	# primaryExp_####
//	| lVal				# lValExpr
//	| number			# primaryExp_####
/// ATTENTION: THIS RULE HAS 2: exp and number!!!!!!!!
/// \param context
/// \return
std::any Visitor::visitPrimaryExp_(SysyParser::PrimaryExp_Context* context)
{
    if (g_lc->isInGlobal()) {
        if (context->exp()) {
            return context->exp()->accept(this);
        } else {
            return context->number()->accept(this);
        }
    }
}
/// primaryExp:
//	Lparen exp Rparen	# primaryExp_
//	| lVal				# lValExpr
/// \param context
/// \return
std::any Visitor::visitLValExpr(SysyParser::LValExprContext* context)
{
    return context->lVal()->accept(this);
}

std::any Visitor::visitDecIntConst(SysyParser::DecIntConstContext* context)
{
    // FINAL
    return context->DecIntConst()->getText();
}

std::any Visitor::visitOctIntConst(SysyParser::OctIntConstContext* context)
{
    // FINAL
    return context->OctIntConst()->getText();
}

std::any Visitor::visitHexIntConst(SysyParser::HexIntConstContext* context)
{
    // FINAL
    return context->HexIntConst()->getText();
}

std::any Visitor::visitDecFloatConst(SysyParser::DecFloatConstContext* context)
{
    // FINAL
    return context->DecFloatConst()->getText();
}

std::any Visitor::visitHexFloatConst(SysyParser::HexFloatConstContext* context)
{
    // FINAL
    return context->HexFloatConst()->getText();
}

/// A Const literal number
/// \param context
/// \return std::shared_ptr<CVal>
std::any Visitor::visitNumber(SysyParser::NumberContext* context)
{
    std::string num;
    if (context->intConst()) {
        auto x                        = context->intConst()->accept(this);
        num                           = std::any_cast<std::string>(x);
        int                   trueNum = Utils::tryParseInteger(num);
        std::shared_ptr<CVal> n       = std::make_shared<IntCVal>("", trueNum);
        n->isConst                    = g_sw->isConst.get();
        n->isGlobal                   = g_lc->isInGlobal();
        return n;
    } else {
        auto x = context->floatConst()->accept(this);
        num    = std::any_cast<std::string>(x);
        float trueNum;
        trueNum                           = std::strtof(num.c_str(), nullptr);
        std::shared_ptr<CVal> n           = std::make_shared<FloatCVal>("", trueNum);
        n->isConst                        = g_sw->isConst.get();
        n->isGlobal                       = g_lc->isInGlobal();
        return n;
    }
}

std::any Visitor::visitUnaryExp_(SysyParser::UnaryExp_Context* context)
{
    // FINAL
    return context->primaryExp()->accept(this);
}

std::any Visitor::visitCall(SysyParser::CallContext* context)
{
    return 0;
}

std::any Visitor::visitUnaryAdd(SysyParser::UnaryAddContext* context)
{
    return 0;
}

std::any Visitor::visitUnarySub(SysyParser::UnarySubContext* context)
{
    // Const decl, we should pass the value as a true value
    if (g_sw->isConst.get()) {
        auto n = std::any_cast<std::shared_ptr<CVal>>(context->unaryExp()->accept(this));
        n->unary();
        return (n);
    }
    // TODO: Impl non-const declare
    else {
        // Hope that you can remind yourself what this is...
    }
    return 0;
}

std::any Visitor::visitNot(SysyParser::NotContext* context)
{
    return 0;
}

std::any Visitor::visitStringConst(SysyParser::StringConstContext* context)
{
    return 0;
}

std::any Visitor::visitFuncRParam(SysyParser::FuncRParamContext* context)
{
    return 0;
}

std::any Visitor::visitFuncRParams(SysyParser::FuncRParamsContext* context)
{
    return 0;
}

/// mulExp Div unaryExp	# div
/// \param context
/// \return
std::any Visitor::visitDiv(SysyParser::DivContext* context)
{
    if (g_sw->isConst.get()) {
        // addExp と mulExp IntORFloatCVal returnする
        auto lav = context->mulExp()->accept(this);
        auto rav = context->unaryExp()->accept(this);
        auto lv  = std::any_cast<std::shared_ptr<CVal>>(lav);
        auto rv  = std::any_cast<std::shared_ptr<CVal>>(rav);

        auto res = Utils::constBiCalc(lv, rv, IRCtrl::IRValOp::Div);
        return res;
    }
    return 0;
}

/// mulExp Mod unaryExp	# mod;
/// \param context
/// \return
std::any Visitor::visitMod(SysyParser::ModContext* context)
{
    if (g_sw->isConst.get()) {
        // addExp と mulExp IntORFloatCVal returnする
        auto lav = context->mulExp()->accept(this);
        auto rav = context->unaryExp()->accept(this);
        auto lv  = std::any_cast<std::shared_ptr<CVal>>(lav);
        auto rv  = std::any_cast<std::shared_ptr<CVal>>(rav);

        auto res = Utils::constBiCalc(lv, rv, IRCtrl::IRValOp::Mod);
        return res;
    }
    return 0;
}

/// mulExp Mul unaryExp	# mul
/// \param context
/// \return
std::any Visitor::visitMul(SysyParser::MulContext* context)
{
    if (g_sw->isConst.get()) {
        // addExp と mulExp IntORFloatCVal returnする
        auto lav = context->mulExp()->accept(this);
        auto rav = context->unaryExp()->accept(this);
        auto lv  = std::any_cast<std::shared_ptr<CVal>>(lav);
        auto rv  = std::any_cast<std::shared_ptr<CVal>>(rav);

        auto res = Utils::constBiCalc(lv, rv, IRCtrl::IRValOp::Mul);
        return res;
    }
    return 0;
}

// mulExp:
//	unaryExp				# mulExp_
std::any Visitor::visitMulExp_(SysyParser::MulExp_Context* context)
{
    // FINAL
    return context->unaryExp()->accept(this);
}

/// addExp:
///	mulExp				# addExp_
/// \param context
/// \return Returns what mulExp returns
std::any Visitor::visitAddExp_(SysyParser::AddExp_Context* context)
{
    // Actually it is mulExp
    // FINAL
    return context->mulExp()->accept(this);
}

/// addExp:
///	| addExp Add mulExp	# add
///  \param context
///  \return
std::any Visitor::visitAdd(SysyParser::AddContext* context)
{
    if (g_sw->isConst.get()) {
        // addExp と mulExp IntORFloatCVal returnする
        auto lav = context->addExp()->accept(this);
        auto rav = context->mulExp()->accept(this);
        auto lv  = std::any_cast<std::shared_ptr<CVal>>(lav);
        auto rv  = std::any_cast<std::shared_ptr<CVal>>(rav);

        auto res = Utils::constBiCalc(lv, rv, IRCtrl::IRValOp::Add);
        return res;
    }
    return 0;
}

/// 	| addExp Sub mulExp	# sub;
/// \param context
/// \return
std::any Visitor::visitSub(SysyParser::SubContext* context)
{
    if (g_sw->isConst.get()) {
        // addExp と mulExp IntORFloatCVal returnする
        auto lav = context->addExp()->accept(this);
        auto rav = context->mulExp()->accept(this);
        auto lv  = std::any_cast<std::shared_ptr<CVal>>(lav);
        auto rv  = std::any_cast<std::shared_ptr<CVal>>(rav);

        auto res = Utils::constBiCalc(lv, rv, IRCtrl::IRValOp::Sub);
        return res;
    }
    return 0;
}

std::any Visitor::visitGeq(SysyParser::GeqContext* context)
{
    return 0;
}

std::any Visitor::visitLt(SysyParser::LtContext* context)
{
    return 0;
}

std::any Visitor::visitRelExp_(SysyParser::RelExp_Context* context)
{
    return 0;
}

std::any Visitor::visitLeq(SysyParser::LeqContext* context)
{
    return 0;
}

std::any Visitor::visitGt(SysyParser::GtContext* context)
{
    return 0;
}

std::any Visitor::visitNeq(SysyParser::NeqContext* context)
{
    return 0;
}

std::any Visitor::visitEq(SysyParser::EqContext* context)
{
    return 0;
}

std::any Visitor::visitEqExp_(SysyParser::EqExp_Context* context)
{
    return 0;
}

std::any Visitor::visitLAndExp_(SysyParser::LAndExp_Context* context)
{
    return 0;
}

/// addExp:
//	| addExp Add mulExp	# add
/// \param context
/// \return
std::any Visitor::visitAnd(SysyParser::AndContext* context)
{
    if (g_sw->isConst.get()) {
        // return a const number.
    }
    return 0;
}

std::any Visitor::visitOr(SysyParser::OrContext* context)
{
    return 0;
}

std::any Visitor::visitLOrExp_(SysyParser::LOrExp_Context* context)
{
    return 0;
}
