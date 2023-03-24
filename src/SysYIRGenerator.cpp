#include <iostream>
#include <memory>
#include <vector>
using namespace std;
#include "SysYIRGenerator.h"

namespace sysy {

any SysYIRGenerator::visitModule(SysYParser::ModuleContext *ctx) {
  auto pModule = new Module();
  assert(pModule);
  module.reset(pModule);
  // turn on global flag
  isLocal = true;
  return visitChildren(ctx);
}

any SysYIRGenerator::visitGlobalDecl(SysYParser::DeclContext *ctx) {
  bool isConst = ctx->CONST();
  Type *type = ctx->btype()->INT() ? Type::getIntType() : Type::getFloatType();
  for (auto varDef : ctx->varDef()) {
    auto lValue = varDef->lValue();
    auto id = lValue->ID()->getText();
    vector<Value *> dims;
    for (auto exp : lValue->exp()) {
      auto res = exp->accept(this);
      assert(res.has_value());
      auto dim = any_cast<Value *>(res);
      // dynamic_cast<C>(expression)
    //   dims.push_back();
    }
  }
  return visitChildren(ctx);
}
} // namespace sysy