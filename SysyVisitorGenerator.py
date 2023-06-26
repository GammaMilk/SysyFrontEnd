PREFIX = "SysyParser"
Classes1 = '''  class CompUnitContext;
  class CompUnitItemContext;
  class DeclContext;
  class ConstDeclContext;
  class BTypeContext;
  class ConstDefContext;
  class VarDeclContext;
  class VarDefContext;
  class InitValContext;
  class FuncDefContext;
  class FuncTypeContext;
  class FuncFParamsContext;
  class FuncFParamContext;
  class BlockContext;
  class BlockItemContext;
  class StmtContext;
  class ExpContext;
  class CondContext;
  class LValContext;
  class PrimaryExpContext;
  class IntConstContext;
  class FloatConstContext;
  class NumberContext;
  class UnaryExpContext;
  class StringConstContext;
  class FuncRParamContext;
  class FuncRParamsContext;
  class MulExpContext;
  class AddExpContext;
  class RelExpContext;
  class EqExpContext;
  class LAndExpContext;
  class LOrExpContext; '''.split("\n")


def main():
    for t in Classes1:
        funcname = f"{PREFIX}::{t.strip().strip(';').split(' ')[-1]}"
        s = f"std::any visit({funcname} *ctx){{throw std::runtime_error(\"visit {funcname} not implemented\");}}"
        print(s)
        s = f"std::any visitChildren({funcname} *ctx){{throw std::runtime_error(\"visitChildren {funcname} not implemented\");}}"
        print(s)
        s = f"std::any visitTerminal({funcname} *ctx){{throw std::runtime_error(\"visitTerminal {funcname} not implemented\");}}"
        print(s)
        s = f"std::any visitErrorNode({funcname} *ctx){{throw std::runtime_error(\"visitErrorNode {funcname} not implemented\");}}"
        print(s)


if __name__ == "__main__":
    main()
