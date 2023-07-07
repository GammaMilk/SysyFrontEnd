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

    // function
    void createFunction(IRCtrl::FuncType& type1, const string& name1);
    void finishFunction();
    // builder
    void        build(std::ostream& os);
    std::string build();
    void        addIntoCurBB(unique_ptr<LocalSen> sen);

    // label and sen getter
    int                                         getNewLabel();
    string                                      getNewLocalLabelStr();
    string                                      getLastLocalLabelStr() const;
    [[nodiscard]] int                           getLastLabel() const;
    [[nodiscard]] const unique_ptr<LocalSen>&   getLastSen() const;
    [[nodiscard]] const shared_ptr<IRFunction>& getFunction() const;

    // tools
    void checkTypeAndCast(SPType src, SPType target, string sourceName);
    /// Do a cast if type not eq or do nothing
    /// \param from source type
    /// \param to target type
    /// \param from_name source name
    void checkTypeAndCast(IRValType from, IRValType to, string from_name);


    // AddInstuctions into the builder.
    const unique_ptr<LocalSen>& addAdd(SPType t_, string v1, string v2);
    const unique_ptr<LocalSen>& addSub(SPType t_, string v1, string v2);

private:
    // Here stmts only means other stmt(other than function, var, const)
    std::vector<std::string>    _stmts;
    std::string                 _filename;
    int                         _label = 1000;
    std::shared_ptr<IRProgram>  program;
    std::shared_ptr<IRFunction> thisFunction;

public:
    [[nodiscard]] const shared_ptr<IRProgram>& getProgram() const;


private:
    void _as(const std::string& s);
};
}   // namespace IRCtrl


#endif   // SYSYLEX_IRBUILDER_H
