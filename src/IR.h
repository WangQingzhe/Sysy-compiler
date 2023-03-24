#pragma once

#include <cassert>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace sysy {

template <typename IterT> struct range {
  using iterator = IterT;
  using value_type = typename std::iterator_traits<iterator>::value_type;
  using reference = typename std::iterator_traits<iterator>::reference;

  iterator b;
  iterator e;
  explicit range(iterator b, iterator e) : b(b), e(e) {}
  iterator begin() { return b; }
  iterator end() { return e; }
};

template <typename IterT> range<IterT> make_range(IterT b, IterT e) {
  return range(b, e);
}

//===----------------------------------------------------------------------===//
// Types
//
// The SysY type system is quite simple.
// 1. The base class `Type` is used to represent all primitive scalar types,
// include `int`, `float`, `void`, and the label type representing branch
// targets.
// 2. `PointerType` and `FunctionType` derive from `Type` and represent pointer
// type and function type, respectively.
//===----------------------------------------------------------------------===//

class Type {
public:
  enum Kind {
    kInt,
    kFloat,
    kVoid,
    kLabel,
    kPointer,
    kFunction,
  };
  Kind kind;

protected:
  Type(Kind kind) : kind(kind) {}
  virtual ~Type() {}

public:
  static Type *getIntType();
  static Type *getFloatType();
  static Type *getVoidType();
  static Type *getLabelType();
  static Type *getPointerType(Type *baseType);
  static Type *getFunctionType(Type *returnType,
                               const std::vector<Type *> &paramTypes = {});
  static int getTypeSize();

public:
  Kind getKind() const { return kind; }
  bool isInt() const { return kind == kInt; }
  bool isFloat() const { return kind == kFloat; }
  bool isVoid() const { return kind == kVoid; }
  bool isLabel() const { return kind == kLabel; }
  bool isPointer() const { return kind == kPointer; }
  bool isFunction() const { return kind == kFunction; }
}; // class Type

class PointerType : public Type {
protected:
  Type *baseType;

protected:
  PointerType(Type *baseType) : Type(kPointer), baseType(baseType) {}

public:
  static PointerType *get(Type *baseType);

public:
  Type *getBaseType() const { return baseType; }
}; // class PointerType

class FunctionType : public Type {
private:
  Type *returnType;
  std::vector<Type *> paramTypes;

protected:
  FunctionType(Type *returnType, const std::vector<Type *> &paramTypes = {})
      : Type(kFunction), returnType(returnType), paramTypes(paramTypes) {}

public:
  static FunctionType *get(Type *returnType,
                           const std::vector<Type *> &paramTypes = {});

public:
  Type *getReturnType() const { return returnType; }
  const std::vector<Type *> &getParamTypes() const { return paramTypes; }
  int getNumParams() const { return paramTypes.size(); }
}; // class FunctionType

//===----------------------------------------------------------------------===//
// Values
//
// description
//===----------------------------------------------------------------------===//

class User;
class Value;

class Use {
public:
  enum Kind {
    kRead,
    kWrite,
    kReadWrite,
  };

private:
  Kind kind;
  int index;
  User *user;
  Value *value;

public:
  Use() = default;
  Use(Kind kind, int index, User *user, Value *value)
      : kind(kind), index(index), user(user), value(value) {}

public:
  Kind getKind() const { return kind; }
  int getIndex() const { return index; }
  User *getUser() const { return user; }
  Value *getValue() const { return value; }
  bool isRead() const { return kind == kRead; }
  bool isWrite() const { return kind == kWrite; }
  bool isReadWrite() const { return kind == kReadWrite; }
  void setValue(Value *value) { value = value; }
}; // class Use

class Value {
protected:
  Type *type;
  std::string name;
  std::list<Use *> uses;

protected:
  Value(Type *type, const std::string &name = "")
      : type(type), name(name), uses() {}
  virtual ~Value() {}

public:
  Type *getType() const { return type; }
  bool isInt() const { return type->isInt(); }
  bool isFloat() const { return type->isFloat(); }
  bool isPointer() const { return type->isPointer(); }
  const std::list<Use *> &getUses() { return uses; }
  void addUse(Use *use) { uses.push_back(use); }
  void replaceAllUsesWith(Value *value);
  void removeUse(Use *use) { uses.remove(use); }
}; // class Value

class BasicBlock;
class Argument : public Value {
protected:
  BasicBlock *block;
  int index;

protected:
  Argument(Type *type, BasicBlock *block, int index,
           const std::string &name = "")
      : Value(type, name), block(block), index(index) {}
};

class Instruction;
class Function;
class BasicBlock : public Value {
  friend class Function;

public:
  using inst_list = std::list<std::unique_ptr<Instruction>>;
  using iterator = inst_list::iterator;
  using arg_list = std::vector<Argument>;
  using block_list = std::vector<BasicBlock *>;

protected:
  Function *parent;
  inst_list instructions;
  arg_list arguments;
  block_list successors;
  block_list predecessors;

protected:
  explicit BasicBlock(Function *parent, const std::string &name = "")
      : Value(Type::getLabelType(), name), parent(parent), instructions(),
        arguments(), successors(), predecessors() {}

public:
  int getNumArguments() const { return arguments.size(); }
  int getNumPredecessors() const { return predecessors.size(); }
  int getNumSuccessors() const { return successors.size(); }
  Function *getParent() const { return parent; }
  inst_list &getInstructions() { return instructions; }
  arg_list &getArguments() { return arguments; }
  block_list &getPredecessors() { return predecessors; }
  block_list &getSuccessors() { return successors; }
  iterator begin() { return instructions.begin(); }
  iterator end() { return instructions.end(); }
}; // class BasicBlock

class User : public Value {
protected:
  std::vector<Use> operands;

protected:
  User(Type *type, const std::string &name = "")
      : Value(type, name), operands() {}

public:
  int getNumOperands() const { return operands.size(); }
  const std::vector<Use> &getOperands() const { return operands; }
  const Use &getOperand(int index) const { return operands[index]; }
  void addOperand(Value *value, Use::Kind mode = Use::kRead) {
    operands.emplace_back(mode, operands.size(), this, value);
    value->addUse(&operands.back());
  }
  void addOperands(const std::vector<Value *> &operands) {
    for (auto value : operands)
      addOperand(value);
  }
  void replaceOperand(int index, Value *value);
  void setOperand(int index, Value *value);
  const std::string &getName() const { return name; }
}; // class User

// class Constant : public User {
// protected:
//   union scalar {
//     int iConstant;
//     float fConstant;
//   };
//   std::vector<int> dims;
//   std::vector<scalar> data;

// protected:
//   Constant(Type *type, const std::string &name = "") : User(type, name) {}

// public:
//   int getInt() const {
//     assert(isInt());
//     return iConstant;
//   }
//   float getFloat() const {
//     assert(isFloat());
//     return fConstant;
//   }
// }; // class ConstantInst

class Instruction : public User {
public:
  enum Kind : uint64_t {
    kInvalid = 0x0UL,
    // Binary
    kAdd = 0x1UL << 0,
    kSub = 0x1UL << 1,
    kMul = 0x1UL << 2,
    kSDiv = 0x1UL << 3,
    kSRem = 0x1UL << 4,
    kICmpEQ = 0x1UL << 5,
    kICmpNE = 0x1UL << 6,
    kICmpLT = 0x1UL << 7,
    kICmpGT = 0x1UL << 8,
    kICmpLE = 0x1UL << 9,
    kICmpGE = 0x1UL << 10,
    kAShr = 0x1UL << 11,
    kLShr = 0x1UL << 12,
    kShl = 0x1UL << 13,
    kFAdd = 0x1UL << 14,
    kFSub = 0x1UL << 15,
    kFMul = 0x1UL << 16,
    kFDiv = 0x1UL << 17,
    kFRem = 0x1UL << 18,
    kFCmpEQ = 0x1UL << 19,
    kFCmpNE = 0x1UL << 20,
    kFCmpLT = 0x1UL << 21,
    kFCmpGT = 0x1UL << 22,
    kFCmpLE = 0x1UL << 23,
    kFCmpGE = 0x1UL << 24,
    // Unary
    kNeg = 0x1UL << 25,
    kNot = 0x1UL << 26,
    kFNeg = 0x1UL << 26,
    kFtoI = 0x1UL << 28,
    kIToF = 0x1UL << 29,
    kBitCast = 0x1UL << 30,
    // call
    kCall = 0x1UL << 33,
    // terminator
    kCondBr = 0x1UL << 31,
    kBr = 0x1UL << 32,
    kReturn = 0x1UL << 34,
    // mem op
    kAlloca = 0x1UL << 35,
    kLoad = 0x1UL << 36,
    kStore = 0x1UL << 37,
    // constant
    // kConstant = 0x1UL << 38,
  };

protected:
  Kind kind;
  BasicBlock *parent;

protected:
  Instruction(Kind kind, Type *type, BasicBlock *parent = nullptr,
              const std::string &name = "")
      : User(type, name), kind(kind), parent(parent) {}

public:
  Kind getKind() const { return kind; }
  BasicBlock *getParent() const { return parent; }
  void setParent(BasicBlock *bb) { parent = bb; }

  bool isInteger() const {
    static constexpr uint64_t IntegerOpMask =
        kAdd | kSub | kMul | kSDiv | kSRem | kICmpEQ | kICmpNE | kICmpLT |
        kICmpGT | kICmpLE | kICmpGE | kAShr | kLShr | kShl | kNeg | kNot |
        kIToF;
    return kind & IntegerOpMask;
  }
  bool isCmp() const {
    static constexpr uint64_t CondOpMask =
        kICmpEQ | kICmpNE | kICmpLT | kICmpGT | kICmpLE | kICmpGE | kFCmpEQ |
        kFCmpNE | kFCmpLT | kFCmpGT | kFCmpLE | kFCmpGE;
    return kind & CondOpMask;
  }
  bool isTerminator() {
    static constexpr uint64_t TerminatorOpMask = kCondBr | kBr | kReturn;
    return kind & TerminatorOpMask;
  }
  bool isCommutative() {
    static constexpr uint64_t CommutativeOpMask =
        kAdd | kMul | kICmpEQ | kICmpNE | kFAdd | kFMul | kFCmpEQ | kFCmpNE;
    return kind & CommutativeOpMask;
  }
}; // class Instruction

class Function;
class CallInst : public Instruction {
  friend class IRBuilder;

protected:
  CallInst(Function *callee, const std::vector<Value *> args = {},
           BasicBlock *parent = nullptr, const std::string &name = "");

public:
  Function *getCallee();
  auto getArguments() {
    return make_range(std::next(operands.begin()), operands.end());
  }
}; // class CallInst

class UnaryInst : public Instruction {
  friend class IRBuilder;

protected:
  UnaryInst(Kind kind, Type *type, Value *operand, BasicBlock *parent = nullptr,
            const std::string &name = "")
      : Instruction(kind, type, parent, name) {
    addOperand(operand);
  }

public:
  Value *getOperand() const { return operands[0].getValue(); }
}; // class UnaryInst

class BinaryInst : public Instruction {
  friend class IRBuilder;

protected:
  BinaryInst(Kind kind, Type *type, Value *lhs, Value *rhs, BasicBlock *parent,
             const std::string &name = "")
      : Instruction(kind, type, parent, name) {
    addOperand(lhs);
    addOperand(rhs);
  }

public:
  Value *getLhs() const { return operands[0].getValue(); }
  Value *getRhs() const { return operands[1].getValue(); }
}; // class BinaryInst

class TerminatorInst : public Instruction {
protected:
  using Instruction::Instruction;
}; // TerminatorInst

class ReturnInst : public TerminatorInst {
  friend class IRBuilder;

protected:
  ReturnInst(Value *value = nullptr, BasicBlock *parent = nullptr)
      : TerminatorInst(kReturn, Type::getVoidType(), parent, "") {
    if (value)
      addOperand(value);
  }
}; // class ReturnInst

class BranchInst : public TerminatorInst {
protected:
  using TerminatorInst::TerminatorInst;

public:
  bool isUnconditional() const { return kind == kBr; }
  bool isConditional() const { return kind == kCondBr; }
}; // class BranchInst

class UncondBrInst : public BranchInst {
  friend class IRBuilder;

protected:
  UncondBrInst(BasicBlock *block, std::vector<Value *> args,
               BasicBlock *parent = nullptr)
      : BranchInst(kCondBr, Type::getVoidType(), parent, "") {
    assert(block->getNumArguments() == args.size());
    addOperand(block);
    addOperands(args);
  }

public:
  BasicBlock *getBlock() const {
    return dynamic_cast<BasicBlock *>(getOperand(0).getValue());
  }
  auto getArguments() const {
    return make_range(std::next(operands.begin()), operands.end());
  }
}; // class UncondBrInst

class CondBrInst : public BranchInst {
  friend class IRBuilder;

protected:
  CondBrInst(Value *condition, BasicBlock *thenBlock, BasicBlock *elseBlock,
             const std::vector<Value *> &thenArgs,
             const std::vector<Value *> &elseArgs, BasicBlock *parent = nullptr)
      : BranchInst(kCondBr, Type::getVoidType(), parent, "") {
    assert(thenBlock->getNumArguments() == thenArgs.size() and
           elseBlock->getNumArguments() == elseArgs.size());
    addOperand(condition);
    addOperand(thenBlock);
    addOperand(elseBlock);
    addOperands(thenArgs);
    addOperands(elseArgs);
  }

public:
  BasicBlock *getThenBlock() const {
    return dynamic_cast<BasicBlock *>(getOperand(0).getValue());
  }
  BasicBlock *getElseBlock() const {
    return dynamic_cast<BasicBlock *>(getOperand(1).getValue());
  }
  auto getThenArguments() const {
    auto begin = operands.begin() + 2;
    auto end = begin + getThenBlock()->getNumArguments();
    return make_range(begin, end);
  }
  auto getElseArguments() const {
    auto begin = operands.begin() + 2 + getThenBlock()->getNumArguments();
    auto end = operands.end();
    return make_range(begin, end);
  }
}; // class CondBrInst

class MemoryInst : public Instruction {
protected:
  using Instruction::Instruction;
}; // class MemoryInst

class AllocaInst : public MemoryInst {
  friend class IRBuilder;

protected:
  AllocaInst(Type *type, const std::vector<Value *> &dims = {},
             BasicBlock *parent = nullptr, const std::string &name = "")
      : MemoryInst(kAlloca, type, parent, name) {
    addOperands(dims);
  }

public:
  int getNumDims() const { return getNumOperands(); }
  auto &getDims() const { return operands; }
  Value *getDim(int index) { return getOperand(index).getValue(); }
}; // class AllocaInst

class LoadInst : public MemoryInst {
  friend class IRBuilder;

protected:
  LoadInst(Value *pointer, const std::vector<Value *> &indices = {},
           BasicBlock *parent = nullptr, const std::string &name = "")
      : MemoryInst(
            kLoad,
            dynamic_cast<PointerType *>(pointer->getType())->getBaseType(),
            parent, name) {
    addOperands(indices);
  }

public:
  int getNumIndices() const { return getNumOperands() - 1; }
  Value *getPointer() const { return operands.front().getValue(); }
  auto getIndices() const {
    return make_range(std::next(operands.begin()), operands.end());
  }
  Value *getIndex(int index) const { return getOperand(index + 1).getValue(); }
}; // class LoadInst

class StoreInst : public MemoryInst {
  friend class IRBuilder;

protected:
  StoreInst(Value *value, Value *pointer,
            const std::vector<Value *> &indices = {},
            BasicBlock *parent = nullptr, const std::string &name = "")
      : MemoryInst(kStore, Type::getVoidType(), parent, name) {
    addOperand(value);
    addOperand(pointer);
    addOperands(indices);
  }

public:
  int getNumIndices() const { return getNumOperands() - 2; }
  Value *getValue() const { return operands[0].getValue(); }
  Value *getPointer() const { return operands[1].getValue(); }
  auto getIndices() const {
    return make_range(operands.begin() + 2, operands.end());
  }
  Value *getIndex(int index) const { return getOperand(index + 2).getValue(); }
}; // class StoreInst

class Function : public Value {
  friend class Module;

protected:
  Function(Type *type, const std::string &name) : Value(type, name) {
    blocks.emplace_back(new BasicBlock(this, "entry"));
  }

public:
  using block_list = std::list<std::unique_ptr<BasicBlock>>;

protected:
  block_list blocks;

public:
  Type *getReturnType() const {
    return dynamic_cast<FunctionType *>(getType())->getReturnType();
  }
  auto &getParamTypes() const {
    return dynamic_cast<FunctionType *>(getType())->getParamTypes();
  }
  block_list &getBasicBlocks() { return blocks; }
  BasicBlock *getEntryBlock() { return blocks.front().get(); }
  BasicBlock *addBasicBlock(const std::string &name = "") {
    blocks.emplace_back(new BasicBlock(this, name));
    return blocks.back().get();
  }
  void removeBasicBlock(BasicBlock *block) {
    blocks.remove_if([&](std::unique_ptr<BasicBlock> &b) -> bool {
      return block == b.get();
    });
  }
}; // class Function

class GlobalValue : public User {
  friend class Module;

protected:
  GlobalValue(Type *type, const std::vector<Value *> &dims = {},
              const std::string &name = "")
      : User(type, name) {
    addOperands(dims);
  }

public:
  int getNumDims() const { return getNumOperands(); }
  Value *getDim(int index) { return getOperand(index).getValue(); }
}; // class GlobalValue

class Module {
protected:
  std::map<std::string, std::unique_ptr<Function>> functions;
  std::map<std::string, std::unique_ptr<GlobalValue>> globals;
  // std::list<std::unique_ptr<Function>> functions;
  // std::list<std::unique_ptr<GlobalValue>> globals;

public:
  Module() = default;

public:
  Function *createFunction(const std::string &name, Type *type) {
    auto result = functions.try_emplace(name, new Function(type, name));
    if (not result.second)
      return nullptr;
    return result.first->second.get();
  };
  GlobalValue *createGlobalValue(const std::string &name, Type *type,
                                 const std::vector<Value *> &dims = {}) {
    auto result = globals.try_emplace(name, new GlobalValue(type, dims, name));
    if (not result.second)
      return nullptr;
    return result.first->second.get();
  }
  Function *getFunction(const std::string &name) const {
    auto result = functions.find(name);
    if (result == functions.end())
      return nullptr;
    return result->second.get();
  }
  GlobalValue *getGlobalValue(const std::string &name) const {
    auto result = globals.find(name);
    if (result == globals.end())
      return nullptr;
    return result->second.get();
  }
}; // class Module

inline CallInst::CallInst(Function *callee, const std::vector<Value *> args,
                          BasicBlock *parent, const std::string &name)
    : Instruction(kCall, callee->getReturnType(), parent, name) {
  addOperand(callee);
  for (auto arg : args)
    addOperand(arg);
}

inline Function *CallInst::getCallee() {
  return dynamic_cast<Function *>(getOperand(0).getValue());
}

} // namespace sysy