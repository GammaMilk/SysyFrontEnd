//
// Created by gao on 6/30/23.
//

#include "IRBasicBlock.h"
namespace IRCtrl
{


void IRBasicBlock::add(UPLocalSen sen)
{
    this->instructions.emplace_back(std::move(sen));
}
}   // namespace IRCtrl