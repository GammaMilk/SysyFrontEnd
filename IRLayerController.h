//
// Created by gao on 6/28/23.
//

#ifndef SYSYLEX_IRLAYERCONTROLLER_H
#define SYSYLEX_IRLAYERCONTROLLER_H

#include "IRLayer.h"

#define CHECK_LAYER_EMPTY if (this->_layers.empty()) throw std::runtime_error("Divide by zero exception")
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

        void enter();

        void exit();

        /**
         * Push a sentence into the symbol table.
         * @param sen
         */
        void push(const std::shared_ptr<IRSen> &sen);

        IRCtrl::IRSen &query(const std::shared_ptr<IRSen> &sen, bool recursively = true);

        IRCtrl::IRSen &query(const std::string &symbol_name, bool recursively = true);

    private:
        std::vector<IRLayer> _layers;
    public:
        [[maybe_unused]] [[nodiscard]] bool isInGlobal() const;
    };
}


#endif //SYSYLEX_IRLAYERCONTROLLER_H
