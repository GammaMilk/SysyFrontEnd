//
// Created by gao on 6/28/23.
//

#include "IRLayerController.h"
#include "IRLogger.h"
#include <iostream>

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
/// \param sen
/// \return
IRCtrl::IRSen& IRCtrl::IRLayerController::query(const std::shared_ptr<IRSen>& sen, bool recursively)
{
    CHECK_LAYER_EMPTY;
    auto ss = this->_layers.back();

    // Check if sen in the xs
    auto sr = ss.symbols.find(sen->name);
    if (sr == ss.symbols.end()) { LOGD("ERROR, Unexpected end"); }
    return *sr->second;
}

///
/// \param symbol_name
/// \return
IRCtrl::IRSen& IRCtrl::IRLayerController::query(const std::string& symbol_name, bool recursively)
{
    CHECK_LAYER_EMPTY;
    auto ss = this->_layers.back();
    auto sr = ss.symbols.find(symbol_name);
    if (sr == ss.symbols.end()) { LOGD("ERROR, Unexpected end"); }
    return *sr->second;
}

void IRCtrl::IRLayerController::push(const std::shared_ptr<IRSen>& sen)
{
    CHECK_LAYER_EMPTY;
    this->_layers.back().symbols[sen->name] = sen;
}
