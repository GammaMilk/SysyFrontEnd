//
// Created by gao on 6/28/23.
//
#include "IRLogger.h"

std::ofstream g_debug_out;

std::ostream &sysy_debug()
{
#ifdef _DEBUG
    return std::cerr;
#else
    g_debug_out = std::ofstream("/dev/null");
    return g_debug_out;
#endif
}