//
// Created by gao on 6/28/23.
//
#include "IRLogger.h"

namespace IRCtrl
{
std::ofstream g_debug_null_out = std::ofstream("/dev/null");
bool          g_enable_log     = true;
std::ostream& sysy_debug()
{
#ifdef _DEBUG
    if (g_enable_log)
        return std::cerr;
    else
        return g_debug_null_out;
#else
    return g_debug_null_out;
#endif
}
}   // namespace IRCtrl
