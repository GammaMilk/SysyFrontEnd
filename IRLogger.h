//
// Created by gao on 6/28/23.
//

#ifndef SYSYLEX_IRLOGGER_H
#define SYSYLEX_IRLOGGER_H


#include <iostream>
#include <fstream>

#ifndef _DEBUG
#define _DEBUG
#endif

std::ostream &sysy_debug();

#define ANSI_COLOR_BLUE "\033[1;34m"
#define ANSI_COLOR_ORANGE "\033[1;33m"
#define ANSI_COLOR_RESET "\033[0m"

#define LOGD(message) \
    do { \
        sysy_debug() << ANSI_COLOR_BLUE << "File: " << __FILE__ << ANSI_COLOR_RESET \
                  << ANSI_COLOR_ORANGE << ", Line: " << __LINE__ << ANSI_COLOR_RESET \
                  << ": " << message << std::endl; \
    } while(0)


#endif //SYSYLEX_IRLOGGER_H
