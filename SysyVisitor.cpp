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
std::shared_ptr<IRCtrl::IRBuilder> g_builder =
    std::make_shared<IRCtrl::IRBuilder>(IRCtrl::IRBuilder());
std::shared_ptr<IRCtrl::IRLayerController> g_lc =
    std::make_shared<IRCtrl::IRLayerController>(IRCtrl::IRLayerController());
std::shared_ptr<IRCtrl::IRGlobalSwitch> g_sw =
    std::make_shared<IRCtrl::IRGlobalSwitch>(IRCtrl::IRGlobalSwitch());
}   // namespace IRCtrl

using namespace IRCtrl;
using IRCtrl::g_builder;
using IRCtrl::g_lc;
using IRCtrl::g_sw;
#define IR_IS_CONST g_sw->isConst.get()

// tool functions for Visitor
/// from a std::any val to get last value.
/// \param aVal
/// \return
std::tuple<IRValType, string> getLastValue(std::any aVal)
{
    if (aVal.type() == typeid(shared_ptr<CVal>)) {
        auto irval = ACS(CVal, aVal);
        return {irval->type, irval->toString()};
    } else if (aVal.type()==typeid(int) && std::any_cast<int>(aVal) == 0x7de6543d) {
        return {IRValType::Unknown, ""};
    } else {
        auto& lastSen = g_builder->getLastSen();
        return {lastSen->_retType->type, lastSen->_label};
    }
}
// ---end tool functions

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
        auto              bType = parent->bType();
        IRCtrl::IRValType type;
        if (dynamic_cast<SysyParser::FloatContext*>(bType)) {
            type = IRCtrl::IRValType::Float;
        } else {
            type = IRCtrl::IRValType::Int;
        }
        auto idName = string(context->Ident()->getText());

        auto initVal = context->initVal();

        // not array
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
                    // local const int
                    string funcName         = g_builder->getFunction()->name;
//                    string  = funcName + "." + idName;
                    auto localConstIdName=Utils::localConstName(funcName, idName);
                    // Can we init like this way???

                    auto thisIntConstVal = std::make_shared<IntCVal>(localConstIdName, num);
                    g_lc->pushGlobal(thisIntConstVal);
                    g_builder->getProgram()->addGlobalConst(thisIntConstVal);
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
                    // local const int
                    string funcName         = g_builder->getFunction()->name;
                    auto localConstIdName=Utils::localConstName(funcName, idName);
                    // Can we init like this way???
                    auto thisIntConstVal = std::make_shared<FloatCVal>(localConstIdName, num);
                    g_lc->pushGlobal(thisIntConstVal);
                    g_builder->getProgram()->addGlobalConst(thisIntConstVal);
                }
            }


        } else {
            // array cosnt val list

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
                shared_ptr<CArr> thisArr;
                string funcName = g_builder->getFunction()->name;
                auto thisArrGlobalName=Utils::localConstName(funcName, idName);
                thisArr       = Utils::buildAnCArrFromInitList(iList, shape);
                thisArr->name = thisArrGlobalName;
                thisArr->id=thisArrGlobalName;
                g_lc->pushGlobal(thisArr);
                g_builder->getProgram()->addGlobalConst(thisArr);
                // TODO generate store inst
            }
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

std::any GlobalVarDef(SysyParser::VarDefContext* context, Visitor* this_)
{
    // vars type
    IRCtrl::IRValType type = this_->curBType;
    // idName
    string idName = context->Ident()->getText();
    LOGD("Enter VisitVarDef, id == " << idName);

    // Only in Global, the initList can be initialized as const numbers.
    g_sw->isConst.dive(true);

    if (context->exp().empty()) {

        // float/int a (= ?)*
        // get the init val if exists
        float fInit = 0;
        int   iInit = 0;
        // only not array, not lVal can be initialized by this_ way.
        // but how can we know is that a lVal?
        // use try...
        if (context->initVal() != nullptr && context->exp().empty()) {
            g_sw->isConst.dive(true);
            auto initVal  = std::any_cast<shared_ptr<CVal>>(context->initVal()->accept(this_));
            auto fValInit = std::dynamic_pointer_cast<FloatCVal>(initVal);
            auto iValInit = std::dynamic_pointer_cast<IntCVal>(initVal);
            if (fValInit != nullptr) {
                fInit = fValInit->fVal;
                iInit = (int)fInit;
            } else {
                iInit = iValInit->iVal;
                fInit = (float)iInit;
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
            auto   a           = e->accept(this_);
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
            auto iList_a = context->initVal()->accept(this_);
            iList        = any_cast<shared_ptr<InitListVal>>(iList_a);
        }
        iList->contained = this_->curBType;
        auto thisArr     = Utils::buildAnVArrFromInitList(iList, shape);
        thisArr->name    = idName;
        thisArr->advancedType = make_shared<ArrayType>(this_->curBType, vector<size_t>(shape.begin(), shape.end()));
        g_lc->push(thisArr);
        g_builder->getProgram()->addGlobalVar(thisArr);
    }

    g_sw->isConst.ascend();
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
        return GlobalVarDef(context, this);
    }
    else {
        g_enable_log = true;
        LOGD("Local Var name == " << idName << ",  type = " << Utils::valTypeToStr(type));
        int           newLabel  = g_builder->getNewLabel();
        const string& newLabelS = "%" + std::to_string(newLabel);
        // local single:
        if (context->exp().empty()) {
            // local float/int a (= ?)*
            auto sen = make_unique<AllocaSen>(newLabelS, makeType(type));
            g_builder->addIntoCurBB(std::move(sen));

            // make a object and push it into LayerController and Builder
            if (type == IRCtrl::IRValType::Float) {
                auto vVal = make_shared<LocalFloat>(newLabelS, idName);
                g_lc->push(vVal);
            } else {
                auto vVal = make_shared<LocalInt>(newLabelS, idName);
                g_lc->push(vVal);
            }

            // All the code below is about init the val.
            // get the init val if exists

            // only not array, not lVal can be initialized by this way.
            // but how can we know is that a lVal?
            // use try...
            if (context->initVal() != nullptr) {
                // initVal may be lVal exp or pure number(CVal)
                shared_ptr<CVal> initVal;
                g_sw->isConst.dive(false);
                auto anyResult = context->initVal()->accept(this);
                g_sw->isConst.ascend();
                if (g_sw->isCVal) {
                    initVal                  = ACS(CVal, anyResult);
                    auto [pos, iInit, fInit] = Utils::parseCVal(initVal);
                    string actualValS;
                    if (type == IRCtrl::IRValType::Float) {
                        actualValS = Utils::floatTo64BitStr(fInit);
                    } else {
                        actualValS = std::to_string(iInit);
                    }
                    auto storeSen = MU<StoreSen>(newLabelS, makeType(type), actualValS);
                    g_builder->addIntoCurBB(std::move(storeSen));
                } else {
                    auto lastLabel = g_builder->getLastLocalLabelStr();
                    // TODO we need a store. check type.
                    auto storeSen = MU<StoreSen>(newLabelS, makeType(type), lastLabel);
                    g_builder->addIntoCurBB(std::move(storeSen));
                }
            }

        } else {
            /*
             *   _                 _     _                      _                    _
                | |   ___  __ __ _| |   /_\  _ _ _ _   _ _  ___| |_   __ ___ _ _  __| |_
                | |__/ _ \/ _/ _` | |  / _ \| '_| '_| | ' \/ _ \  _| / _/ _ \ ' \(_-<  _|
                |____\___/\__\__,_|_| /_/ \_\_| |_|   |_||_\___/\__| \__\___/_||_/__/\__|
             */

            // Array 1 : deal with shape
            g_sw->isConst.dive(true);
            std::deque<size_t> shape;
            for (auto& e : context->exp()) {
                auto a                      = e->accept(this);
                auto number_val             = std::any_cast<std::shared_ptr<CVal>>(a);
                auto [position, iVal, fVal] = Utils::parseCVal(number_val);
                size_t thisDimSize          = iVal;
                shape.emplace_back(thisDimSize);
            }
            g_sw->isConst.ascend();

            // Alloca inserted.
            auto arrType =
                make_shared<ArrayType>(curBType, std::vector<size_t>(shape.begin(), shape.end()));
            auto allocaSen = make_unique<AllocaSen>(newLabelS, arrType);
            g_builder->addIntoCurBB(std::move(allocaSen));

            // local Array 2 : deal with init data
            // attention: we needn't initialize the actual val into that VArr.
            // but we need to init the val
            shared_ptr<InitListVal> iList;
            iList            = make_shared<InitListVal>();
            iList->contained = this->curBType;
            auto thisArr     = Utils::buildAnVArrFromInitList(iList, shape);
            thisArr->advancedType=arrType;
            thisArr->name    = idName;
            thisArr->id=newLabelS;
            g_lc->push(thisArr);

            // local Array 3: load data by visiting init list.
            if (context->initVal()) {
                size_t szArr = 4;   // i32 is 4
                for (auto x : shape) { szArr *= x; }
                // whether init or not, first do memset.
                auto memSet = MU<Memset>(
                    g_builder->getNewLocalLabelStr(), newLabelS, makePointer(arrType), 0, szArr
                );
                g_builder->addIntoCurBB(std::move(memSet));

                // pass the shape to the init list
                // attention: this will pass to visitInitList. because there's no single value after
                // an array.
                this->curShape = vector<size_t>(shape.begin(), shape.end());
                this->curArrayPos.resize(curShape.size(), 0);
                curArrayDim = 0;
                curArrId    = newLabelS;
                curArrType  = arrType;
                context->initVal()->accept(this);   // check visitInitList
                // finally add into lc. already inserted before. nothing to do here.
            }
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

/// According to shape, add N to cur.
/// \param shape [2][3]
/// \param cur [1][2]
/// \param N
/// \param startsAt
/// \param reset
void ArrayPosPlusN(const vector<size_t>& shape, vector<size_t>& cur, size_t N, int startsAt=-1, bool reset=true)
{
    if(startsAt==-1)
        startsAt = shape.size()-1;
    for (int i = startsAt; i >= 0; --i) {   // ATTENTION!!!!!!!!
        cur[i] += N;
        if (cur[i] < shape[i]) { break; }
        N = cur[i] / shape[i];
        cur[i] %= shape[i];
    }
    if (reset) {
        for (int i = startsAt+1; i < shape.size(); ++i) {
            cur[i] = 0;
        }
    }
}

/// initVal: {{},1, 2, {}, {5}}
/// Lbrace (initVal (Comma initVal)*)? Rbrace	# initList;
/// \param context
/// \return 0
std::any Visitor::visitInitList(SysyParser::InitListContext* context)
{
    if (IR_IS_CONST) {
        auto ril = make_shared<InitListVal>();
        for (auto& x : context->initVal()) {
            auto a = x->accept(this);   // ACCEPT
            try {
                auto il = ACS(InitListVal, a);
                ril->initList.emplace_back(il);
                ril->which.emplace_back(IRCtrl::InitListVal::INITLIST);
            } catch (const std::bad_any_cast&) {
                auto cv = any_cast<shared_ptr<CVal>>(a);
                ril->cVal.emplace_back(cv);
                ril->which.emplace_back(IRCtrl::InitListVal::CVAL);
            }
        }
        return ril;
    }
    // Not Const init. may need read from other variables.
    else {
        // int e[4][5]={{1,2,3,4,5},{},3};
        auto  arrInnerType = curBType;
        for (auto& init : context->initVal()) {
            int thisDimBefore = curArrayPos[curArrayDim];
            // $accept
            curArrayDim++;
            const std::any& accept = init->accept(this);
            curArrayDim--;
            // #accept
            auto [type, lastId]    = getLastValue(accept);
            if (type == IRCtrl::IRValType::Unknown) {
                // TODO this branch is InitList.
                if(curArrayPos[curArrayDim]==thisDimBefore) {
                    ArrayPosPlusN(curShape, curArrayPos,1,curArrayDim);
                }
            } else {
                // it was a Val no matter it is const
                if (type == IRCtrl::IRValType::Int && arrInnerType == IRCtrl::IRValType::Float) {
                    // convert Int to Float.
                    g_builder->checkTypeAndCast(type, arrInnerType, lastId);
                    lastId = g_builder->getLastLocalLabelStr();
                } else if (type == IRCtrl::IRValType::Float && arrInnerType == IRCtrl::IRValType::Int) {
                    // convert Float to Int.
                    g_builder->checkTypeAndCast(type, arrInnerType, lastId);
                    lastId = g_builder->getLastLocalLabelStr();
                }
                // get element ptr and store.
                auto gepLabel = g_builder->getNewLocalLabelStr();
                auto gep      = MU<GepSen>(gepLabel, curArrType, curArrId, curArrayPos);
                g_builder->addIntoCurBB(std::move(gep));
                auto st = MU<StoreSen>(gepLabel, makeType(arrInnerType), lastId);
                g_builder->addIntoCurBB(std::move(st));

                ArrayPosPlusN(curShape, curArrayPos, 1);
            }
        }
    }
    return 0x7de6543d;
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
        auto allocaSen =
            make_unique<AllocaSen>(g_builder->getNewLocalLabelStr(), fpTypes[l]);
        g_builder->addIntoCurBB(std::move(allocaSen));
        // store
        auto storeSen = MU<StoreSen>(
            g_builder->getLastLocalLabelStr(), fpTypes[l], "%arg_" + std::to_string(l)
        );
        g_builder->addIntoCurBB(std::move(storeSen));
        auto& fpVar = fps[l];
        fpVar->id   = g_builder->getLastLocalLabelStr(); // reallocate id
        LOGD("FPVAR name=" << fpVar->name << ", id=" << fpVar->id);
        g_lc->push(fpVar);
    }
    // visit its block
    context->block()->accept(this);
    // check void function has ret sen?
    if (funcType.retType == IRCtrl::IRValType::Void) {
        for (auto& bb : g_builder->getFunction()->bbs) {
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
    for (auto& x : context->blockItem()) { x->accept(this); }
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
        auto   lVal_a = context->lVal()->accept(this);
        auto   lVal   = ACS(IRVal, lVal_a);
        string target;
        auto   localVar = DPC(LocalVar, lVal);
        if (localVar != nullptr)
            target = localVar->id;
        else
            target = "@" + lVal->name;
        LOGD("assign target = " << target);
        auto type = makeType(lVal->type);

        g_sw->needLoad.dive(true);
        auto exp_a = context->exp()->accept(this);
        g_sw->needLoad.ascend();

        auto [ty, rs] = getLastValue(exp_a);
        if (lVal->type == IRCtrl::IRValType::Int && ty == IRCtrl::IRValType::Float) {
            g_builder->checkTypeAndCast(IRCtrl::IRValType::Float, IRCtrl::IRValType::Int, rs);
            rs = g_builder->getLastLocalLabelStr();
        } else if (lVal->type == IRCtrl::IRValType::Float && ty == IRCtrl::IRValType::Int) {
            g_builder->checkTypeAndCast(IRCtrl::IRValType::Int, IRCtrl::IRValType::Float, rs);
            rs = g_builder->getLastLocalLabelStr();
        }
        auto s = MU<StoreSen>(target, type, rs);
        g_builder->addIntoCurBB(std::move(s));
    }
    return 0;
}

/// 	| exp? Semicolon							# exprStmt
/// \param context
/// \return
std::any Visitor::visitExprStmt(SysyParser::ExprStmtContext* context)
{
    if(context->exp()!= nullptr) {
        return context->exp()->accept(this);
    }
    return 0x11451419;
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
        auto r           = context->exp()->accept(this);
        auto funcRetType = makeType(g_builder->getFunction()->_type.retType);
        if (g_sw->isCVal.get()) {
            auto   cv = ACS(CVal, r);
            size_t pos;
            int    iVal;
            float  fVal;
            std::tie(pos, iVal, fVal) = Utils::parseCVal(cv);
            if (pos > 0) {   // valid return number.
                if (g_builder->getFunction()->_type.retType == IRCtrl::IRValType::Int) {
                    auto retInt = MU<ReturnSen>(std::to_string(iVal), funcRetType);
                    g_builder->addIntoCurBB(std::move(retInt));
                } else {
                    auto retFl = MU<ReturnSen>(Utils::floatTo64BitStr(fVal), funcRetType);
                    g_builder->addIntoCurBB(std::move(retFl));
                }
            }
        }
        // LVal expression branch
        else {
            // it must inserted a sen into builder.
            auto& lastSen  = g_builder->getLastSen();
            auto  lastName = g_builder->getLastLocalLabelStr();
            g_builder->checkTypeAndCast(lastSen->_retType, funcRetType, lastName);
            auto ret = MU<ReturnSen>(g_builder->getLastLocalLabelStr(), funcRetType);
            g_builder->addIntoCurBB(std::move(ret));
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
    } catch (const std::bad_any_cast&) {
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
        // First of all, we must check if the val is array.

        string idName = context->Ident()->getText();
        if (context->exp().empty()) {
            // 1. query local const first.
            shared_ptr<IRVal> t  = g_lc->queryLocal(idName);
            if (t == nullptr) {
                // 2. query global.
                t = g_lc->queryLocalConst(idName, g_builder->getFunction()->name);
                if(t== nullptr) t=g_lc->query(idName);
                assert(t != nullptr);
            }
            g_sw->isCVal= true;
            return DPC(CVal, t);
        } else {
            // 3. deal with const array
            std::vector<int> indices;
            for (auto& e : context->exp()) {
                auto c              = e->accept(this);
                auto [_1, iVal, _2] = Utils::parseCVal(ACS(CVal, c));
                indices.emplace_back(iVal);
            }
            shared_ptr<IRVal> t  = g_lc->queryLocal(idName);
            if (t == nullptr) {
                // 2. query global.
                t = g_lc->queryLocalConst(idName, g_builder->getFunction()->name);
                if(t== nullptr) t=g_lc->query(idName);
                assert(t != nullptr);
            }
            auto cv   = DPC(CArr, t);
            auto cVal = cv->access(indices);
            g_sw->isCVal= true;
            return cVal;
        }
    } else {
        // Non const, we need generate some code to store the result.
        string idName = context->Ident()->getText();
        LOGD("lVal idName=" << idName);
        string            sourceId;
        bool              isLocal = true;
        shared_ptr<IRVal> t       = g_lc->queryLocal(idName);
        if (t == nullptr) {
            // query global.
            t = g_lc->queryLocalConst(idName,g_builder->getFunction()->name);
            if (t== nullptr) t=g_lc->query(idName);
            isLocal  = false;
            sourceId = "@" + t->name;
        } else {
            sourceId = t->id;
        }
        // if is array, need to get the element.
        if (g_sw->needLoad.get()) {
            g_sw->isCVal = false;
            // (1) on arrays.
            auto vArr = DPC(VArr, t);
            auto cArr = DPC(CArr, t);
            auto fpVar = DPC(FPVar,t);
            std::vector<size_t> shape;

            // Array 1 : deal with shape
            for (auto& e : context->exp()) {
                auto   a          = e->accept(this);
                auto [_,i,____] = Utils::parseCVal(ACS(CVal, a));
                size_t thisDimSize = i;
                shape.emplace_back(thisDimSize);
            }
            // the shape really match a terminal element on an array???
            // In sysylex, "const int[]" won't be passed by function args. so we needn't deal
            // with that in-ordinary thing.

            // varr or carr
            if (vArr || cArr || (fpVar&&fpVar->type==IRCtrl::IRValType::Arr)) {
                if (cArr) {
                    // no need to check if you idiot are accessing a const string.
                    vector<int> shape_(shape.begin(), shape.end());
                    g_sw->isCVal = true;
                    return cArr->access(shape_);
                } else if (vArr) {
                    // check terminal?
                    if(vArr->_shape.size()==shape.size()) {
                        // local varr / global varr (the same way.)
                        // get element ptr
                        auto gepSen = MU<GepSen>(g_builder->getNewLocalLabelStr(),vArr->getTrueAdvType(),vArr->id, shape);
                        gepSen->_retType = makeType(vArr->containedType);
                        auto gepLabel =g_builder->getLastLocalLabelStr();
                        // load from ptr
                        auto loadSen = MU<LoadSen>(g_builder->getNewLocalLabelStr(),makeType(vArr->containedType),gepLabel);
                        g_builder->addIntoCurBB(std::move(gepSen));
                        g_builder->addIntoCurBB(std::move(loadSen));
                        g_sw->isCVal=false;
                        return 0;
                    }
                    // not the terminal node.
                    else {
                        // TODO:
                        if(shape.empty()) {
                            auto gepSen = MU<GepSen>(g_builder->getNewLocalLabelStr(),vArr->getTrueAdvType(),sourceId, std::vector<size_t>{0});
                            gepSen->_retType = makeType(vArr->containedType);
                            auto gepLabel =g_builder->getLastLocalLabelStr();
                            g_builder->addIntoCurBB(std::move(gepSen));
                            g_sw->isCVal=false;
                            return 0;
                        }
                        // get element ptr
                        auto gepSen = MU<GepSen>(g_builder->getNewLocalLabelStr(),vArr->getTrueAdvType(),sourceId, shape);
                        gepSen->_retType = makeType(vArr->containedType);
                        auto gepLabel =g_builder->getLastLocalLabelStr();
                        g_builder->addIntoCurBB(std::move(gepSen));

                        auto newArrTypeTmp = MS<ArrayType>(*DPC(ArrayType, vArr->getTrueAdvType()));
                        auto n2 = std::deque<int>(newArrTypeTmp->innerShape.begin(), newArrTypeTmp->innerShape.end());
                        for(auto i=0;i<shape.size();i++) n2.pop_front();
                        newArrTypeTmp->innerShape=vector<size_t>(n2.begin(),n2.end());

                        auto gepSen2 = MU<GepSen>(g_builder->getNewLocalLabelStr(),newArrTypeTmp,gepLabel, vector<size_t>{0});
                        gepSen2->_retType = makeType(vArr->containedType);
                        auto gepLabel2 =g_builder->getLastLocalLabelStr();
                        g_builder->addIntoCurBB(std::move(gepSen2));

                        g_sw->isCVal= false;
                        return 0;
                    }
                } // on fpvar array.
                else if(fpVar&&fpVar->type==IRCtrl::IRValType::Arr) {
                    auto fpArrType = DPC(ArrayType, fpVar->fpType);
                    // terminal node, directly to the ass.
                    if(fpArrType->innerShape.size()==shape.size()) {
                        // 2 steps load like this
                        /*
                         *  %v3 = load [2 x i32]*, [2 x i32]** %v1
                            %v4 = getelementptr [2 x i32], [2 x i32]* %v3, i32 4, i32 1
                            %v5 = load i32, i32* %v4
                         */
                        auto loadSen1 = MU<LoadSen>(g_builder->getNewLocalLabelStr(),fpArrType,fpVar->id);
                        g_builder->addIntoCurBB(std::move(loadSen1));
                        auto s1Label = g_builder->getLastLocalLabelStr();

                        auto gepSen = MU<GepSen>(g_builder->getNewLocalLabelStr(),fpArrType,s1Label, shape, true);
                        g_builder->addIntoCurBB(std::move(gepSen));
                        auto s2Label = g_builder->getLastLocalLabelStr();

                        auto loadSen = MU<LoadSen>(g_builder->getNewLocalLabelStr(),makeType(IRCtrl::IRValType::Int), s2Label);
                        g_builder->addIntoCurBB(std::move(loadSen));
                        g_sw->isCVal=false;
                        return 0;
                    }
                    // not the terminal... fuck!!!!!
                    else {
                        // 0. no shape requested, just load and return.
                        if(shape.empty()) {
                            auto loadSen1 = MU<LoadSen>(g_builder->getNewLocalLabelStr(),fpArrType,fpVar->id);
                            g_builder->addIntoCurBB(std::move(loadSen1));
                            g_sw->isCVal=false;
                            return 0;
                        }

                        // 1. the same way load from alloca.
                        // %v3 = load [8 x i32]*, [8 x i32]** %v1
                        auto loadSen1 = MU<LoadSen>(g_builder->getNewLocalLabelStr(),fpArrType,fpVar->id);
                        g_builder->addIntoCurBB(std::move(loadSen1));
                        auto s1Label = g_builder->getLastLocalLabelStr();

                        // 2. gep the var but not completely.
                        // %v4 = getelementptr [8 x i32], [8 x i32]* %v3, i32 1
                        auto gepSen = MU<GepSen>(g_builder->getNewLocalLabelStr(),fpArrType,s1Label, shape, true);
                        g_builder->addIntoCurBB(std::move(gepSen));
                        auto s2Label = g_builder->getLastLocalLabelStr();

                        // 3. another gep, but pass 0,0
                        // %v5 = getelementptr [8 x i32], [8 x i32]* %v4, i32 0, i32 0
                        auto gepSen2 = MU<GepSen>(g_builder->getNewLocalLabelStr(),fpArrType,s2Label,vector<size_t>({0,0}), true);
                        g_builder->addIntoCurBB(std::move(gepSen2));
                        auto s3Label = g_builder->getLastLocalLabelStr();

                        g_sw->isCVal=false;
                        return 0;
                    }
                }
            }
            // else on single variable.
            else {
                unique_ptr<LocalSen> s =
                    MU<LoadSen>(g_builder->getNewLocalLabelStr(), t->getTrueAdvType(), sourceId);
                g_builder->addIntoCurBB(std::move(s));
            }
        }
        return t;
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
    // TODO 写了跟他妈没写一样这个判断。狗屁不通！
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
    if (IR_IS_CONST) {
        // if const, this func must return exact CVal
        return context->lVal()->accept(this);
    }
    // If in function:
    // Need Load, Not Const.
    if (g_sw->isInFunc.get()) {
        g_sw->needLoad.dive(true);
        g_sw->isConst.dive(false);
        auto ret = context->lVal()->accept(this);
        g_sw->isConst.ascend();
        g_sw->needLoad.ascend();
        g_sw->isCVal = false;
        return ret;
    } else {
        return context->lVal()->accept(this);
    }
}



/// A Const literal number
/// \param context
/// \return std::shared_ptr<CVal>
std::any Visitor::visitNumber(SysyParser::NumberContext* context)
{
    std::string num;
    g_sw->isCVal.set(true);
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
        trueNum                 = std::strtof(num.c_str(), nullptr);
        std::shared_ptr<CVal> n = std::make_shared<FloatCVal>("", trueNum);
        n->isConst              = g_sw->isConst.get();
        n->isGlobal             = g_lc->isInGlobal();
        return n;
    }
}

std::any Visitor::visitUnaryExp_(SysyParser::UnaryExp_Context* context)
{
    // FINAL
    return context->primaryExp()->accept(this);
}

/// Ident Lparen funcRParams? Rparen	# call
/// \param context
/// \return
std::any Visitor::visitCall(SysyParser::CallContext* context)
{
    string idName = context->Ident()->getText();
    // TODO parse funcrparams.
    if(context->funcRParams()!= nullptr) {
        context->funcRParams()->accept(this);
    }
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
        g_sw->isCVal = true;
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

/// funcRParam: exp | stringConst;
/// \param context
/// \return exp.accept
std::any Visitor::visitFuncRParam(SysyParser::FuncRParamContext* context)
{
    // Here stringConst was ignored.
    // FINAL
    return context->exp()->accept(this);
}

/// funcRParams: funcRParam (Comma funcRParam)*;
//  note: funcRParam: exp
/// \param context
/// \return
std::any Visitor::visitFuncRParams(SysyParser::FuncRParamsContext* context)
{
    // TODO visitFuncRParams
    for(auto r:context->funcRParam()) {
        r->accept(this);
    }
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

        auto res     = Utils::constBiCalc(lv, rv, IRCtrl::IRValOp::Div);
        g_sw->isCVal = true;
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

        auto res     = Utils::constBiCalc(lv, rv, IRCtrl::IRValOp::Mod);
        g_sw->isCVal = true;
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

        auto res     = Utils::constBiCalc(lv, rv, IRCtrl::IRValOp::Mul);
        g_sw->isCVal = true;
        return res;
    } else {   // addExp と mulExp IntORFloatCVal returnする
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
        auto lv  = ACS(CVal, lav);
        auto rv  = ACS(CVal, rav);

        auto res     = Utils::constBiCalc(lv, rv, IRCtrl::IRValOp::Add);
        g_sw->isCVal = true;
        return res;
    } else {
        IRValType lt;
        string ls;
        IRValType rt;
        string rs;

        bool isCVal[2];
        auto lav  = context->addExp()->accept(this);
        isCVal[0] = g_sw->isCVal.get();
        auto& lvs = g_builder->getLastSen();

        if (isCVal[0])
            lt = ACS(CVal, lav)->type;
        else
            lt = lvs->_retType->type;
        if (isCVal[0])
            ls = ACS(CVal, lav)->toString();
        else
            ls = lvs->_label;

        auto rav  = context->mulExp()->accept(this);
        isCVal[1] = g_sw->isCVal.get();
        auto& rvs = g_builder->getLastSen();

        if (isCVal[1])
            rt = ACS(CVal, rav)->type;
        else
            rt = rvs->_retType->type;
        if (isCVal[1])
            rs = ACS(CVal, rav)->toString();
        else
            rs = rvs->_label;

        if (isCVal[0] && isCVal[1]) {
            // 按照const的写法，直接算出来。
            auto lv  = ACS(CVal, lav);
            auto rv  = ACS(CVal, rav);
            auto res = Utils::constBiCalc(lv, rv, IRCtrl::IRValOp::Add);
            g_sw->isCVal.set(true);
            return res;
        }
        // not all const.......Let's gen some sentences.
        else {
            if (lt == IRCtrl::IRValType::Int) {
                if (rt != IRCtrl::IRValType::Int) {
                    g_builder->checkTypeAndCast(
                        makeType(IRCtrl::IRValType::Float), makeType(IRCtrl::IRValType::Int), rs
                    );
                    rs = g_builder->getLastLocalLabelStr();
                }
                g_builder->addAdd(makeType(IRCtrl::IRValType::Int), ls, rs);
            } else {
                if (rt != IRCtrl::IRValType::Float) {
                    g_builder->checkTypeAndCast(
                        makeType(IRCtrl::IRValType::Int), makeType(IRCtrl::IRValType::Float), rs
                    );
                    rs = g_builder->getLastLocalLabelStr();
                }
                g_builder->addAdd(makeType(IRCtrl::IRValType::Float), ls, rs);
            }
        }
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