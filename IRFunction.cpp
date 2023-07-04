//
// Created by gao on 7/1/23.
//

#include "IRFunction.h"
#include "IRUtils.h"
namespace IRCtrl
{
string IRFunction::toString()
{
    /*
     * vector<SPLocalVar> alloca;
        vector<SPBasicBlock> bbs;
        SPBasicBlock curBB;
        FuncType _type;
     */
    stringstream ss;
    string       retTypeStr = Utils::valTypeToStr(this->_type.retType);
    // define i32 @funcName(
    ss << "define " << retTypeStr << " @" << this->name << "(";
    // i32 %arg_0, [59 x i32]* %arg_1,
    size_t labelNum = 0;
    for (auto& t : this->_type.paramsType) {
        ss << t->toString() << " %arg_" << labelNum << ",";
        labelNum += 1;
    }
    if (labelNum != 0) {
        string s = ss.str();
        if (!s.empty()) { s.pop_back(); }
        ss.str("");
        ss << s;
    }

    //    string s = ss.str();
    //    s=s.substr(0,s.size()-2);
    //    ss.clear();
    ss << ") {\n";

    // TODO func BBs
    for (auto& b : bbs) {
        ss << b->name << ":\n";
        for (auto& s : b->instructions) { ss << "    " << s->toString() << "\n"; }
        ss << "\n";
    }

    ss << "\n}";
    return ss.str();
}
}   // namespace IRCtrl