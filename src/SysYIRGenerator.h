#pragma once

#include "IR.h"
#include "IRBuilder.h"
#include "SysYBaseVisitor.h"
#include "SysYParser.h"
#include <cassert>
#include <forward_list>
#include <memory>
#include <string>

namespace sysy
{

  class SymbolTable
  {
  private:
    enum Kind
    {
      kModule,
      kFunction,
      kBlock,
    };

  public:
    struct ModuleScope
    {
      SymbolTable &table;
      ModuleScope(SymbolTable &table) : table(table) { table.enter(kModule); }
      ~ModuleScope() { table.exit(); }
    };
    struct FunctionScope
    {
      SymbolTable &table;
      FunctionScope(SymbolTable &table) : table(table) { table.enter(kFunction); }
      ~FunctionScope() { table.exit(); }
    };

    struct BlockScope
    {
      SymbolTable &table;
      BlockScope(SymbolTable &table) : table(table) { table.enter(kBlock); }
      ~BlockScope() { table.exit(); }
    };

  private:
    std::forward_list<std::pair<Kind, std::map<std::string, Value *>>> symbols;

  public:
    SymbolTable() = default;

  public:
    bool isModuleScope() const { return symbols.front().first == kModule; }
    bool isFunctionScope() const { return symbols.front().first == kFunction; }
    bool isBlockScope() const { return symbols.front().first == kBlock; }
    Value *lookup(const std::string &name) const
    {
      for (auto &scope : symbols)
      {
        auto iter = scope.second.find(name);
        if (iter != scope.second.end())
          return iter->second;
      }
      return nullptr;
    }
    auto insert(const std::string &name, Value *value)
    {
      assert(not symbols.empty());
      return symbols.front().second.emplace(name, value);
    }
    // void remove(const std::string &name) {
    //   for (auto &scope : symbols) {
    //     auto iter = scope.find(name);
    //     if (iter != scope.end()) {
    //       scope.erase(iter);
    //       return;
    //     }
    //   }
    // }
  private:
    void enter(Kind kind)
    {
      symbols.emplace_front();
      symbols.front().first = kind;
    }
    void exit() { symbols.pop_front(); }
  }; // class SymbolTable

  class SysYIRGenerator : public SysYBaseVisitor
  {
  private:
    std::unique_ptr<Module> module;
    IRBuilder builder;
    SymbolTable symbols;
    int d = 0, n = 0;
    vector<int> path;
    bool isalloca;
    AllocaInst *current_alloca;
    GlobalValue *current_global;
    Type *current_type;
    int numdims = 0;

  public:
    SysYIRGenerator() = default;

  public:
    Module *get() const { return module.get(); }

  public:
    virtual std::any visitModule(SysYParser::ModuleContext *ctx) override;

    virtual std::any visitDecl(SysYParser::DeclContext *ctx) override;

    virtual std::any visitBtype(SysYParser::BtypeContext *ctx) override;

    virtual std::any visitVarDef(SysYParser::VarDefContext *ctx) override
    {
      return visitChildren(ctx);
    }

    virtual std::any visitInitValue(SysYParser::InitValueContext *ctx) override;

    virtual std::any visitFunc(SysYParser::FuncContext *ctx) override;

    virtual std::any visitFuncType(SysYParser::FuncTypeContext *ctx) override;

    virtual std::any
    visitFuncFParams(SysYParser::FuncFParamsContext *ctx) override
    {
      return visitChildren(ctx);
    }

    virtual std::any
    visitFuncFParam(SysYParser::FuncFParamContext *ctx) override
    {
      return visitChildren(ctx);
    }

    virtual std::any visitBlockStmt(SysYParser::BlockStmtContext *ctx) override;

    // virtual std::any visitBlockItem(SysYParser::BlockItemContext *ctx)
    // override;

    virtual std::any visitStmt(SysYParser::StmtContext *ctx) override
    {
      return visitChildren(ctx);
    }

    virtual std::any visitAssignStmt(SysYParser::AssignStmtContext *ctx) override;

    virtual std::any visitExpStmt(SysYParser::ExpStmtContext *ctx) override
    {
      return visitChildren(ctx);
    }

    virtual std::any visitIfStmt(SysYParser::IfStmtContext *ctx) override;

    virtual std::any visitWhileStmt(SysYParser::WhileStmtContext *ctx) override;

    virtual std::any visitBreakStmt(SysYParser::BreakStmtContext *ctx) override;

    virtual std::any
    visitContinueStmt(SysYParser::ContinueStmtContext *ctx) override;

    virtual std::any
    visitReturnStmt(SysYParser::ReturnStmtContext *ctx) override;

    virtual std::any visitEmptyStmt(SysYParser::EmptyStmtContext *ctx) override
    {
      return visitChildren(ctx);
    }

    virtual std::any
    visitRelationExp(SysYParser::RelationExpContext *ctx) override;

    virtual std::any
    visitMultiplicativeExp(SysYParser::MultiplicativeExpContext *ctx) override;

    virtual std::any visitLValueExp(SysYParser::LValueExpContext *ctx) override;

    virtual std::any visitNumberExp(SysYParser::NumberExpContext *ctx) override;

    virtual std::any visitAndExp(SysYParser::AndExpContext *ctx) override;

    virtual std::any visitUnaryExp(SysYParser::UnaryExpContext *ctx) override;

    virtual std::any visitParenExp(SysYParser::ParenExpContext *ctx) override;

    virtual std::any visitStringExp(SysYParser::StringExpContext *ctx) override
    {
      return visitChildren(ctx);
    }

    virtual std::any visitOrExp(SysYParser::OrExpContext *ctx) override;

    virtual std::any visitCallExp(SysYParser::CallExpContext *ctx) override
    {
      return visitChildren(ctx);
    }

    virtual std::any
    visitAdditiveExp(SysYParser::AdditiveExpContext *ctx) override;

    virtual std::any visitEqualExp(SysYParser::EqualExpContext *ctx) override;

    virtual std::any visitCall(SysYParser::CallContext *ctx) override;

    virtual std::any visitLValue(SysYParser::LValueContext *ctx) override
    {
      return visitChildren(ctx);
    }

    virtual std::any visitNumber(SysYParser::NumberContext *ctx) override
    {
      return visitChildren(ctx);
    }

    virtual std::any visitString(SysYParser::StringContext *ctx) override
    {
      return visitChildren(ctx);
    }

    virtual std::any
    visitFuncRParams(SysYParser::FuncRParamsContext *ctx) override
    {
      return visitChildren(ctx);
    }
    virtual std::any
    visitCondExp(SysYParser::ExpContext *ctx) { return visitChildren(ctx); }

  private:
    std::any visitGlobalDecl(SysYParser::DeclContext *ctx);
    std::any visitLocalDecl(SysYParser::DeclContext *ctx);
    Type *getArithmeticResultType(Type *lhs, Type *rhs)
    {
      assert(lhs->isIntOrFloat() and rhs->isIntOrFloat());
      return lhs == rhs ? lhs : Type::getFloatType();
    }
  }; // class SysYIRGenerator

    
  class DAG
  {
    private:
      Module *module;
      std::vector<DAG_Function *> functions;
    public:
      DAG(Module *module) : module(module){}
    public:
      Module* DAG_refine();
  };


  class DAG_Function
  {
    private:
      Function* function;
      std::vector<DAG_BB *> basicblocks;
    public:
      DAG_Function(Function* function) : function(function){}
    public:
      void DAG_Function_refine();
  };

  class DAG_BB
  {
    private:
      BasicBlock *basicblock;
      //map from value to node
      std::map<Value *, DAG_node *> value2node;
      // //map from ID to node
      // std::map<string, DAG_node *> ID2NODE;
      // //map from int to node
      // std::map<int, DAG_node *> INT2NODE;
      // //map from float to node
      // std::map<float, DAG_node *> FLOAT2NODE;
      //all nodes
      std::vector<DAG_node *> nodelist;
    public:
      DAG_BB(BasicBlock *bb) : basicblock(bb){}
      void DAG_BB_refine();
  };


  class DAG_node : public Value
  {
    public:
      enum OP : uint64_t
      {
        knoop,
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
      };
    private:
      //node identifiers
      int node_id;

      //if is mid-node, maybe has operator
      bool mid;
      OP op;
      
      //if is leaf-node, maybe has constant number
      bool leaf;
      int intconst;
      float floatconst;

      //some identifiers that associated to this node 
      std::vector<string> ids;
      
      //relationship with other nodes
      std::vector<DAG_node *>sons;
      std::vector<DAG_node *>fathers;




    public:
      DAG_node(bool isleaf, OP op) 
          : leaf(isleaf), mid(!isleaf), op(op) {}

    
  };
} // namespace sysy