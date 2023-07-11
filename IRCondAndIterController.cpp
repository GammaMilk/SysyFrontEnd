//
// Created by gao on 7/1/23.
//

#include "IRCondAndIterController.h"

namespace IRCtrl
{

IRCondAndIterController::IRCondAndIterController() = default;
void IRCondAndIterController::pushIf(
    SPBB trueBB_, SPBB ifFalseBB_, SPBB ifAfterBB_, size_t curLayerNum_
)
{
    auto newLayer = make_shared<IRBBLayer>();
    newLayer->trueBB = trueBB_;
    newLayer->ifFalseBB = ifFalseBB_;
    newLayer->ifAfterBB = ifAfterBB_;
    newLayer->curLayerNum = curLayerNum_;
    newLayer->type = IRBBLayer::If;
    _layers.push_back(newLayer);
}
void IRCondAndIterController::pushWhile(
    SPBB trueBB_, SPBB whileCondBB_, SPBB whileBreakBB_, size_t curLayerNum_
)
{
    auto newLayer = make_shared<IRBBLayer>();
    newLayer->trueBB = std::move(trueBB_);
    newLayer->whileCondBB = std::move(whileCondBB_);
    newLayer->whileBreakBB = std::move(whileBreakBB_);
    newLayer->curLayerNum = curLayerNum_;
    newLayer->type = IRBBLayer::While;
    _layers.push_back(newLayer);
}
void IRCondAndIterController::pop() {
    _layers.pop_back();
}
SPBB IRCondAndIterController::queryIfTrueBB()
{
    for (auto it = _layers.rbegin(); it != _layers.rend(); it++) {
        if ((*it)->type == IRBBLayer::If) {
            return (*it)->trueBB;
        }
    }
    throw std::runtime_error("IRCondAndIterController::queryIfTrueBB: no if layer");
    return nullptr;
}
SPBB IRCondAndIterController::queryIfFalseBB()
{
    for (auto it = _layers.rbegin(); it != _layers.rend(); it++) {
        if ((*it)->type == IRBBLayer::If) {
            return (*it)->ifFalseBB;
        }
    }
    throw std::runtime_error("IRCondAndIterController::queryIfFalseBB: no if layer");
    return nullptr;
}
SPBB IRCondAndIterController::queryIfAfterBB()
{
    for (auto it = _layers.rbegin(); it != _layers.rend(); it++) {
        if ((*it)->type == IRBBLayer::If) {
            return (*it)->ifAfterBB;
        }
    }
    throw std::runtime_error("IRCondAndIterController::queryIfAfterBB: no if layer");
    return nullptr;
}
SPBB IRCondAndIterController::queryWhileBreakBB()
{
    for (auto it = _layers.rbegin(); it != _layers.rend(); it++) {
        if ((*it)->type == IRBBLayer::While) {
            return (*it)->whileBreakBB;
        }
    }
    throw std::runtime_error("IRCondAndIterController::queryWhileBreakBB: no while layer");
    return nullptr;
}
SPBB IRCondAndIterController::queryWhileCondBB()
{
    for (auto it = _layers.rbegin(); it != _layers.rend(); it++) {
        if ((*it)->type == IRBBLayer::While) {
            return (*it)->whileCondBB;
        }
    }
    throw std::runtime_error("IRCondAndIterController::queryWhileCondBB: no while layer");
    return nullptr;
}
SPBB IRCondAndIterController::queryWhileTrueBB()
{
    for (auto it = _layers.rbegin(); it != _layers.rend(); it++) {
        if ((*it)->type == IRBBLayer::While) {
            return (*it)->trueBB;
        }
    }
    throw std::runtime_error("IRCondAndIterController::queryWhileTrueBB: no while layer");
    return nullptr;
}
size_t IRCondAndIterController::queryWhileCurLayerNum()
{
    for (auto it = _layers.rbegin(); it != _layers.rend(); it++) {
        if ((*it)->type == IRBBLayer::While) {
            return (*it)->curLayerNum;
        }
    }
    throw std::runtime_error("IRCondAndIterController::queryWhileCurLayerNum: no while layer");
    return -1;
}
}   // namespace IRCtrl