//
// Created by gao on 7/3/23.
// Global decl for g_ prefix variables
//

#ifndef SYSYLEX_IRGLOBAL_H
#define SYSYLEX_IRGLOBAL_H

#include "IRBBController.h"
#include "IRGlobalSwitch.h"
#include "IRLayerController.h"
#include "IRBuilder.h"
namespace IRCtrl
{
extern std::shared_ptr<IRCtrl::IRBuilder>         g_builder;
extern std::shared_ptr<IRCtrl::IRLayerController> g_lc;
extern std::shared_ptr<IRCtrl::IRGlobalSwitch>    g_sw;
extern std::shared_ptr<IRCtrl::IRBBController>    g_bbc;
}   // namespace IRCtrl
#endif   // SYSYLEX_IRGLOBAL_H
