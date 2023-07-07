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
    std::string inputFile;
    std::string outputFile;
    std::string optimizationLevel;

    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);

        if (arg == "-S") {
            // 处理 -S 参数
        } else if (arg == "-o") {
            if (i + 1 < argc) {
                outputFile = argv[i + 1];
                ++i;   // 跳过下一个参数
            } else {
                std::cerr << "缺少输出文件名" << std::endl;
                return 1;
            }
        } else if (arg == "-O1") {
            // 处理 -O1 参数
        } else {
            inputFile = arg;
        }
    }

    // 处理缺省参数
    if (inputFile.empty()) { inputFile = "../testsrc/1.c"; }
    if (outputFile.empty()) { outputFile = "../testsrc/1.txt"; }
    std::ifstream inputStream;
    std::ofstream outputStream;

    auto sourceFileName = "../testsrc/1.c";

    outputStream.open("../testsrc/1.txt", std::ios::out);
    inputStream.open(sourceFileName, std::ios::in);
    if (!inputStream) {
        cout << "no such inputStream" << endl;
        return 0;
    }
    LOGD("File Fine.");
    ANTLRInputStream  input(inputStream);
    SysyLexer         lexer(&input);
    CommonTokenStream tokens(&lexer);

    tokens.fill();
    for (auto token : tokens.getTokens()) {
        //        LOGD(token->toString());
    }

    SysyParser                   parser(&tokens);
    SysyParser::CompUnitContext* tree = parser.compUnit();

    // cout<<parser.blockItem()->toStringTree(true)<<endl;

    auto visitor = Visitor();
    // visitor.visit(tree);
    tree->accept(&visitor);
    // cout << tree->toStringTree(&parser) << endl << endl;

    cout << endl;

    //    outputStream << tree->toStringTree(&parser, true) << endl;

    IRCtrl::g_builder->setFilename(sourceFileName);
    IRCtrl::g_builder->build(outputStream);


    outputStream.close();
    return 0;
}
