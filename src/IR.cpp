#include "IR.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <map>
#include <memory>
#include <unordered_set>
#include <vector>

namespace std {

//===----------------------------------------------------------------------===//
// Types
//===----------------------------------------------------------------------===//

template <> struct hash<sysy::FunctionType> {
  size_t operator()(const sysy::FunctionType &type) {
    auto num = 1 + type.getNumParams();
    vector<sysy::Type *> buffer(1, type.getReturnType());
    copy(type.getParamTypes().begin(), type.getParamTypes().end(),
         back_inserter(buffer));
    string bytes(reinterpret_cast<char *>(buffer.data()),
                 buffer.size() * sizeof(sysy::Type *));
    hash<string> hasher;
    return hasher(bytes);
  }
};

} // namespace std

namespace sysy {

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
  static std::map<Type *, PointerType> pointerTypes;
  auto result = pointerTypes.try_emplace(baseType, baseType);
  return &(result.first->second);
}

FunctionType *FunctionType::get(Type *returnType,
                                const std::vector<Type *> &paramTypes) {
  static std::unordered_set<FunctionType> functionTypes;
  auto result = functionTypes.emplace(returnType, paramTypes);
  // STL set objects are immutable, sothe emplace method returns const_iterator.
  // We are sure FunctionType pointers cannot be used to mutate the underlying
  // FunctionType object because all FunctionType methods are const methods.
  return const_cast<FunctionType *>(&(*result.first));
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