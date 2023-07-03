//
// Created by gao on 7/1/23.
//

#ifndef SYSYLEX_IRFUNCTION_H
#define SYSYLEX_IRFUNCTION_H
#include "IRTypes.h"
#include "IRVal.h"
#include "IRBasicBlock.h"
#include "IRBBController.h"
namespace IRCtrl
{

class IRFunction : public IRVal
{
public:
    vector<SPLocalVar>   alloca;
    vector<SPBasicBlock> bbs;
    SPBasicBlock         curBB;
    FuncType             _type;

    IRFunction(FuncType& type1, const string& name1)
        : _type(std::move(type1))
    {
        curBB = make_shared<IRBasicBlock>("LEntry");
        bbs.push_back(curBB);
        name = name1;
        type = IRValType::Func;
    }

public:
    string toString() override;
};

}   // namespace IRCtrl

#endif   // SYSYLEX_IRFUNCTION_H
