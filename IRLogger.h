//
// Created by gao on 6/28/23.
//

#ifndef SYSYLEX_IRLOGGER_H
#define SYSYLEX_IRLOGGER_H


#include <iostream>
#include <fstream>

#ifndef _DEBUG
#    define _DEBUG
#endif

std::ostream& sysy_debug();

#define ANSI_COLOR_BLUE "\033[1;34m"
#define ANSI_COLOR_ORANGE "\033[1;33m"
#define ANSI_COLOR_YELLOW "\033[1;93m"
#define ANSI_COLOR_RED "\033[1;31m"
#define ANSI_COLOR_RESET "\033[0m"

#define LOGD(message)                                                                            \
    do {                                                                                         \
        sysy_debug() << ANSI_COLOR_BLUE << "[D] File: " << __FILE__ << ANSI_COLOR_RESET          \
                     << ANSI_COLOR_ORANGE << ", Line: " << __LINE__ << ANSI_COLOR_RESET << ":\t" \
                     << message << std::endl;                                                    \
    } while (0)
#define LOGW(message)                                                                            \
    do {                                                                                         \
        sysy_debug() << ANSI_COLOR_BLUE << "[W] File: " << __FILE__ << ANSI_COLOR_RESET          \
                     << ANSI_COLOR_ORANGE << ", Line: " << __LINE__ << ANSI_COLOR_RESET << ":\t" \
                     << ANSI_COLOR_YELLOW << message << ANSI_COLOR_RESET << std::endl;           \
    } while (0)

#define LOGE(message)                                                                            \
    do {                                                                                         \
        sysy_debug() << ANSI_COLOR_BLUE << "[E] File: " << __FILE__ << ANSI_COLOR_RESET          \
                     << ANSI_COLOR_ORANGE << ", Line: " << __LINE__ << ANSI_COLOR_RESET << ":\t" \
                     << ANSI_COLOR_RED << message << ANSI_COLOR_RESET << std::endl;              \
    } while (0)

#endif   // SYSYLEX_IRLOGGER_H
