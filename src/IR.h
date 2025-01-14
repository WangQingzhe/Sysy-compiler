#pragma once

#include "range.h"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <type_traits>
#include <vector>

namespace sysy
{

  /*!
   * \defgroup type Types
   * The SysY type system is quite simple.
   * 1. The base class `Type` is used to represent all primitive scalar types,
   * include `int`, `float`, `void`, and the label type representing branch
   * targets.
   * 2. `PointerType` and `FunctionType` derive from `Type` and represent pointer
   * type and function type, respectively.
   *
   * NOTE `Type` and its derived classes have their ctors declared as 'protected'.
   * Users must use Type::getXXXType() methods to obtain `Type` pointers.
   * @{
   */

  /*!
   * `Type` is used to represent all primitive scalar types,
   * include `int`, `float`, `void`, and the label type representing branch
   * targets
   */
  class Type
  {
  public:
    enum Kind
    {
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
    virtual ~Type() = default;

  public:
    static Type *getIntType();
    static Type *getFloatType();
    static Type *getVoidType();
    static Type *getLabelType();
    static Type *getPointerType(Type *baseType);
    static Type *getFunctionType(Type *returnType,
                                 const std::vector<Type *> &paramTypes = {});

  public:
    Kind getKind() const { return kind; }
    bool isInt() const { return kind == kInt; }
    bool isFloat() const { return kind == kFloat; }
    bool isVoid() const { return kind == kVoid; }
    bool isLabel() const { return kind == kLabel; }
    bool isPointer() const { return kind == kPointer; }
    bool isFunction() const { return kind == kFunction; }
    bool isIntOrFloat() const { return kind == kInt or kind == kFloat; }
    int getSize() const;
    template <typename T>
    std::enable_if_t<std::is_base_of_v<Type, T>, T *> as() const
    {
      return dynamic_cast<T *>(const_cast<Type *>(this));
    }
    void print(std::ostream &os) const;
  }; // class Type

  //! Pointer type
  class PointerType : public Type
  {
  protected:
    Type *baseType;

  protected:
    PointerType(Type *baseType) : Type(kPointer), baseType(baseType) {}

  public:
    static PointerType *get(Type *baseType);

  public:
    Type *getBaseType() const { return baseType; }
  }; // class PointerType

  //! Function type
  class FunctionType : public Type
  {
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
    auto getParamTypes() const { return make_range(paramTypes); }
    int getNumParams() const { return paramTypes.size(); }
  }; // class FunctionType

  /*!
   * @}
   */

  /*!
   * \defgroup ir IR
   *
   * The SysY IR is an instruction level language. The IR is orgnized
   * as a four-level tree structure, as shown below
   *
   * \dotfile ir-4level.dot IR Structure
   *
   * - `Module` corresponds to the top level "CompUnit" syntax structure
   * - `GlobalValue` corresponds to the "Decl" syntax structure
   * - `Function` corresponds to the "FuncDef" syntax structure
   * - `BasicBlock` is a sequence of instructions without branching. A `Function`
   *   made up by one or more `BasicBlock`s.
   * - `Instruction` represents a primitive operation on values, e.g., add or sub.
   *
   * The fundamental data concept in SysY IR is `Value`. A `Value` is like
   * a register and is used by `Instruction`s as input/output operand. Each value
   * has an associated `Type` indicating the data type held by the value.
   *
   * Most `Instruction`s have a three-address signature, i.e., there are at most 2
   * input values and at most 1 output value.
   *
   * The SysY IR adots a Static-Single-Assignment (SSA) design. That is, `Value`
   * is defined (as the output operand ) by some instruction, and used (as the
   * input operand) by other instructions. While a value can be used by multiple
   * instructions, the `definition` occurs only once. As a result, there is a
   * one-to-one relation between a value and the instruction defining it. In other
   * words, any instruction defines a value can be viewed as the defined value
   * itself. So `Instruction` is also a `Value` in SysY IR. See `Value` for the
   * type hierachy.
   *
   * @{
   */

  class User;
  class Value;

  //! `Use` represents the relation between a `Value` and its `User`
  class Use
  {
  private:
    //! the position of value in the user's operands, i.e.,
    //! user->getOperands[index] == value
    int index;
    User *user;
    Value *value;

  public:
    Use() = default;
    Use(int index, User *user, Value *value)
        : index(index), user(user), value(value) {}

  public:
    int getIndex() const { return index; }
    User *getUser() const { return user; }
    Value *getValue() const { return value; }
    void setValue(Value *value) { value = value; }
  }; // class Use

  template <typename T>
  inline std::enable_if_t<std::is_base_of_v<Value, T>, bool>
  isa(const Value *value)
  {
    return T::classof(value);
  }

  template <typename T>
  inline std::enable_if_t<std::is_base_of_v<Value, T>, T *>
  dyncast(Value *value)
  {
    return isa<T>(value) ? static_cast<T *>(value) : nullptr;
  }

  template <typename T>
  inline std::enable_if_t<std::is_base_of_v<Value, T>, const T *>
  dyncast(const Value *value)
  {
    return isa<T>(value) ? static_cast<const T *>(value) : nullptr;
  }

  //! The base class of all value types
  class Value
  {
  public:
    enum Kind : uint64_t
    {
      kInvalid,
      // Instructions
      // Binary
      kAdd = 0x1UL << 0,
      kSub = 0x1UL << 1,
      kMul = 0x1UL << 2,
      kDiv = 0x1UL << 3,
      kRem = 0x1UL << 4,
      kICmpEQ = 0x1UL << 5,
      kICmpNE = 0x1UL << 6,
      kICmpLT = 0x1UL << 7,
      kICmpGT = 0x1UL << 8,
      kICmpLE = 0x1UL << 9,
      kICmpGE = 0x1UL << 10,
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
      kFNeg = 0x1UL << 27,
      kFtoI = 0x1UL << 28,
      kItoF = 0x1UL << 29,
      // call
      kCall = 0x1UL << 30,
      // terminator
      kCondBr = 0x1UL << 31,
      kBr = 0x1UL << 32,
      kReturn = 0x1UL << 33,
      // mem op
      kAlloca = 0x1UL << 34,
      kLoad = 0x1UL << 35,
      kStore = 0x1UL << 36,
      kFirstInst = kAdd,
      kLastInst = kStore,
      // others
      kArgument = 0x1UL << 37,
      kBasicBlock = 0x1UL << 38,
      kFunction = 0x1UL << 39,
      kConstant = 0x1UL << 40,
      kGlobal = 0x1UL << 41,
    };

  protected:
    Kind kind;
    Type *type;
    std::string name;
    std::list<Use *> uses;

  protected:
    Value(Kind kind, Type *type, const std::string &name = "")
        : kind(kind), type(type), name(name), uses() {}

  public:
    virtual ~Value() = default;

  public:
    Kind getKind() const { return kind; }
    static bool classof(const Value *) { return true; }

  public:
    Type *getType() const { return type; }
    const std::string &getName() const { return name; }
    void setName(const std::string &n) { name = n; }
    bool hasName() const { return not name.empty(); }
    bool isInt() const { return type->isInt(); }
    bool isFloat() const { return type->isFloat(); }
    bool isPointer() const { return type->isPointer(); }
    const std::list<Use *> &getUses() { return uses; }
    void addUse(Use *use) { uses.push_back(use); }
    void replaceAllUsesWith(Value *value);
    void removeUse(Use *use) { uses.remove(use); }
    bool isConstant() const;

  public:
    virtual void print(std::ostream &os) const {};
  }; // class Value

  /*!
   * Static constants known at compile time.
   *
   * `ConstantValue`s are not defined by instructions, and do not use any other
   * `Value`s. It's type is either `int` or `float`.
   */
  class ConstantValue : public Value
  {
  protected:
    union
    {
      int iScalar;
      float fScalar;
      double dScalar;
    };

  protected:
    ConstantValue(int value)
        : Value(kConstant, Type::getIntType(), ""), iScalar(value) {}
    ConstantValue(float value)
        : Value(kConstant, Type::getFloatType(), ""), fScalar(value) {}
    ConstantValue(double value)
        : Value(kConstant, Type::getFloatType(), ""), dScalar(value) {}

  public:
    static ConstantValue *get(int value);
    static ConstantValue *get(float value);
    static ConstantValue *get(double value);

  public:
    static bool classof(const Value *value)
    {
      return value->getKind() == kConstant;
    }

  public:
    int getInt() const
    {
      assert(isInt());
      return iScalar;
    }
    float getFloat() const
    {
      assert(isFloat());
      return fScalar;
    }
    double getDouble() const
    {
      assert(isFloat());
      return dScalar;
    }

  public:
    void print(std::ostream &os) const override;
  }; // class ConstantValue

  class BasicBlock;
  /*!
   * Arguments of `BasicBlock`s.
   *
   * SysY IR is an SSA language, however, it does not use PHI instructions as in
   * LLVM IR. `Value`s from different predecessor blocks are passed explicitly as
   * block arguments. This is also the approach used by MLIR.
   * NOTE that `Function` does not own `Argument`s, function arguments are
   * implemented as its entry block's arguments.
   */

  class Argument : public Value
  {
  protected:
    BasicBlock *block;
    int index;
    std::vector<int> dims;

  public:
    Argument(Type *type, BasicBlock *block, int index, const std::vector<int> &dims = {},
             const std::string &name = "");

  public:
    static bool classof(const Value *value)
    {
      return value->getKind() == kArgument;
    }

  public:
    BasicBlock *getParent() const { return block; }
    int getIndex() const { return index; }
    int getNumDims() const { return dims.size(); }
    auto getDims() const { return make_range(dims.begin(), dims.end()); }
    int getDim(int index) const { return dims[index]; }

  public:
    void print(std::ostream &os) const override;
  };

  class Instruction;
  class Function;
  /*!
   * The container for `Instruction` sequence.
   *
   * `BasicBlock` maintains a list of `Instruction`s, with the last one being
   * a terminator (branch or return). Besides, `BasicBlock` stores its arguments
   * and records its predecessor and successor `BasicBlock`s.
   */
  class BasicBlock : public Value
  {
    friend class Function;

  public:
    using inst_list = std::list<std::unique_ptr<Instruction>>;
    using iterator = inst_list::iterator;
    using arg_list = std::vector<std::unique_ptr<Argument>>;
    using block_list = std::vector<BasicBlock *>;

  protected:
    Function *parent;
    inst_list instructions;
    arg_list arguments;
    block_list successors;
    block_list predecessors;

  protected:
    explicit BasicBlock(Function *parent, const std::string &name = "");

  public:
    static bool classof(const Value *value)
    {
      return value->getKind() == kBasicBlock;
    }

  public:
    int getNumInstructions() const { return instructions.size(); }
    int getNumArguments() const { return arguments.size(); }
    int getNumPredecessors() const { return predecessors.size(); }
    int getNumSuccessors() const { return successors.size(); }
    Function *getParent() const { return parent; }
    inst_list &getInstructions() { return instructions; }
    auto getArguments() const { return make_range(arguments); }
    block_list &getPredecessors() { return predecessors; }
    block_list &getSuccessors() { return successors; }
    iterator begin() { return instructions.begin(); }
    iterator end() { return instructions.end(); }
    iterator terminator() { return std::prev(end()); }
    Argument *createArgument(Type *type, const std::vector<int> &dims = {}, const std::string &name = "")
    // Argument *createArgument(Type *type, const std::string &name = "")
    {
      auto arg = new Argument(type, this, arguments.size(), dims, name);
      // auto arg = new Argument(type, this, arguments.size(), name);
      assert(arg);
      arguments.emplace_back(arg);
      return arguments.back().get();
    };

  public:
    void print(std::ostream &os) const override;
  }; // class BasicBlock

  //! User is the abstract base type of `Value` types which use other `Value` as
  //! operands. Currently, there are two kinds of `User`s, `Instruction` and
  //! `GlobalValue`.
  class User : public Value
  {
  protected:
    std::vector<Use> operands;

  protected:
    User(Kind kind, Type *type, const std::string &name = "")
        : Value(kind, type, name), operands() {}

  public:
    using use_iterator = std::vector<Use>::const_iterator;
    struct operand_iterator : public std::vector<Use>::const_iterator
    {
      using Base = std::vector<Use>::const_iterator;
      operand_iterator(const Base &iter) : Base(iter) {}
      using value_type = Value *;
      value_type operator->() { return Base::operator*().getValue(); }
      value_type operator*() { return Base::operator*().getValue(); }
    };
    // struct const_operand_iterator : std::vector<Use>::const_iterator {
    //   using Base = std::vector<Use>::const_iterator;
    //   const_operand_iterator(const Base &iter) : Base(iter) {}
    //   using value_type = Value *;
    //   value_type operator->() { return operator*().getValue(); }
    // };

  public:
    int getNumOperands() const { return operands.size(); }
    operand_iterator operand_begin() const { return operands.begin(); }
    operand_iterator operand_end() const { return operands.end(); }
    auto getOperands() const
    {
      return make_range(operand_begin(), operand_end());
    }
    Value *getOperand(int index) const { return operands[index].getValue(); }
    void addOperand(Value *value)
    {
      operands.emplace_back(operands.size(), this, value);
      value->addUse(&operands.back());
    }
    template <typename ContainerT>
    void addOperands(const ContainerT &operands)
    {
      for (auto value : operands)
        addOperand(value);
    }
    void replaceOperand(int index, Value *value);
    void setOperand(int index, Value *value);
  }; // class User

  /*!
   * Base of all concrete instruction types.
   */
  class Instruction : public User
  {
  public:
    // enum Kind : uint64_t {
    //   kInvalid = 0x0UL,
    //   // Binary
    //   kAdd = 0x1UL << 0,
    //   kSub = 0x1UL << 1,
    //   kMul = 0x1UL << 2,
    //   kDiv = 0x1UL << 3,
    //   kRem = 0x1UL << 4,
    //   kICmpEQ = 0x1UL << 5,
    //   kICmpNE = 0x1UL << 6,
    //   kICmpLT = 0x1UL << 7,
    //   kICmpGT = 0x1UL << 8,
    //   kICmpLE = 0x1UL << 9,
    //   kICmpGE = 0x1UL << 10,
    //   kFAdd = 0x1UL << 14,
    //   kFSub = 0x1UL << 15,
    //   kFMul = 0x1UL << 16,
    //   kFDiv = 0x1UL << 17,
    //   kFRem = 0x1UL << 18,
    //   kFCmpEQ = 0x1UL << 19,
    //   kFCmpNE = 0x1UL << 20,
    //   kFCmpLT = 0x1UL << 21,
    //   kFCmpGT = 0x1UL << 22,
    //   kFCmpLE = 0x1UL << 23,
    //   kFCmpGE = 0x1UL << 24,
    //   // Unary
    //   kNeg = 0x1UL << 25,
    //   kNot = 0x1UL << 26,
    //   kFNeg = 0x1UL << 27,
    //   kFtoI = 0x1UL << 28,
    //   kItoF = 0x1UL << 29,
    //   // call
    //   kCall = 0x1UL << 30,
    //   // terminator
    //   kCondBr = 0x1UL << 31,
    //   kBr = 0x1UL << 32,
    //   kReturn = 0x1UL << 33,
    //   // mem op
    //   kAlloca = 0x1UL << 34,
    //   kLoad = 0x1UL << 35,
    //   kStore = 0x1UL << 36,
    //   // constant
    //   // kConstant = 0x1UL << 37,
    // };

  protected:
    Kind kind;
    BasicBlock *parent;
    int protect_offset = -1;
    int pass_offset = -1;
    int protect_cnt = 0;

  protected:
    Instruction(Kind kind, Type *type, BasicBlock *parent = nullptr,
                const std::string &name = "");

  public:
    static bool classof(const Value *value)
    {
      return value->getKind() >= kFirstInst and value->getKind() <= kLastInst;
    }

  public:
    Kind getKind() const { return kind; }
    BasicBlock *getParent() const { return parent; }
    Function *getFunction() const { return parent->getParent(); }
    void setParent(BasicBlock *bb) { parent = bb; }
    void set_protect(int offset = 0) { protect_offset = offset; }
    void set_pass(int offset) { pass_offset = offset; }
    void set_protect_cnt(int cnt) { protect_cnt = cnt; }
    int ProtectOffset() { return protect_offset; }
    int PassOffset() { return pass_offset; }
    int ProtectCnt() { return protect_cnt; }

    bool isBinary() const
    {
      static constexpr uint64_t BinaryOpMask =
          (kAdd | kSub | kMul | kDiv | kRem) |
          (kICmpEQ | kICmpNE | kICmpLT | kICmpGT | kICmpLE | kICmpGE) |
          (kFAdd | kFSub | kFMul | kFDiv | kFRem) |
          (kFCmpEQ | kFCmpNE | kFCmpLT | kFCmpGT | kFCmpLE | kFCmpGE);
      return kind & BinaryOpMask;
    }
    bool isUnary() const
    {
      static constexpr uint64_t UnaryOpMask = kNeg | kNot | kFNeg | kFtoI | kItoF;
      return kind & UnaryOpMask;
    }
    bool isMemory() const
    {
      static constexpr uint64_t MemoryOpMask = kAlloca | kLoad | kStore;
      return kind & MemoryOpMask;
    }
    bool isTerminator() const
    {
      static constexpr uint64_t TerminatorOpMask = kCondBr | kBr | kReturn;
      return kind & TerminatorOpMask;
    }
    bool isCmp() const
    {
      static constexpr uint64_t CmpOpMask =
          (kICmpEQ | kICmpNE | kICmpLT | kICmpGT | kICmpLE | kICmpGE) |
          (kFCmpEQ | kFCmpNE | kFCmpLT | kFCmpGT | kFCmpLE | kFCmpGE);
      return kind & CmpOpMask;
    }
    bool isBranch() const
    {
      static constexpr uint64_t BranchOpMask = kBr | kCondBr;
      return kind & BranchOpMask;
    }
    bool isCommutative() const
    {
      static constexpr uint64_t CommutativeOpMask =
          kAdd | kMul | kICmpEQ | kICmpNE | kFAdd | kFMul | kFCmpEQ | kFCmpNE;
      return kind & CommutativeOpMask;
    }
    bool isUnconditional() const { return kind == kBr; }
    bool isConditional() const { return kind == kCondBr; }
  }; // class Instruction

  class Function;
  //! Function call.
  class CallInst : public Instruction
  {
    friend class IRBuilder;

  protected:
    CallInst(Function *callee, const std::vector<Value *> &args = {},
             BasicBlock *parent = nullptr, const std::string &name = "");

  public:
    static bool classof(const Value *value) { return value->getKind() == kCall; }

  public:
    Function *getCallee() const;
    auto getArguments() const
    {
      return make_range(std::next(operand_begin()), operand_end());
    }

  public:
    void print(std::ostream &os) const override;
  }; // class CallInst

  //! Unary instruction, includes '!', '-' and type conversion.
  class UnaryInst : public Instruction
  {
    friend class IRBuilder;

  protected:
    UnaryInst(Kind kind, Type *type, Value *operand, BasicBlock *parent = nullptr,
              const std::string &name = "")
        : Instruction(kind, type, parent, name)
    {
      addOperand(operand);
    }

  public:
    static bool classof(const Value *value)
    {
      return Instruction::classof(value) and
             static_cast<const Instruction *>(value)->isUnary();
    }

  public:
    Value *getOperand() const { return User::getOperand(0); }

  public:
    void print(std::ostream &os) const override;
  }; // class UnaryInst

  //! Binary instruction, e.g., arithmatic, relation, logic, etc.
  class BinaryInst : public Instruction
  {
    friend class IRBuilder;

  protected:
    BinaryInst(Kind kind, Type *type, Value *lhs, Value *rhs, BasicBlock *parent,
               const std::string &name = "")
        : Instruction(kind, type, parent, name)
    {
      addOperand(lhs);
      addOperand(rhs);
    }

  public:
    static bool classof(const Value *value)
    {
      return Instruction::classof(value) and
             static_cast<const Instruction *>(value)->isBinary();
    }

  public:
    Value *getLhs() const { return getOperand(0); }
    Value *getRhs() const { return getOperand(1); }

  public:
    void print(std::ostream &os) const override;
  }; // class BinaryInst

  //! The return statement
  class ReturnInst : public Instruction
  {
    friend class IRBuilder;

  protected:
    ReturnInst(Value *value = nullptr, BasicBlock *parent = nullptr)
        : Instruction(kReturn, Type::getVoidType(), parent, "")
    {
      if (value)
        addOperand(value);
    }

  public:
    static bool classof(const Value *value)
    {
      return value->getKind() == kReturn;
    }

  public:
    bool hasReturnValue() const { return not operands.empty(); }
    Value *getReturnValue() const
    {
      return hasReturnValue() ? getOperand(0) : nullptr;
    }

  public:
    void print(std::ostream &os) const override;
  }; // class ReturnInst

  //! Unconditional branch
  class UncondBrInst : public Instruction
  {
    friend class IRBuilder;

  protected:
    UncondBrInst(BasicBlock *block, std::vector<Value *> args,
                 BasicBlock *parent = nullptr)
        : Instruction(kBr, Type::getVoidType(), parent, "")
    {
      assert(block->getNumArguments() == args.size());
      addOperand(block);
      addOperands(args);
    }

  public:
    static bool classof(const Value *value) { return value->getKind() == kBr; }

  public:
    BasicBlock *getBlock() const { return dyncast<BasicBlock>(getOperand(0)); }
    auto getArguments() const
    {
      return make_range(std::next(operand_begin()), operand_end());
    }

  public:
    void print(std::ostream &os) const override;
  }; // class UncondBrInst

  //! Conditional branch
  class CondBrInst : public Instruction
  {
    friend class IRBuilder;

  protected:
    CondBrInst(Value *condition, BasicBlock *thenBlock, BasicBlock *elseBlock,
               const std::vector<Value *> &thenArgs,
               const std::vector<Value *> &elseArgs, BasicBlock *parent = nullptr)
        : Instruction(kCondBr, Type::getVoidType(), parent, "")
    {
      assert(thenBlock->getNumArguments() == thenArgs.size() and
             elseBlock->getNumArguments() == elseArgs.size());
      addOperand(condition);
      addOperand(thenBlock);
      addOperand(elseBlock);
      addOperands(thenArgs);
      addOperands(elseArgs);
    }

  public:
    static bool classof(const Value *value)
    {
      return value->getKind() == kCondBr;
    }

  public:
    Value *getCondition() const { return getOperand(0); }
    BasicBlock *getThenBlock() const
    {
      return dyncast<BasicBlock>(getOperand(1));
    }
    BasicBlock *getElseBlock() const
    {
      return dyncast<BasicBlock>(getOperand(2));
    }
    auto getThenArguments() const
    {
      auto begin = std::next(operand_begin(), 3);
      auto end = std::next(begin, getThenBlock()->getNumArguments());
      return make_range(begin, end);
    }
    auto getElseArguments() const
    {
      auto begin =
          std::next(operand_begin(), 3 + getThenBlock()->getNumArguments());
      auto end = operand_end();
      return make_range(begin, end);
    }

  public:
    void print(std::ostream &os) const override;
  }; // class CondBrInst

  //! Allocate memory for stack variables, used for non-global variable declartion
  class AllocaInst : public Instruction
  {
    friend class IRBuilder;

  protected:
    AllocaInst(Type *type, const std::vector<Value *> &dims = {},
               BasicBlock *parent = nullptr, const std::string &name = "", const bool isConst = false)
        : Instruction(kAlloca, type, parent, name), isConst(isConst)
    {
      addOperands(dims);
      if (isConst && dims.size() > 0)
      {
        int size = 1;
        for (std::size_t i = 0; i < dims.size(); i++)
          size *= dynamic_cast<ConstantValue *>(dims[i])->getInt();
        if (static_cast<const PointerType *>(getType())->getBaseType()->isInt())
          int_array = new int[size];
        else if (static_cast<const PointerType *>(getType())->getBaseType()->isFloat())
          double_array = new double[size];
      }
    }

  public:
    static bool classof(const Value *value)
    {
      return value->getKind() == kAlloca;
    }

  protected:
    bool isConst;
    int int_value;
    float float_value;
    double double_value;
    bool Int = false;
    int *int_array;
    double *double_array;

  public:
    int getNumDims() const { return getNumOperands(); }
    auto getDims() const { return getOperands(); }
    Value *getDim(int index) { return getOperand(index); }
    bool Const() const { return isConst; }
    void setInt(int value, const std::vector<Value *> &indices = {})
    {
      Int = true;
      if (indices.size() == 0)
        int_value = value;
      else
      {
        int factor = 1;
        int offset = 0;
        for (int i = indices.size() - 1; i >= 0; i--)
        {
          offset += factor * dynamic_cast<ConstantValue *>(indices[i])->getInt();
          factor *= dynamic_cast<ConstantValue *>(getDim(i))->getInt();
        }
        int_array[offset] = value;
      }
    }
    void setFloat(float value) { float_value = value; }
    void setDouble(double value, const std::vector<Value *> &indices = {})
    {
      if (indices.size() == 0)
        double_value = value;
      else
      {
        int factor = 1;
        int offset = 0;
        for (int i = indices.size() - 1; i >= 0; i--)
        {
          offset += factor * dynamic_cast<ConstantValue *>(indices[i])->getInt();
          factor *= dynamic_cast<ConstantValue *>(getDim(i))->getInt();
        }
        double_array[offset] = value;
      }
    }
    int getInt(const std::vector<Value *> &indices = {})
    {
      if (indices.size() == 0)
        return int_value;
      int factor = 1;
      int offset = 0;
      for (int i = indices.size() - 1; i >= 0; i--)
      {
        offset += factor * dynamic_cast<ConstantValue *>(indices[i])->getInt();
        factor *= dynamic_cast<ConstantValue *>(getDim(i))->getInt();
      }
      return int_array[offset];
    }
    float getFloat() { return float_value; }
    double getDouble(const std::vector<Value *> &indices = {})
    {
      if (indices.size() == 0)
        return double_value;
      int factor = 1;
      int offset = 0;
      for (int i = indices.size() - 1; i >= 0; i--)
      {
        offset += factor * dynamic_cast<ConstantValue *>(indices[i])->getInt();
        factor *= dynamic_cast<ConstantValue *>(getDim(i))->getInt();
      }
      return double_array[offset];
    }
    bool isInt() { return Int; }

  public:
    void print(std::ostream &os) const override;
  }; // class AllocaInst

  //! Load a value from memory address specified by a pointer value
  class LoadInst : public Instruction
  {
    friend class IRBuilder;

  protected:
    LoadInst(Value *pointer, const std::vector<Value *> &indices = {},
             BasicBlock *parent = nullptr, const std::string &name = "")
        : Instruction(kLoad, pointer->getType()->as<PointerType>()->getBaseType(),
                      parent, name)
    {
      addOperand(pointer);
      addOperands(indices);
    }

  public:
    static bool classof(const Value *value) { return value->getKind() == kLoad; }

  public:
    int getNumIndices() const { return getNumOperands() - 1; }
    Value *getPointer() const { return getOperand(0); }
    auto getIndices() const
    {
      return make_range(std::next(operand_begin()), operand_end());
    }
    Value *getIndex(int index) const { return getOperand(index + 1); }

  public:
    void print(std::ostream &os) const override;
  }; // class LoadInst

  //! Store a value to memory address specified by a pointer value
  class StoreInst : public Instruction
  {
    friend class IRBuilder;

  protected:
    StoreInst(Value *value, Value *pointer,
              const std::vector<Value *> &indices = {},
              BasicBlock *parent = nullptr, const std::string &name = "")
        : Instruction(kStore, Type::getVoidType(), parent, name)
    {
      addOperand(value);
      addOperand(pointer);
      addOperands(indices);
    }

  public:
    static bool classof(const Value *value) { return value->getKind() == kStore; }

  public:
    int getNumIndices() const { return getNumOperands() - 2; }
    Value *getValue() const { return getOperand(0); }
    Value *getPointer() const { return getOperand(1); }
    auto getIndices() const
    {
      return make_range(std::next(operand_begin(), 2), operand_end());
    }
    Value *getIndex(int index) const { return getOperand(index + 2); }

  public:
    void print(std::ostream &os) const override;
  }; // class StoreInst

  class Module;
  //! Function definition
  class Function : public Value
  {
    friend class Module;

  protected:
    Function(Module *parent, Type *type, const std::string &name)
        : Value(kFunction, type, name), parent(parent), variableID(0), blocks()
    {
      // blocks.emplace_back(new BasicBlock(this, "entry"));
    }

  public:
    static bool classof(const Value *value)
    {
      return value->getKind() == kFunction;
    }

  public:
    using block_list = std::list<std::unique_ptr<BasicBlock>>;

  protected:
    Module *parent;
    int variableID;
    int blockID;
    block_list blocks;
    block_list exit_blocks;

  public:
    Type *getReturnType() const
    {
      return getType()->as<FunctionType>()->getReturnType();
    }
    auto getParamTypes() const
    {
      return getType()->as<FunctionType>()->getParamTypes();
    }
    auto getBasicBlocks() const { return make_range(blocks); }
    BasicBlock *getEntryBlock() const { return blocks.front().get(); }
    BasicBlock *addBasicBlock(const std::string &name = "")
    {
      blocks.emplace_back(new BasicBlock(this, name));
      return blocks.back().get();
    }
    BasicBlock *addExitBlock(const std::string &name = "")
    {
      exit_blocks.emplace_back(new BasicBlock(this, name));
      return exit_blocks.back().get();
    }
    void moveExitBlock()
    {
      // move(exit_blocks.back());
      blocks.emplace_back(move(exit_blocks.back()));
      exit_blocks.pop_back();
    }
    void removeBasicBlock(BasicBlock *block)
    {
      blocks.remove_if([&](std::unique_ptr<BasicBlock> &b) -> bool
                       { return block == b.get(); });
    }
    int allocateVariableID() { return variableID++ % 11; }
    void resetVariableID() { variableID = 0; }
    int allocateblockID() { return blockID++; }

  public:
    void print(std::ostream &os) const override;
  }; // class Function

  // class ArrayValue : public User {
  // protected:
  //   ArrayValue(Type *type, const std::vector<Value *> &values = {})
  //       : User(type, "") {
  //     addOperands(values);
  //   }

  // public:
  //   static ArrayValue *get(Type *type, const std::vector<Value *> &values);
  //   static ArrayValue *get(const std::vector<int> &values);
  //   static ArrayValue *get(const std::vector<float> &values);

  // public:
  //   auto getValues() const { return getOperands(); }

  // public:
  //   void print(std::ostream &os) const override{};
  // }; // class ConstantArray

  //! Global value declared at file scope
  class GlobalValue : public User
  {
    friend class Module;

  protected:
    Module *parent;
    bool hasInit;
    bool isConst;
    bool Int = false;
    int *int_array;
    double *double_array;
    int size = 1;

  protected:
    //******************Revised by lyq BEGIN***************************************
    GlobalValue(Module *parent, Type *type, const std::string &name,
                const std::vector<Value *> &dims = {}, Value *init = nullptr, bool isconst = false)
        : User(kGlobal, type, name), parent(parent), hasInit(init), isConst(isconst)
    //******************Revised by lyq END*****************************************
    {
      assert(type->isPointer());
      addOperands(dims);
      size = 1;
      if (init)
        addOperand(init);
      if (dims.size() > 0)
      {
        for (std::size_t i = 0; i < dims.size(); i++)
          size *= dynamic_cast<ConstantValue *>(dims[i])->getInt();
        if (static_cast<const PointerType *>(getType())->getBaseType()->isInt())
          int_array = new int[size]{};
        else if (static_cast<const PointerType *>(getType())->getBaseType()->isFloat())
          double_array = new double[size]{};
      }
    }

  public:
    static bool classof(const Value *value)
    {
      return value->getKind() == kGlobal;
    }

  public:
    Value *init() const { return hasInit ? operands.back().getValue() : nullptr; }
    int getNumDims() const { return getNumOperands() - (hasInit ? 1 : 0); }
    Value *getDim(int index) { return getOperand(index); }
    //******************Revised by lyq BEGIN***************************************
    bool isconst() { return isConst; }
    //******************Revised by lyq END*****************************************
    void setInt(int value, const std::vector<Value *> &indices = {})
    {
      Int = true;
      int factor = 1;
      int offset = 0;
      for (int i = indices.size() - 1; i >= 0; i--)
      {
        offset += factor * dynamic_cast<ConstantValue *>(indices[i])->getInt();
        factor *= dynamic_cast<ConstantValue *>(getDim(i))->getInt();
      }
      int_array[offset] = value;
    }
    void setDouble(double value, const std::vector<Value *> &indices = {})
    {
      int factor = 1;
      int offset = 0;
      for (int i = indices.size() - 1; i >= 0; i--)
      {
        offset += factor * dynamic_cast<ConstantValue *>(indices[i])->getInt();
        factor *= dynamic_cast<ConstantValue *>(getDim(i))->getInt();
      }
      double_array[offset] = value;
    }
    int getInt(const std::vector<Value *> &indices = {})
    {
      int factor = 1;
      int offset = 0;
      for (int i = indices.size() - 1; i >= 0; i--)
      {
        offset += factor * dynamic_cast<ConstantValue *>(indices[i])->getInt();
        factor *= dynamic_cast<ConstantValue *>(getDim(i))->getInt();
      }
      return int_array[offset];
    }
    double getDouble(const std::vector<Value *> &indices = {})
    {
      int factor = 1;
      int offset = 0;
      for (int i = indices.size() - 1; i >= 0; i--)
      {
        offset += factor * dynamic_cast<ConstantValue *>(indices[i])->getInt();
        factor *= dynamic_cast<ConstantValue *>(getDim(i))->getInt();
      }
      return double_array[offset];
    }
    int *getIntarray() { return int_array; }
    double *getDoublearray() { return double_array; }
    bool isInt() { return Int; }
    int getsize() { return size; }

  public:
    void print(std::ostream &os) const override;
  }; // class GlobalValue

  //! IR unit for representing a SysY compile unit
  class Module
  {
  protected:
    std::vector<std::unique_ptr<Value>> children;
    std::map<std::string, Function *> functions;
    std::map<std::string, GlobalValue *> globals;

  public:
    Module() = default;

  public:
    Function *createFunction(const std::string &name, Type *type)
    {
      if (functions.count(name))
        return nullptr;
      auto func = new Function(this, type, name);
      assert(func);
      children.emplace_back(func);
      functions.emplace(name, func);
      return func;
    };
    //******************Revised by lyq BEGIN***************************************
    GlobalValue *createGlobalValue(const std::string &name, Type *type,
                                   const std::vector<Value *> &dims = {},
                                   Value *init = nullptr,
                                   bool isconst = false)
    {
      if (globals.count(name))
        return nullptr;
      auto global = new GlobalValue(this, type, name, dims, init, isconst);
      assert(global);
      children.emplace_back(global);
      globals.emplace(name, global);
      return global;
    }
    //******************Revised by lyq END*****************************************
    Function *getFunction(const std::string &name) const
    {
      auto result = functions.find(name);
      if (result == functions.end())
        return nullptr;
      return result->second;
    }
    GlobalValue *getGlobalValue(const std::string &name) const
    {
      auto result = globals.find(name);
      if (result == globals.end())
        return nullptr;
      return result->second;
    }

  public:
    void print(std::ostream &os) const;

  public:
    std::map<std::string, Function *> *getFunctions()
    {
      return &functions;
    }
    std::map<std::string, GlobalValue *> *getGlobalValues()
    {
      return &globals;
    }
  }; // class Module

  /*!
   * @}
   */
  inline std::ostream &operator<<(std::ostream &os, const Type &type)
  {
    type.print(os);
    return os;
  }

  inline std::ostream &operator<<(std::ostream &os, const Value &value)
  {
    value.print(os);
    return os;
  }

} // namespace sysy
