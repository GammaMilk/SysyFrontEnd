/* Copyright (c) 2012-2017 The ANTLR Project. All rights reserved.
 * Use of this file is governed by the BSD 3-clause license that
 * can be found in the LICENSE.txt file in the project root.
 */

//
//  main.cpp
//  antlr4-cpp-demo
//
//  Created by Mike Lischke on 13.03.16.
//

#include <fstream>
#include <iostream>
#include <ostream>

#include "SysyLexer.h"
#include "SysyParser.h"
#include "SysyVisitor.h"
#include "antlr4-runtime.h"
#include "tree/AbstractParseTreeVisitor.h"
#include "tree/ParseTree.h"
#include "IRLogger.h"
#include "IRBuilder.h"
#include "IRGlobal.h"

using namespace antlrcpp;
using namespace antlr4;
using std::cout;
using std::endl;
using tree::ErrorNode;
using tree::ParseTree;
using tree::TerminalNode;
constexpr bool      is_debug    = true;
static std::ostream null_stream = std::ostream(nullptr);

int main(int argc, const char** argv)
{
    std::string inputFileName;
    std::string outputFileName;
    std::string optimizationLevel;

    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);

        if (arg == "-o") {
            if (i + 1 < argc) {
                outputFileName = argv[i + 1];
                ++i;
            } else {
                std::cerr << "No output filename" << std::endl;
                return 1;
            }
        } else if (arg == "-O2" || arg == "-O1" || arg == "-O0") {
            optimizationLevel = arg;
        } else if (arg == "-S") {
            // do nothing
        } else if (arg == "-arm") {
            IRCtrl::IR_SWITCH_ENABLE_ARM_AAPCS_VFPCC = true;
        } else if (arg == "-dso") {
            IRCtrl::IR_SWITCH_ENABLE_DSO_LOCAL = true;
        } else {
            inputFileName = arg;
        }
    }
    if (!inputFileName.empty()) { LOGD("input: " << inputFileName); }

    // 处理缺省参数
    if (inputFileName.empty()) { inputFileName = "../testsrc/1.c"; }
    if (outputFileName.empty()) { outputFileName = "../testsrc/1.txt"; }
    std::ifstream inputStream;
    std::ofstream outputStream;

    auto sourceFileName = inputFileName;

    outputStream.open(outputFileName, std::ios::out);
    inputStream.open(sourceFileName, std::ios::in);
    if (!inputStream) {
        cout << "no such inputStream" << endl;
        return 0;
    }
    LOGD("File Fine." << inputFileName);
    ANTLRInputStream  input(inputStream);
    SysyLexer         lexer(&input);
    CommonTokenStream tokens(&lexer);

    SysyParser                   parser(&tokens);
    SysyParser::CompUnitContext* tree = parser.compUnit();

    auto irVisitor = IRVisitor();
    tree->accept(&irVisitor);

    cout << endl;

    IRCtrl::g_builder->setFilename(sourceFileName);
    IRCtrl::g_builder->build(outputStream);


    outputStream.close();
    return 0;
}
