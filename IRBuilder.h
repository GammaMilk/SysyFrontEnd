//
// Created by gao on 6/26/23.
//

#ifndef SYSYLEX_IRBUILDER_H
#define SYSYLEX_IRBUILDER_H


#include <cstdio>
#include <string>
#include <vector>
#include "IRLayerController.h"


namespace IRCtrl
{
    class IRBuilder
    {
    public:
        IRBuilder();

        explicit IRBuilder(const std::string &filename);

        void addGlobalVarDeclStmt(ssize_t t, const std::string &name);

        void addConstVarDeclStmt(ssize_t t, const std::string &name);

        void build(std::ostream &os);

        int getNewLabel();

        int getLastLabel() const;

        std::string build();

        bool isInGlobal();

    private:
        std::vector<std::string> _stmts;
        std::string _filename;
        bool _inGlobalScope = true;
        int _label = 100;
        IRCtrl::IRLayerController _lc;
    public:
        [[nodiscard]] bool isInGlobalScope() const;

        void setInGlobalScope(bool x);

    private:

        void _as(const std::string &s);
    };
}


#endif //SYSYLEX_IRBUILDER_H
