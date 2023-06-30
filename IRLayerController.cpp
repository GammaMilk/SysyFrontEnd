//
// Created by gao on 6/28/23.
//

#include "IRLayerController.h"
#include "IRLogger.h"
#include <iostream>

namespace IRCtrl
{
[[maybe_unused]] bool IRCtrl::IRLayerController::isInGlobal() const
{
    return this->_layers.size() <= 1;
}


IRCtrl::IRLayerController::IRLayerController()
{
    enter();
}


void IRCtrl::IRLayerController::enter()
{
    IRLayer layer;
    this->_layers.emplace_back(layer);
}


void IRCtrl::IRLayerController::exit()
{
    this->_layers.pop_back();
}

///
/// \param val
/// \return
std::shared_ptr<IRCtrl::IRVal>
IRCtrl::IRLayerController::query(const std::shared_ptr<IRVal>& val, bool recursively)
{
    return query(val->name, recursively);
}

///
/// \param symbol_name
/// \return
std::shared_ptr<IRCtrl::IRVal>
IRCtrl::IRLayerController::query(const std::string& symbol_name, bool recursively)
{
    CHECK_LAYER_EMPTY;
    for (auto it = this->_layers.rbegin(); it != this->_layers.rend(); it++) {
        auto sy = it->symbols.find(symbol_name);
        if (sy != it->symbols.end()) { return sy->second; }
        if (!recursively) break;
    }
    return nullptr;
}


void IRCtrl::IRLayerController::push(const std::shared_ptr<IRVal>& sen)
{
    this->_layers[_layers.size() - 1].symbols.insert(std::make_pair(sen->name, sen));
    LOGD(this->_layers.back().symbols.size());
}
}   // namespace IRCtrl