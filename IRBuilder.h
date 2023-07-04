//
// Created by gao on 6/26/23.
//

#ifndef SYSYLEX_IRBUILDER_H
#define SYSYLEX_IRBUILDER_H


#include <cstdio>
#include <string>
#include <vector>
#include "IRLayerController.h"
#include "SysyParser.h"
#include "IRVal.h"
#include "IRProgram.h"


namespace IRCtrl
{
class IRBuilder
{
public:
    IRBuilder();

    explicit IRBuilder(const std::string& filename);
    void setFilename(const std::string& filename);
    void createFunction(IRCtrl::FuncType& type1, const string& name1);
    void finishFunction();
    void build(std::ostream& os);
    void addIntoCurBB(unique_ptr<LocalSen> sen);

    int               getNewLabel();
    [[nodiscard]] int getLastLabel() const;

    std::string                                 build();
    [[nodiscard]] const shared_ptr<IRFunction>& getFunction() const;


private:
    // Here stmts only means other stmt(other than function, var, const)
    std::vector<std::string>   _stmts;
    std::string                _filename;
    int                        _label = 100;
    std::shared_ptr<IRProgram>  program;
    std::shared_ptr<IRFunction> thisFunction;

public:
    const shared_ptr<IRProgram>& getProgram() const;


public:
    [[nodiscard]] bool isInGlobalScope() const;

    void setInGlobalScope(bool x);

private:
    void _as(const std::string& s);
};
}   // namespace IRCtrl


#endif   // SYSYLEX_IRBUILDER_H
