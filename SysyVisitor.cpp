

#include "SysyVisitor.h"
#include "IRBuilder.h"
#include "IRLogger.h"
#include "IRUtils.h"
#include "IRExceptions.h"
#include "IRGlobalSwitch.h"
#include "IRBBController.h"
#include "IRGlobal.h"

namespace IRCtrl
{
    // Init builder and layer controller
    std::shared_ptr<IRCtrl::IRBuilder> g_builder = std::make_shared<IRCtrl::IRBuilder>(IRCtrl::IRBuilder());
    std::shared_ptr<IRCtrl::IRLayerController> g_lc = std::make_shared<IRCtrl::IRLayerController>(
            IRCtrl::IRLayerController());
    std::shared_ptr<IRCtrl::IRGlobalSwitch> g_sw = std::make_shared<IRCtrl::IRGlobalSwitch>(IRCtrl::IRGlobalSwitch());
}   // namespace IRCtrl

using namespace IRCtrl;
using IRCtrl::g_lc;
using IRCtrl::g_builder;
using IRCtrl::g_sw;

/// compUnit: compUnitItem* EOF;
/// \param context
/// \return
std::any Visitor::visitCompUnit(SysyParser::CompUnitContext* context)
{
    LOGD("Enter CompUnit");



    for (auto& x : context->compUnitItem()) { x->accept(this); }

    LOGD("Exit CompUnit\nWell done.");
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

/// constDecl: Const bType constDef (Comma constDef)* Semicolon;
/// \param context
/// \return
std::any Visitor::visitConstDecl(SysyParser::ConstDeclContext* context)
{
    // Set this layer to const.
    g_sw->isConst.dive();
    g_sw->isConst.set(true);
    context->bType()->accept(this);   // this will change curBType;
    for (auto& x : context->constDef()) { x->accept(this); }
    g_sw->isConst.ascend();           // resume
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

    // var's type
    IRCtrl::IRValType type = this->curBType;

    // idName
    string idName = context->Ident()->getText();
    LOGD("Enter VisitVarDef, id == " << idName);


    if (g_lc->isInGlobal()) {
        // Only in Global, the initList can be initialized as const numbers.
        g_sw->isConst.dive();
        g_sw->isConst.set(true);

        if (context->exp().empty()) {

            // float/int a (= ?)*
            // get the init val if exists
            float fInit = 0;
            int iInit = 0;
            // only not array, not lVal can be initialized by this way.
            // but how can we know is that a lVal?
            // use try...
            if (context->initVal() != nullptr && context->exp().empty()) {
                g_sw->isConst.dive(true);
                auto initVal = std::any_cast<shared_ptr<CVal>>(context->initVal()->accept(this));
                auto fValInit = std::dynamic_pointer_cast<FloatCVal>(initVal);
                auto iValInit = std::dynamic_pointer_cast<IntCVal>(initVal);
                if (fValInit != nullptr) {
                    fInit = fValInit->fVal;
                    iInit = (int) fInit;
                } else {
                    iInit = iValInit->iVal;
                    fInit = (float) iInit;
                }
                g_sw->isConst.ascend();
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
    }
        // TODO local define like local int/float id([])* (= ?)?
    else {
        g_enable_log = true;
        LOGD("Local Var name == " << idName << ",  type = " << Utils::valTypeToStr(type));
        int           newLabel  = g_builder->getNewLabel();
        const string& newLabelS = "%" + std::to_string(newLabel);
        if (context->exp().empty()) {
            // local float/int a (= ?)*
            auto sen = make_unique<AllocaSen>(newLabelS, makeType(type));
            g_builder->addIntoCurBB(std::move(sen));

            // make a object and push it into LayerController and Builder
            if (type == IRCtrl::IRValType::Float) {
                auto vVal = make_shared<LocalFloat>(newLabelS, idName);
                if (context->initVal() != nullptr) {
                    // TODO we need a "STORE" sentence.
                }
                g_lc->push(vVal);
            } else {
                auto vVal = make_shared<LocalInt>(newLabelS, idName);
                if (context->initVal() != nullptr) {
                    // local float has initialized
                    // TODO we need a "STORE" sentence.
                }
                g_lc->push(vVal);
            }

            // All the code below is about init the val.
            // get the init val if exists
            float fInit = 0;
            int iInit = 0;
            // only not array, not lVal can be initialized by this way.
            // but how can we know is that a lVal?
            // use try...
            if (context->initVal() != nullptr) {
                // initVal may be lVal exp or pure number(CVal)
                g_sw->isConst.dive(true);
                auto anyResult = context->initVal()->accept(this);
                shared_ptr<CVal> initVal;
                try {
                    initVal = ACS(CVal, anyResult);
                    auto fValInit = std::dynamic_pointer_cast<FloatCVal>(initVal);
                    auto iValInit = std::dynamic_pointer_cast<IntCVal>(initVal);
                    if (fValInit != nullptr) {
                        fInit = fValInit->fVal;
                        iInit = (int) fInit;
                    } else {
                        iInit = iValInit->iVal;
                        fInit = (float) iInit;
                    }
                    string actualValS;
                    if (type == IRCtrl::IRValType::Float) {
                        actualValS = Utils::floatTo64BitStr(fInit);
                    } else {
                        actualValS = std::to_string(iInit);
                    }
                    auto storeSen = MU<StoreSen>(g_builder->getNewLocalLabelStr(), makeType(type), actualValS);
                    g_builder->addIntoCurBB(std::move(storeSen));

                }
                    // Not Pure Num. it was lVal
                catch (const std::bad_any_cast &) {
                    // Oh, it was not a pure number...
                    // But it just a lVal or a lVal expression.
                    g_sw->isConst.dive(false);
                    auto lastLabel = g_builder->getLastLocalLabelStr();
                    // TODO we need a store
                    g_sw->isConst.ascend();
                }
                g_sw->isConst.ascend();

            }

        } else {

            // local Array
            // float/int a[][][][] (= ?)?

            // Array 1 : deal with shape
            g_sw->isConst.dive(true);
            std::deque<size_t> shape;
            for (auto& e : context->exp()) {
                auto   a           = e->accept(this);
                auto   number_val  = std::any_cast<std::shared_ptr<CVal>>(a);
                auto   int_val     = std::dynamic_pointer_cast<IntCVal>(number_val);
                auto   float_val   = std::dynamic_pointer_cast<FloatCVal>(number_val);
                size_t thisDimSize = (int_val != nullptr) ? int_val->iVal : (size_t)float_val->fVal;
                shape.emplace_back(thisDimSize);
            }
            g_sw->isConst.ascend();

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

            auto arrType =
                make_shared<ArrayType>(curBType, std::vector<size_t>(shape.begin(), shape.end()));
            auto allocaSen = make_unique<AllocaSen>(newLabelS, arrType);
            g_builder->addIntoCurBB(std::move(allocaSen));
            // TODO store constants into that array.
        }
        g_enable_log = false;
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
    context->funcType()->accept(this);   // this operation will change this->curBType

    // Func name
    string idName = context->Ident()->getText();
    LOGD("Enter FuncDef, name==" << idName);
    // Only for log
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch"
    switch (this->curBType) {
    case IRValType::Int: LOGD("Int Func"); break;
    case IRValType::Float: LOGD("Float Func"); break;
    case IRValType::Void: LOGD("Void Func"); break;
    }
#pragma clang diagnostic pop
    auto retType = this->curBType;

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
    FuncType funcType(retType, fpTypes);
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
    g_lc->dive();
    // operating on args.
    //   %v9 = alloca i32
    //  store i32 %arg_0, i32* %v9
    for (int l = 0; l < fpTypes.size(); l++) {
        // alloca
        auto I1lII1llI10Oll01O = make_unique<AllocaSen>(g_builder->getNewLocalLabelStr(), fpTypes[l]);
        g_builder->addIntoCurBB(std::move(I1lII1llI10Oll01O));
        // store
        auto l11lII1lI1lI1lI1l = MU<StoreSen>(g_builder->getLastLocalLabelStr(), fpTypes[l],
                                              "%arg_" + std::to_string(l));
        g_builder->addIntoCurBB(std::move(l11lII1lI1lI1lI1l));
        auto &fpVar = fps[l];
        fpVar->id = g_builder->getLastLocalLabelStr();
        LOGD("FPVAR name=" << fpVar->name << ", id=" << fpVar->id);
        g_lc->push(fpVar);
    }
    // visit its block
    context->block()->accept(this);
    // check void function has ret sen?
    if (funcType.retType == IRCtrl::IRValType::Void) {
        for (auto &bb: g_builder->getFunction()->bbs) {
            if (!bb->hasTerminalSen()) {
                auto retSen = MU<ReturnSen>("", makeType(IRCtrl::IRValType::Void));
                bb->add(std::move(retSen));
            }
        }
    }
    g_lc->ascend();
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
///
/// funcFParam:
///	bType Ident													# scalarParam
///	| bType Ident Lbracket Rbracket (Lbracket exp Rbracket)*	# arrayParam;
/// \param context
/// \return vector<SPFPVar> t;
std::any Visitor::visitFuncFParams(SysyParser::FuncFParamsContext* context)
{
    vector<SPFPVar> t;
    int             argN = 0;
    for (auto& x : context->funcFParam()) {
        auto v = std::any_cast<SPFPVar>(x->accept(this));
        v->id  = "arg_" + std::to_string(argN++);
        t.emplace_back(v);
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
    SPFPVar fp = make_shared<FPVar>("", context->Ident()->getText());
//    fp->fpType = fParam;
    fp->setFpType(fParam);
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
    SPFPVar fp = make_shared<FPVar>("", context->Ident()->getText());
//    fp->fpType = fParam;
    fp->setFpType(fParam);
    return fp;


    // ATTENTION: I didn't make a
}

/// block: Lbrace blockItem* Rbrace;
/// \param context
/// \return
std::any Visitor::visitBlock(SysyParser::BlockContext* context)
{
    if (!g_sw->isInFunc.get()) { g_lc->dive(); }
    // In a Function.
    // TODO visitBlock
    for (auto& x : context->blockItem()) {
        x->accept(this);   // TODO
    }
    if (!g_sw->isInFunc.get()) { g_lc->ascend(); }
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
    if (g_sw->isInFunc.get()) {
        auto lVal_a = context->lVal()->accept(this);
        g_sw->needLoad.dive(true);
        auto exp_a = context->exp()->accept(this);
        g_sw->needLoad.ascend();
    }
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

/// stmt:
///	| Return exp? Semicolon						# return;
/// \param context
/// \return
std::any Visitor::visitReturn(SysyParser::ReturnContext* context)
{
    if (context->exp() == nullptr) {
        g_builder->addIntoCurBB(MU<ReturnSen>("", makeType(IRCtrl::IRValType::Void)));
    } else {
        // TODO
        auto r = context->exp()->accept(this);
        if (g_sw->isCVal.get()) {
            auto cv = ACS(CVal, r);
            size_t pos;
            int iVal;
            float fVal;
            std::tie(pos, iVal, fVal) = Utils::parseCVal(cv);
            if (pos > 0) { // valid return number.
                if (g_builder->getFunction()->_type.retType == IRCtrl::IRValType::Int) {
                    auto retInt = MU<ReturnSen>(std::to_string(iVal), makeType(IRCtrl::IRValType::Int));
                    g_builder->addIntoCurBB(std::move(retInt));
                } else {
                    auto retFl = MU<ReturnSen>(Utils::floatTo64BitStr(fVal), makeType(IRCtrl::IRValType::Float));
                    g_builder->addIntoCurBB(std::move(retFl));
                }
            }
        }
            // LVal expression branch
        else {

        }
    }
    return 0;
}

/// If the result is CVal, it will return. Otherwise it doesn't return anything.
/// Instead, this function will add a sen into current function.
/// \param context
/// \return shared_ptr<CVal> or nothing.
std::any Visitor::visitExp(SysyParser::ExpContext* context)
{
    // FINAL.
    auto a = context->addExp()->accept(this);

    try {
        ACS(CVal, a);
        g_sw->isCVal.set(true);
        return a;
    } catch (const std::bad_any_cast &) {
        LOGD("bad any_cast at an exp()");
        g_sw->isCVal.set(false);
        return 0;
    }
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
        auto                     val   = std::dynamic_pointer_cast<CVal>(query);
        return val;
    } else {
        // Non const, we need generate some code to store the result.
        string idName = context->Ident()->getText();
        LOGD("idName=" << idName);
        string sourceId;
        bool isLocal = true;
        shared_ptr<IRVal> t = g_lc->queryLocal(idName);
        if (t == nullptr) {
            // query global.
            t = g_lc->query(idName);
            isLocal = false;
            sourceId = "@" + t->name;
        } else {
            sourceId = dynamic_pointer_cast<LocalVar>(t)->id;
        }
        // need load the exact value from memory?
        // TODO FIX IT
        if (g_sw->needLoad.get()) {
            unique_ptr<LocalSen> s = MU<LoadSen>(g_builder->getNewLocalLabelStr(), t->getTrueAdvType(), sourceId);
            g_builder->addIntoCurBB(std::move(s));
        }
        // TODO.
    }
    return 0;
}

/// primaryExp:
//	Lparen exp Rparen	# primaryExp_       <-----------
//	| lVal				# lValExpr
//	| number			# primaryExp_       <-----------
/// ATTENTION: THIS RULE HAS 2: exp and number!!!!!!!!
/// \param context
/// \return
std::any Visitor::visitPrimaryExp_(SysyParser::PrimaryExp_Context* context)
{
    // 写了跟他妈没写一样这个判断。狗屁不通！
    // TODO
    if (g_sw->isConst.get()) {
        if (context->exp()) {
            return context->exp()->accept(this);
        } else {
            return context->number()->accept(this);
        }
    } else {
        if (context->exp()) {
            return context->exp()->accept(this);
        } else {
            return context->number()->accept(this);
        }
    }
    return 0;
}
/// primaryExp:
//	Lparen exp Rparen	# primaryExp_
//	| lVal				# lValExpr      <-----------
/// \param context
/// \return
std::any Visitor::visitLValExpr(SysyParser::LValExprContext* context)
{
    // If in function:
    // Need Load, Not Const.
    if (g_sw->isInFunc.get()) {
        g_sw->needLoad.dive(true);
        g_sw->isConst.dive(false);
        auto ret = context->lVal()->accept(this);
        g_sw->isConst.ascend();
        g_sw->needLoad.ascend();
        return ret;
    }
        // else not in function, outside, just calc the actually value.
    else {
        return context->lVal()->accept(this);
    }
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
    } else {        // addExp と mulExp IntORFloatCVal returnする
//        auto lav = context->mulExp()->accept(this);
//        auto rav = context->unaryExp()->accept(this);
//        auto lv  = std::any_cast<std::shared_ptr<LocalVar>>(lav);
//        auto rv  = std::any_cast<std::shared_ptr<LocalVar>>(rav);
//
//
//        return res;
        LOGD("VisitMul Not Const!!!!!");
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
