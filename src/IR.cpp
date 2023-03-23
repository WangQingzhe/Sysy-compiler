#include "IR.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <map>
#include <set>
#include <memory>
#include <vector>

namespace sysy {

//===----------------------------------------------------------------------===//
// Types
//===----------------------------------------------------------------------===//

Type *Type::getIntType() {
  static Type intType(kInt);
  return &intType;
}

Type *Type::getFloatType() {
  static Type floatType(kFloat);
  return &floatType;
}

Type *Type::getVoidType() {
  static Type voidType(kVoid);
  return &voidType;
}

Type *Type::getLabelType() {
  static Type labelType(kLabel);
  return &labelType;
}

Type *Type::getPointerType(Type *baseType) {
  // forward to PointerType
  return PointerType::get(baseType);
}

Type *Type::getFunctionType(Type *returnType,
                            const std::vector<Type *> &paramTypes) {
  // forward to FunctionType
  return FunctionType::get(returnType, paramTypes);
}

PointerType *PointerType::get(Type *baseType) {
  static std::map<Type *, std::unique_ptr<PointerType>> pointerTypes;
  auto type = new PointerType(baseType);
  assert(type);
  auto result = pointerTypes.try_emplace(baseType, type);
  return result.first->second.get();
}

bool operator<(const FunctionType &lhs, const FunctionType &rhs) {
  return lhs.getReturnType() < rhs.getReturnType() or
         lhs.getParamTypes().size() < rhs.getParamTypes().size() and
             std::lexicographical_compare(lhs.getParamTypes().begin(),
                                          lhs.getParamTypes().end(),
                                          rhs.getParamTypes().begin(),
                                          rhs.getParamTypes().end());
}

FunctionType *FunctionType::get(Type *returnType,
                                const std::vector<Type *> &paramTypes) {
  static std::set<std::unique_ptr<FunctionType>> functionTypes;
  auto type = new FunctionType(returnType, paramTypes);
  assert(type);
  auto result = functionTypes.emplace(type);
  return result.first->get();
}

void Value::replaceAllUsesWith(Value *value) {
  for (auto &use : uses)
    use->getUser()->setOperand(use->getIndex(), value);
  uses.clear();
}

void User::setOperand(int index, Value *value) {
  assert(index < getNumOperands());
  operands[index].setValue(value);
}

void User::replaceOperand(int index, Value *value) {
  assert(index < getNumOperands());
  auto &use = operands[index];
  use.getValue()->removeUse(&use);
  use.setValue(value);
}

} // namespace sysy