#pragma once

#include "IR.h"
#include <cassert>
#include <memory>

namespace sysy {

class IRBuilder {
private:
  BasicBlock *block;
  BasicBlock::iterator position;

public:
  IRBuilder(BasicBlock *block) : block(block), position(block->end()) {}
  IRBuilder(BasicBlock *block, BasicBlock::iterator position)
      : block(block), position(position) {}

public:
  BasicBlock *getBasicBlock() const { return block; }
  BasicBlock::iterator getPosition() const { return position; }
  void setPosition(BasicBlock::iterator position) { this->position = position; }

public:
  CallInst *createCallInst(Function *callee,
                           const std::vector<Value *> args = {},
                           const std::string &name = "") {
    auto inst = new CallInst(callee, args, block, name);
    assert(inst);
    block->getInstructions().emplace(position, inst);
    return inst;
  }
  UnaryInst *createUnaryInst(Instruction::Kind kind, Type *type, Value *operand,
                             const std::string &name = "") {

    auto inst = new UnaryInst(kind, type, operand, block, name);
    assert(inst);
    block->getInstructions().emplace(position, inst);
    return inst;
  }
  BinaryInst *createBinaryInst(Instruction::Kind kind, Type *type, Value *lhs,
                               Value *rhs, const std::string &name = "") {
    auto inst = new BinaryInst(kind, type, lhs, rhs, block, name);
    assert(inst);
    block->getInstructions().emplace(position, inst);
    return inst;
  }
  ReturnInst *createReturnInst(Value *value = nullptr) {
    auto inst = new ReturnInst(value);
    assert(inst);
    block->getInstructions().emplace(position, inst);
    return inst;
  }
  UncondBrInst *createUncondBrInst(BasicBlock *block,
                                   std::vector<Value *> args) {
    auto inst = new UncondBrInst(block, args, block);
    assert(inst);
    block->getInstructions().emplace(position, inst);
    return inst;
  }
  CondBrInst *createCondBrInst(Value *condition, BasicBlock *thenBlock,
                               BasicBlock *elseBlock,
                               const std::vector<Value *> &thenArgs,
                               const std::vector<Value *> &elseArgs) {
    auto inst = new CondBrInst(condition, thenBlock, elseBlock, thenArgs,
                               elseArgs, block);
    assert(inst);
    block->getInstructions().emplace(position, inst);
    return inst;
  }
  AllocaInst *createAllocaInst(Type *type,
                               const std::vector<Value *> &dims = {},
                               const std::string &name = "") {
    auto inst = new AllocaInst(type, dims, block, name);
    assert(inst);
    block->getInstructions().emplace(position, inst);
    return inst;
  }
  LoadInst *createLoadInst(Value *pointer,
                           const std::vector<Value *> &indices = {},
                           const std::string &name = "") {
    auto inst = new LoadInst(pointer, indices, block, name);
    assert(inst);
    block->getInstructions().emplace(position, inst);
    return inst;
  }
  StoreInst *createStoreInst(Value *value, Value *pointer,
                             const std::vector<Value *> &indices = {},
                             const std::string &name = "") {
    auto inst = new StoreInst(value, pointer, indices, block, name);
    assert(inst);
    block->getInstructions().emplace(position, inst);
    return inst;
  }
};

} // namespace sysy