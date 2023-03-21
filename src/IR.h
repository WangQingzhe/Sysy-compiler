#pragma once

#include <cassert>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <list>
#include <memory>
#include <string>
#include <vector>

namespace sysy {

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
  static Type *getPointerType();
  static Type *getLabelType();
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
private:
  Type *baseType;

protected:
  PointerType(Type *baseType) : Type(kPointer), baseType(baseType) {}

public:
  Type *get(Type *baseType);

public:
  Type *getBaseType() const { return baseType; }
}; // class PointerType

class FunctionType : public Type {
private:
  Type *returnType;
  std::vector<Type *> paramTypes;

protected:
  FunctionType(Type *returnType) : Type(kFunction), returnType(returnType) {}
  FunctionType(Type *returnType, const std::vector<Type *> paramTypes = {})
      : Type(kFunction), returnType(returnType), paramTypes(paramTypes) {}

public:
  Type *get(Type *baseType, const std::vector<Type *> paramTypes = {});

public:
  Type *getReturnType() const { return returnType; }
  const std::vector<Type *> &getParamTypes() const { return paramTypes; }
}; // class FunctionType

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
  std::vector<Use *> uses;

protected:
  Value(Type *type) : type(type), uses() {}
  virtual ~Value() {}

public:
  Type *getType() const { return type; }
  bool isInt() const { return type->isInt(); }
  bool isFloat() const { return type->isFloat(); }
  bool isPointer() const { return type->isPointer(); }
  const std::vector<Use *> &getUses() { return uses; }
  void addUse(Use *use) { uses.push_back(use); }
  void replaceAllUsesWith(Value *value);
}; // class Value

class User : public Value {
protected:
  std::vector<Use> operands;
  std::string name;

protected:
  User(Type *type, const std::vector<Value *> &operands = {},
       const std::string &name = "")
      : Value(type), operands(), name(name) {
    for (auto op : operands)
      addOperand(op);
  }

public:
  struct OperandIterator : public std::vector<Use>::const_iterator {
    OperandIterator(const std::vector<Use>::const_iterator &iter)
        : std::vector<Use>::const_iterator(iter) {}
    using value_type = Value *;
    value_type operator*() { return operator->()->getValue(); }
  };

  OperandIterator op_begin() const {
    return OperandIterator(operands.begin());
  };
  OperandIterator op_end() const { return OperandIterator(operands.end()); };
  int getNumOperands() const { return operands.size(); }
  const std::vector<Use> &getOperands() const { return operands; }
  Value *getOperand(int index) const { return operands[index].getValue(); }
  void addOperand(Value *value) {
    operands.emplace_back(Use::kRead, operands.size(), this, value);
    value->addUse(&operands.back());
  }
  void replaceOperand(int index, Value *value);
  const std::string &getName() const { return name; }

}; // class User

class BasicBlock;
class Argument : public Value {
protected:
  BasicBlock *block;
  int index;

protected:
  Argument(Type *type, BasicBlock *block, int index)
      : Value(type), block(block), index(index) {}
};

class Instruction;
class BasicBlock : public User, public std::list<std::unique_ptr<Instruction>> {
public:
  using arg_list = std::vector<Argument *>;
  using arg_iterator = arg_list::iterator;
  using block_list = std::vector<BasicBlock *>;
  using block_iterator = block_list::iterator;

protected:
  arg_list arguments;
  block_list successors;
  block_list predecessors;

protected:
  BasicBlock(const std::string &name = "")
      : User(Type::getLabelType(), {}, name),
        std::list<std::unique_ptr<Instruction>>() {}

public:
  arg_iterator arg_begin() { return arguments.begin(); }
  arg_iterator arg_end() { return arguments.end(); }
  block_iterator pred_begin() { return predecessors.begin(); }
  block_iterator pred_end() { return predecessors.end(); }
  block_iterator succ_begin() { return successors.begin(); }
  block_iterator succ_end() { return successors.end(); }
  int getNumArguments() const { return arguments.size(); }
  int getNumPredecessors() const { return predecessors.size(); }
  int getNumSuccessors() const { return successors.size(); }
  const arg_list &getArguments() const { return arguments; }
  const block_list &getPredecessors() const { return predecessors; }
  const block_list &getSuccessors() const { return successors; }
  Argument *getArgument(int index) const { return arguments[index]; }
  BasicBlock *getPredecessor(int index) const { return predecessors[index]; }
  BasicBlock *getSuccessor(int index) const { return successors[index]; }
}; // class BasicBlock

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
    kConstant = 0x1UL << 38,
  };

protected:
  Kind kind;
  BasicBlock *parent;

protected:
  Instruction(Kind kind, Type *type, const std::vector<Value *> &oprands = {},
              BasicBlock *parent = nullptr, const std::string &name = "")
      : User(type, oprands, name), kind(kind), parent(parent) {}

public:
  Kind getKind() const { return kind; }
  BasicBlock *getBasicBlock() const { return parent; }
  void setBasicBlock(BasicBlock *bb) { parent = bb; }

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

  // static bool isReverse(Instruction *x, Instruction *y);
  // int getRank();
}; // class Instruction

class ConstantInst : public Instruction {
protected:
  union {
    int iConstant;
    float fConstant;
  };

protected:
  ConstantInst(Type *type, BasicBlock *parent = nullptr,
               const std::string &name = "")
      : Instruction(kConstant, type, {}, parent, name) {}

public:
  int getInt() const {
    assert(isInt());
    return iConstant;
  }
  float getFloat() const {
    assert(isFloat());
    return fConstant;
  }
}; // class ConstantInst

class Function;
class CallInst : public Instruction {
protected:
  CallInst(Function *callee, const std::vector<Value *> args = {},
           BasicBlock *parent = nullptr, const std::string &name = "");
  //     : Instruction(kCall, callee->getReturnType(), {}, parent,
  //     name) {}

public:
  Function *getCallee();
  OperandIterator arg_begin() const { return std::next(op_begin()); }
  OperandIterator arg_end() const { return op_end(); }
}; // class CallInst

class UnaryInst : public Instruction {
protected:
  UnaryInst(Kind kind, Type *type, Value *operand, BasicBlock *parent,
            const std::string &name = "")
      : Instruction(kind, type, {}, parent, name) {
    addOperand(operand);
  }

public:
  Value *getOperand() const { return *op_begin(); }
}; // class UnaryInst

class BinaryInst : public Instruction {
protected:
  BinaryInst(Kind kind, Type *type, Value *lhs, Value *rhs, BasicBlock *parent,
             const std::string &name = "")
      : Instruction(kind, type, {}, parent, name) {
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
protected:
  ReturnInst(const std::vector<Value *> &values = {},
             BasicBlock *parent = nullptr)
      : TerminatorInst(kReturn, Type::getVoidType(), values, parent, "") {}
}; // class ReturnInst

class BranchInst : public TerminatorInst {
protected:
  using TerminatorInst::TerminatorInst;

public:
  bool isUnconditional() const { return kind == kBr; }
  bool isConditional() const { return kind == kCondBr; }
}; // class BranchInst

class CondBrInst : public BranchInst {
protected:
  CondBrInst(Value *condition, BasicBlock *trueBlock, BasicBlock *falseBlock,
             BasicBlock *parent = nullptr)
      : BranchInst(kCondBr, Type::getVoidType(), {}, parent, "") {
    addOperand(condition);
    addOperand(trueBlock);
    addOperand(falseBlock);
  }
}; // class CondBrInst

class MemoryInst : public Instruction {
protected:
  using Instruction::Instruction;
}; // class MemoryInst

class AllocaInst : public MemoryInst {
protected:
  AllocaInst(Type *type, const std::vector<Value *> &dims = {},
             BasicBlock *parent = nullptr, const std::string &name = "")
      : MemoryInst(kAlloca, type, dims, parent, name) {}

public:
  int getNumDims() const { return getNumOperands(); }
  Value *getDim(int index) { return getOperand(index); }
}; // class AllocaInst

class LoadInst : public MemoryInst {
protected:
  LoadInst(Value *pointer, const std::vector<Value *> &indices = {},
           BasicBlock *parent = nullptr, const std::string &name = "")
      : MemoryInst(
            kLoad,
            dynamic_cast<PointerType *>(pointer->getType())->getBaseType(),
            indices, parent, name) {}

public:
  Value *getPointer() const { return operands.front().getValue(); }
  int getNumIndices() const { return getNumOperands() - 1; }
  Value *getIndex(int index) const { return getOperand(index + 1); }
}; // class LoadInst

class StoreInst : public MemoryInst {
protected:
  StoreInst(Value *value, Value *pointer,
            const std::vector<Value *> &indices = {},
            BasicBlock *parent = nullptr, const std::string &name = "")
      : MemoryInst(kStore, Type::getVoidType(), {}, parent, name) {
    addOperand(value);
    addOperand(pointer);
    for (auto index : indices)
      addOperand(index);
  }

public:
  Value *getValue() const { return operands[0].getValue(); }
  Value *getPointer() const { return operands[1].getValue(); }
  int getNumIndices() const { return getNumOperands() - 2; }
  Value *getIndex(int index) const { return getOperand(index + 2); }
}; // class StoreInst

class Function : public Value, public std::list<std::unique_ptr<BasicBlock>> {
protected:
  Function(Type *type)
      : Value(type), std::list<std::unique_ptr<BasicBlock>>() {}
}; // class Function

class GlobalValue : public User {
protected:
  GlobalValue(Type *type, const std::vector<Value *> &dims = {},
              const std::string &name = "")
      : User(type, dims, name) {}
public:
  int getNumDims() const { return getNumOperands(); }
  Value *getDim(int index) { return getOperand(index); }
}; // class GlobalValue

class Module {
protected:
  std::list<Function> functions;
  std::list<GlobalValue> globals;
}; // class Module

} // namespace sysy