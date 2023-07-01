//
// Created by gao on 7/1/23.
//

#ifndef SYSYLEX_IRBBCONTROLLER_H
#define SYSYLEX_IRBBCONTROLLER_H
#include "IRBasicBlock.h"
#include "IRTypes.h"
namespace IRCtrl
{

using SPBB = shared_ptr<IRBasicBlock>;

class IRBBLayer
{
public:
    SPBB whileCondBB;
    SPBB trueBB;
    SPBB falseBB;
    SPBB whileBreakBB;
};

/// This class intends to deal with BRANCH and ITERATION
class IRBBController
{
public:
    IRBBController();
    void                         dive();
    void                         ascend();
    const unique_ptr<IRBBLayer>& cur();

    vector<unique_ptr<IRBBLayer>> _layers;
};

}   // namespace IRCtrl

#endif   // SYSYLEX_IRBBCONTROLLER_H
