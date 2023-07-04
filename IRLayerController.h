//
// Created by gao on 6/28/23.
//

#ifndef SYSYLEX_IRLAYERCONTROLLER_H
#define SYSYLEX_IRLAYERCONTROLLER_H

#include "IRLayer.h"

#define CHECK_LAYER_EMPTY \
    if (this->_layers.empty()) throw std::runtime_error("Layer Empty")
namespace IRCtrl
{
class IRLayerController
{
public:
    /**
     * Attention:: When call constructor, the default layer was created.
     *          So you need not to create another layer for Global layer.
     */
    IRLayerController();

    void dive();

    void ascend();

    /**
     * Push a sentence into the symbol table.
     * @param val the target val
     */
    void push(const std::shared_ptr<IRVal>& val);

    std::shared_ptr<IRVal> query(const std::shared_ptr<IRVal>& val, bool recursively = true);

    std::shared_ptr<IRVal> query(const std::string& symbol_name, bool recursively = true);

    std::shared_ptr<LocalVar> queryLocal(const std::string& symbol_name, bool recursively = true);

private:
    std::vector<IRLayer> _layers;

public:
    [[maybe_unused]] [[nodiscard]] bool isInGlobal() const;
};
}   // namespace IRCtrl


#endif   // SYSYLEX_IRLAYERCONTROLLER_H
