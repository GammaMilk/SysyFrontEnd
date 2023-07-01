//
// Created by gao on 7/1/23.
//

#include "IRBBController.h"

namespace IRCtrl
{
IRBBController::IRBBController() = default;
void IRBBController::dive()
{
    auto _1 = make_unique<IRBBLayer>();
    _layers.push_back(std::move(_1));
}
void IRBBController::ascend()
{
    _layers.pop_back();
}
const unique_ptr<IRBBLayer>& IRBBController::cur()
{
    return _layers.back();
}
}   // namespace IRCtrl