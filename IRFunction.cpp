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

    if(onlyDecl) {
        // declare void @putarray(i32, i32*)
        stringstream ss;
        string       retTypeStr = Utils::valTypeToStr(this->_type.retType);
        // define i32 @funcName(
        ss << "declare " << retTypeStr << " @" << this->name << "(";
        // i32, i32*
        size_t labelNum = 0;
        for (auto& t : this->_type.paramsType) {
            ss << t->toString() << ", ";
            labelNum += 1;
        }
        // cut the last ","
        if (labelNum != 0) {
            string s = ss.str();
            if (!s.empty()) {
                s.pop_back();
                s.pop_back();
            }
            ss.str("");
            ss << s;
        }
        ss << ")";
        return ss.str();
    }

    stringstream ss;
    string       retTypeStr = Utils::valTypeToStr(this->_type.retType);
    // define i32 @funcName(
    ss << "define " << retTypeStr << " @" << this->name << "(";
    // i32 %arg_0, [59 x i32]* %arg_1,
    size_t labelNum = 0;
    for (auto& t : this->_type.paramsType) {
        ss << t->toString() << " %arg_" << labelNum << ", ";
        labelNum += 1;
    }

    // del the last ","
    if (labelNum != 0) {
        string s = ss.str();
        if (!s.empty()) {
            s.pop_back();
            s.pop_back();
        }
        ss.str("");
        ss << s;
    }

    //    string s = ss.str();
    //    s=s.substr(0,s.size()-2);
    //    ss.clear();
    ss << ") {\n";

    vector<unique_ptr<LocalSen>> allocas;

    vector<unique_ptr<LocalSen>> notAllocas;
    // TODO func BBs
    for (auto& b : bbs) {
        ss << b->name << ":\n";
        for (auto& s : b->instructions) { ss << "    " << s->toString() << "\n"; }
        //        for (auto& instruction : b->instructions) {
        //            auto x = std::move(instruction);
        //            if (x->getOp() == IROp::ALLOCA) {
        //                allocas.emplace_back(std::move(x));
        //            } else {
        //                notAllocas.emplace_back(std::move(x));
        //            }
        //        }
        //        for (auto& x : allocas) { ss << "    " << x->toString() << "\n"; }
        //        for (auto& x : notAllocas) {
        //            ss << "    " << x->toString() << "\n";
        //            //        }
        //            ss << "\n";
        //        }

        ss << "}\n";
        return ss.str();
    }
}
size_t IRFunction::getParamsNum() const
{
    return _type.paramsType.size();
}

}   // namespace IRCtrl