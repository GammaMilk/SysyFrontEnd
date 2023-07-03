

#include "SysyVisitor.h"
#include "IRBuilder.h"
#include "IRLogger.h"
#include "IRUtils.h"
#include "IRExceptions.h"
#include "IRGlobalSwitch.h"
#include "IRBBController.h"

namespace IRCtrl
{
std::shared_ptr<IRCtrl::IRBuilder>         g_builder;
std::shared_ptr<IRCtrl::IRLayerController> g_lc;
std::shared_ptr<IRCtrl::IRGlobalSwitch>    g_sw;
std::shared_ptr<IRCtrl::IRBBController>    g_bbc;
}   // namespace IRCtrl

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
    g_sw->isConst.dive();
    g_sw->isConst.set(true);
    for (auto& x : context->constDef()) { x->accept(this); }
    g_sw->isConst.ascend();   // resume
    return 0;
}

/// bType: Int # int | Float # float;
/// \param context
/// \return
std::any Visitor::visitInt(SysyParser::IntContext* context)
{
    curBType = IRCtrl::IRValType::Int;
    return curBType;
}

/// bType: Int # int | Float # float;
/// \param context
/// \return
std::any Visitor::visitFloat(SysyParser::FloatContext* context)
{
    curBType = IRCtrl::IRValType::Float;
    return curBType;
}

/// constDef: Ident (Lbracket exp Rbracket)* Assign initVal;
/// constDecl: Const bType constDef (Comma constDef)* Semicolon;
/// \param context
/// \return
std::any Visitor::visitConstDef(SysyParser::ConstDefContext* context)
{
    //    LOGD("Enter VisitConstDef");
    auto p      = context->parent;
    auto parent = dynamic_cast<SysyParser::ConstDeclContext*>(p);
    if (parent != nullptr) {
        auto              btype = parent->bType();
        IRCtrl::IRValType type;
        if (dynamic_cast<SysyParser::FloatContext*>(btype)) {
            type = IRCtrl::IRValType::Float;
            //            LOGD("Type: Float");
        } else {
            type = IRCtrl::IRValType::Int;
            //            LOGD("Type: Int");
        }
        auto idName = string(context->Ident()->getText());
        //        LOGD("Name: " << idName);

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
                int num = (int_val != nullptr) ? int_val->iVal : (int)float_val->fVal;
                //                LOGD(num);
                // Judge if in Global
                if (g_lc->isInGlobal()) {
                    // global const int
                    auto thisIntConstVal = std::make_shared<IntCVal>(idName, num);
                    g_lc->push(thisIntConstVal);
                    g_builder->getProgram()->addGlobalConst(thisIntConstVal);
                } else {
                    // TODO local const int
                }
            } else {
                // Float
                float num;
                num = (float_val != nullptr) ? float_val->fVal : (float)int_val->iVal;
                //                LOGD(num);
                // const float in global?
                if (g_lc->isInGlobal()) {
                    // const float global
                    auto thisFloatConstVal = make_shared<FloatCVal>(idName, num);
                    g_lc->push(thisFloatConstVal);
                    g_builder->getProgram()->addGlobalConst(thisFloatConstVal);
                } else {
                    // TODO const float local
                }
            }

        } else {
            // array val list

            // Array 1 : deal with shape
            std::deque<size_t> shape;
            for (auto& e : context->exp()) {
                auto   a           = e->accept(this);
                auto   number_val  = std::any_cast<std::shared_ptr<CVal>>(a);
                auto   int_val     = std::dynamic_pointer_cast<IntCVal>(number_val);
                auto   float_val   = std::dynamic_pointer_cast<FloatCVal>(number_val);
                size_t thisDimSize = (int_val != nullptr) ? int_val->iVal : (size_t)float_val->fVal;
                shape.emplace_back(thisDimSize);
            }

            // Array 2 : deal with init data
            auto iList_a     = context->initVal()->accept(this);
            auto iList       = any_cast<shared_ptr<InitListVal>>(iList_a);
            iList->contained = type;
            if (g_lc->isInGlobal()) {
                shared_ptr<CArr> thisArr;
                thisArr       = Utils::buildAnCArrFromInitList(iList, shape);
                thisArr->name = idName;
                g_lc->push(thisArr);
                g_builder->getProgram()->addGlobalConst(thisArr);
            } else {
                // TODO local CONST val array
            }


            //            LOGD("Array");
            //            LOGD(initVal->getText());
        }
    }
    //    LOGD("Exit  VisitConstDef");
    return 0;
}

/// varDecl: bType varDef (Comma varDef)* Semicolon;
/// \param context
/// \return
std::any Visitor::visitVarDecl(SysyParser::VarDeclContext* context)
{
    // FINAL
    // Although we are dealing with a VAR decl, but it's initVal is const.
    // so here we set isConst to TRUE.

    this->curBType = dynamic_cast<SysyParser::FloatContext*>(context->bType())
                         ? IRCtrl::IRValType::Float
                         : IRCtrl::IRValType::Int;
    for (auto& v : context->varDef()) { v->accept(this); }
    return 0;
}

/// varDef: Ident (Lbracket exp Rbracket)* (Assign initVal)?;
/// \param context
/// \return
std::any Visitor::visitVarDef(SysyParser::VarDefContext* context)
{
    g_enable_log = false;
    LOGD("Enter VisitVarDef");


    IRCtrl::IRValType type   = this->curBType;
    string            idName = context->Ident()->getText();
    if (g_lc->isInGlobal()) {
        // Only in Global, the initList can be initialized as const numbers.
        g_sw->isConst.dive();
        g_sw->isConst.set(true);

        if (context->exp().empty()) {

            // float/int a (= ?)*

            // get the init val if exists
            float fInit = 0;
            int   iInit = 0;
            if (context->initVal() != nullptr) {
                auto initVal  = std::any_cast<shared_ptr<CVal>>(context->initVal()->accept(this));
                auto fValInit = std::dynamic_pointer_cast<FloatCVal>(initVal);
                auto iValInit = std::dynamic_pointer_cast<IntCVal>(initVal);
                if (fValInit != nullptr) {
                    fInit = fValInit->fVal;
                    iInit = (int)fInit;
                } else {
                    iInit = iValInit->iVal;
                    fInit = (float)iInit;
                }
            }

            // make a object and push it into LayerController and Builder
            if (type == IRCtrl::IRValType::Float) {
                auto vVal = make_shared<FloatVal>(idName);
                if (context->initVal() != nullptr) {
                    // Global float has initialized
                    vVal->fVal    = fInit;
                    vVal->hasInit = true;
                }
                g_lc->push(vVal);
                g_builder->getProgram()->addGlobalVar(vVal);
            } else {
                auto vVal = make_shared<IntVal>(idName);
                if (context->initVal() != nullptr) {
                    // Global int has initialized
                    vVal->iVal    = iInit;
                    vVal->hasInit = true;
                }
                g_lc->push(vVal);
                g_builder->getProgram()->addGlobalVar(vVal);
            }
        } else {

            // Array             // float/int a[][][][] (= ?)?

            // Array 1 : deal with shape
            std::deque<size_t> shape;
            for (auto& e : context->exp()) {
                auto   a           = e->accept(this);
                auto   number_val  = std::any_cast<std::shared_ptr<CVal>>(a);
                auto   int_val     = std::dynamic_pointer_cast<IntCVal>(number_val);
                auto   float_val   = std::dynamic_pointer_cast<FloatCVal>(number_val);
                size_t thisDimSize = (int_val != nullptr) ? int_val->iVal : (size_t)float_val->fVal;
                shape.emplace_back(thisDimSize);
            }

            // Array 2 : deal with init data
            shared_ptr<InitListVal> iList;
            if (context->initVal() == nullptr) {
                // has no initVal. just return a non-initialized VArr. just so.
                auto emptyIList = make_shared<InitListVal>();
                iList           = emptyIList;
            } else {
                auto iList_a = context->initVal()->accept(this);
                iList        = any_cast<shared_ptr<InitListVal>>(iList_a);
            }
            iList->contained = this->curBType;
            auto thisArr     = Utils::buildAnVArrFromInitList(iList, shape);
            thisArr->name    = idName;
            g_lc->push(thisArr);
            g_builder->getProgram()->addGlobalVar(thisArr);
        }

        g_sw->isConst.ascend();
    } else {
        // TODO local define like local int/float id([])* (= ?)?
    }

    LOGD("Exit VisitVarDef");
    g_enable_log = true;
    return 0;
}

std::any Visitor::visitInit(SysyParser::InitContext* context)
{
    // FINAL
    return context->exp()->accept(this);
}

/// Lbrace (initVal (Comma initVal)*)? Rbrace	# initList;
/// \param context
/// \return
std::any Visitor::visitInitList(SysyParser::InitListContext* context)
{
    auto ril = make_shared<InitListVal>();
    for (auto& x : context->initVal()) {
        auto a = x->accept(this);
        try {
            auto il = any_cast<shared_ptr<InitListVal>>(a);
            ril->initList.emplace_back(il);
        } catch (const std::bad_any_cast&) {
            auto cv = any_cast<shared_ptr<CVal>>(a);
            ril->cVal.emplace_back(cv);
        }
    }
    return ril;
}

/// funcDef: funcType Ident Lparen funcFParams? Rparen block;
/// \param context
/// \return
std::any Visitor::visitFuncDef(SysyParser::FuncDefContext* context)
{
    g_enable_log = true;
    LOGD("Enter FuncDef");
    context->funcType()->accept(this);   // this operation will change this->curBType

    // Only for log
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch"
    switch (this->curBType) {
    case IRValType::Int: LOGD("Int Func"); break;
    case IRValType::Float: LOGD("Float Func"); break;
    case IRValType::Void: LOGD("Void Func"); break;
    }
#pragma clang diagnostic pop

    // Func name
    string idName = context->Ident()->getText();

    // IRFunction signature
    // IRFunction fps
    vector<SPFPVar> fps;
    if (context->funcFParams()) {
        auto fp = std::any_cast<vector<SPFPVar>>(context->funcFParams()->accept(this));
        fps     = fp;
    }
    vector<SPType> fpTypes;
    fpTypes.reserve(fps.size());
    for (auto& p : fps) { fpTypes.emplace_back(p->fpType); }
    FuncType funcType(this->curBType, fpTypes);
    // ↑↑↑↑↑↑ funcType here.

    // To build a function var, we need its name, FPTypes, and its returns.
    // In a function, we need to store its instructions.
    // And instructions were stored in BasicBlocks.

    // Another question: do we need to analyze the relationship between BasicBlocks?
    // I don't know yet.

    // So I just...

    // Forgive me.

    // Here we add a function to the builder. At the end of HERE function,
    g_builder->createFunction(funcType, idName);
    // after that, the function was stored in g_builder->thisFunction

    // Func Block

    g_sw->isInFunc.dive(true);
    context->block()->accept(this);
    g_sw->isInFunc.ascend();

    // now we finished a function.
    g_builder->finishFunction();
    LOGD("Exit  FuncDef");
    return 0;
}

/// changes: this->curBType
/// \param context
/// \return this->curBType;
std::any Visitor::visitFuncType_(SysyParser::FuncType_Context* context)
{
    // FINAL
    if (dynamic_cast<SysyParser::FloatContext*>(context->bType())) {
        this->curBType = IRCtrl::IRValType::Float;
    } else {
        this->curBType = IRCtrl::IRValType::Int;
    }
    return this->curBType;
}

/// changes: this->curBType
/// \param context
/// \return Void
std::any Visitor::visitVoid(SysyParser::VoidContext* context)
{
    // FINAL
    this->curBType = IRCtrl::IRValType::Void;
    return IRCtrl::IRValType::Void;
}

/// funcFParams: funcFParam (Comma funcFParam)*;
/// \param context
/// \return vector<SPFPVar> t;
std::any Visitor::visitFuncFParams(SysyParser::FuncFParamsContext* context)
{
    vector<SPFPVar> t;
    for (auto& x : context->funcFParam()) {
        t.emplace_back(std::any_cast<SPFPVar>(x->accept(this)));
    }
    return t;
}

/// funcFParam:
//	bType Ident													# scalarParam
//	| bType Ident Lbracket Rbracket (Lbracket exp Rbracket)*	# arrayParam;
/// \param context
/// \return
std::any Visitor::visitScalarParam(SysyParser::ScalarParamContext* context)
{
    context->bType()->accept(this);   // this will change cur->bType
    SPType fParam;
    switch (this->curBType) {
    case IRValType::Int: {
        fParam = make_shared<IntType>();
    } break;
    case IRValType::Float: {
        fParam = make_shared<FloatType>();
    } break;
    default: break;
    }
    SPFPVar fp = make_shared<FPVar>(context->Ident()->getText());
    fp->fpType = fParam;
    return fp;
}

/// funcFParam:
/// 	| bType Ident Lbracket Rbracket (Lbracket exp Rbracket)*	# arrayParam;
/// \param context
/// \return
std::any Visitor::visitArrayParam(SysyParser::ArrayParamContext* context)
{
    context->bType()->accept(this);   // this will change cur->bType
    SPType fParam;

    // deal with shape
    vector<size_t> shape;
    shape.emplace_back(0);
    g_sw->isConst.dive(true);
    for (auto& e : context->exp()) {
        auto   a           = e->accept(this);
        auto   number_val  = std::any_cast<std::shared_ptr<CVal>>(a);
        auto   int_val     = std::dynamic_pointer_cast<IntCVal>(number_val);
        auto   float_val   = std::dynamic_pointer_cast<FloatCVal>(number_val);
        size_t thisDimSize = (int_val != nullptr) ? int_val->iVal : (size_t)float_val->fVal;
        shape.emplace_back(thisDimSize);
    }
    g_sw->isConst.ascend();

    fParam     = make_shared<ArrayType>(curBType, shape);
    SPFPVar fp = make_shared<FPVar>(context->Ident()->getText());
    fp->fpType = fParam;
    return fp;

    // ATTENTION: I didn't make a
}

/// block: Lbrace blockItem* Rbrace;
/// \param context
/// \return
std::any Visitor::visitBlock(SysyParser::BlockContext* context)
{
    // TODO visitBlock
    if (g_sw->isInFunc.get()) {
        // In a Function.
        g_lc->dive();
        for (auto& x : context->blockItem()) {
            x->accept(this);   // TODO
        }
        g_lc->ascend();
    } else {
        // Whatever in a function. Either the function itself, or the function's blocks.
    }
    return 0;
}

/// blockItem: decl | stmt;
/// \param context
/// \return
std::any Visitor::visitBlockItem(SysyParser::BlockItemContext* context)
{
    // FINAL
    if (context->decl())
        return context->decl()->accept(this);
    else if (context->stmt())
        return context->stmt()->accept(this);
    return 0;
}

/// stmt:
///	lVal Assign exp Semicolon					# assign
/// \param context
/// \return
std::any Visitor::visitAssign(SysyParser::AssignContext* context)
{
    if (g_sw->isInFunc.get()) {}
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
        // Non const, we need generate some code to store the result.
        // TODO.
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
    } else {
        // TODO
    }
    return 0;
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
    } else {
        //        TODO
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
    } else {
        // TODO
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
    } else {
        //        TODO
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
    } else {
        //        TODO
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
    } else {
        //        TODO
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
