//
// Created by gao on 6/26/23.
//

#ifndef SYSYLEX_IRBUILDER_H
#define SYSYLEX_IRBUILDER_H


#include <cstdio>
#include <string>
#include <vector>


class IRBuilder
{
public:
    IRBuilder();

    IRBuilder(std::string filename);

    void addVarDeclStmt(ssize_t t, std::string name);

    void build(std::ostream &os);

    std::string build();

private:
    std::vector<std::string> _stmts;
    std::string _filename;

    void _as(const std::string &s);
};


#endif //SYSYLEX_IRBUILDER_H
