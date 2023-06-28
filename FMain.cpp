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

using namespace antlrcpp;
using namespace antlr4;
using std::cout;
using std::endl;
using tree::ErrorNode;
using tree::ParseTree;
using tree::TerminalNode;
constexpr bool is_debug = true;
static std::ostream null_stream = std::ostream(nullptr);


int main(int, const char **)
{
    std::ifstream file;
    std::ofstream outfile;
    outfile.open("../testsrc/1.txt", std::ios::out);
    file.open("../testsrc/1.c", std::ios::in);
    if (!file)
    {
        cout << "no such file" << endl;
        return 0;
    }
    LOGD("File Fine.");
    ANTLRInputStream input(file);
    SysyLexer lexer(&input);
    CommonTokenStream tokens(&lexer);

    tokens.fill();
    for (auto token: tokens.getTokens())
    {
        // cout << token->toString() << endl;
    }

    SysyParser parser(&tokens);
    SysyParser::CompUnitContext *tree = parser.compUnit();

    // cout<<parser.blockItem()->toStringTree(true)<<endl;

    auto visitor = Visitor();
    // visitor.visit(tree);
    tree->accept(&visitor);
    // cout << tree->toStringTree(&parser) << endl << endl;

    cout << endl;

    outfile << tree->toStringTree(&parser, true) << endl;
    outfile.close();
    return 0;
}