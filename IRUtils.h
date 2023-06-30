//
// Created by gao on 6/29/23.
//

#ifndef SYSYLEX_IRUTILS_H
#define SYSYLEX_IRUTILS_H

#include <string>
#include <stdexcept>

namespace IRCtrl
{
class Utils
{
public:
    static int parseInteger(const std::string& str);

    static int tryParseInteger(const std::string& str);
};   // IRCtrl
}   // namespace IRCtrl

#endif   // SYSYLEX_IRUTILS_H
