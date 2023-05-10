#include "IR.h"
#include <any>
#include <iostream>
#include <memory>
#include <vector>
using namespace std;
#include "Diagnostic.h"
#include "SysYIRGenerator.h"

namespace sysy
{

  any SysYIRGenerator::visitModule(SysYParser::ModuleContext *ctx)
  {
    // global scople of the module
    SymbolTable::ModuleScope scope(symbols);
    // create the IR module
    auto pModule = new Module();
    assert(pModule);
    module.reset(pModule);
    // create function:getint
    auto getint_type = Type::getFunctionType(Type::getIntType());
    auto f_getint = pModule->createFunction("getint", getint_type);
    symbols.insert("getint", f_getint);
    // create function:putint
    auto putint_type = Type::getFunctionType(Type::getVoidType(), vector<Type *>({Type::getIntType()}));
    auto f_putint = pModule->createFunction("putint", putint_type);
    symbols.insert("putint", f_putint);
    // generates globals and functions
    visitChildren(ctx);
    // return the IR module
    return pModule;
  }

  any SysYIRGenerator::visitDecl(SysYParser::DeclContext *ctx)
  {
    // global and local declarations are handled in different ways
    return symbols.isModuleScope() ? visitGlobalDecl(ctx) : visitLocalDecl(ctx);
  }

  any SysYIRGenerator::visitGlobalDecl(SysYParser::DeclContext *ctx)
  {
    // error(ctx, "not implemented yet");
    std::vector<Value *> values;
    bool isConst = ctx->CONST();
    auto type = Type::getPointerType(any_cast<Type *>(visitBtype(ctx->btype())));
    for (auto varDef : ctx->varDef())
    {
      auto name = varDef->lValue()->ID()->getText();
      vector<Value *> dims;
      for (auto exp : varDef->lValue()->exp())
        dims.push_back(any_cast<Value *>(exp->accept(this)));
      auto init = varDef->ASSIGN()
                      ? any_cast<Value *>(visitInitValue(varDef->initValue()))
                      : nullptr;
 //******************Revised by lyq BEGIN***************************************
      auto global_value = module->createGlobalValue(name, type, dims, init, isConst);
//******************Revised by lyq END*****************************************
      symbols.insert(name, global_value);
      values.push_back(global_value);
    }
    // FIXME const
    return values;
  }

  any SysYIRGenerator::visitLocalDecl(SysYParser::DeclContext *ctx)
  {
    // a single declaration statement can declare several variables,
    // collect them in a vector
    std::vector<Value *> values;
    // obtain the declared type
    auto type = Type::getPointerType(any_cast<Type *>(visitBtype(ctx->btype())));
    // handle variables
    for (auto varDef : ctx->varDef())
    {
      // obtain the variable name and allocate space on the stack
      auto name = varDef->lValue()->ID()->getText();
      auto alloca = builder.createAllocaInst(type, {}, name);
      // update the symbol table
      symbols.insert(name, alloca);
      // if an initial value is given, create a store instruction
      if (varDef->ASSIGN())
      {
        auto value = any_cast<Value *>(visitInitValue(varDef->initValue()));
        auto store = builder.createStoreInst(value, alloca);
      }
      // collect the created variable (pointer)
      values.push_back(alloca);
    }
    return values;
  }

  any SysYIRGenerator::visitFunc(SysYParser::FuncContext *ctx)
  {
    // obtain function name and type signature
    auto name = ctx->ID()->getText();
    vector<Type *> paramTypes;
    vector<string> paramNames;
    if (ctx->funcFParams())
    {
      auto params = ctx->funcFParams()->funcFParam();
      for (auto param : params)
      {
        paramTypes.push_back(any_cast<Type *>(visitBtype(param->btype())));
        paramNames.push_back(param->ID()->getText());
      }
    }
    Type *returnType = any_cast<Type *>(visitFuncType(ctx->funcType()));
    auto funcType = Type::getFunctionType(returnType, paramTypes);
    // create the IR function
    auto function = module->createFunction(name, funcType);
    // update the symbol table
    symbols.insert(name, function);
    // create the function scope
    SymbolTable::FunctionScope scope(symbols);
    // create the entry block with the same parameters as the function,
    // and update the symbol table
    auto entry = function->getEntryBlock();
    for (auto i = 0; i < paramTypes.size(); ++i)
    {
      auto arg = entry->createArgument(paramTypes[i], paramNames[i]);
      symbols.insert(paramNames[i], arg);
    }
    // setup the instruction insert point
    builder.setPosition(entry, entry->end());
    // generate the function body
    visitBlockStmt(ctx->blockStmt());
    return function;
  }

  any SysYIRGenerator::visitBtype(SysYParser::BtypeContext *ctx)
  {
    return ctx->INT() ? Type::getIntType() : Type::getFloatType();
  }
  any SysYIRGenerator::visitFuncType(SysYParser::FuncTypeContext *ctx)
  {
    return ctx->INT()
               ? Type::getIntType()
               : (ctx->FLOAT() ? Type::getFloatType() : Type::getVoidType());
  }

  any SysYIRGenerator::visitBlockStmt(SysYParser::BlockStmtContext *ctx)
  {
    // create the block scope
    SymbolTable::BlockScope scope(symbols);
    // create new basicblock

    // the insert point has already been setup
    for (auto item : ctx->blockItem())
      visitBlockItem(item);
    // return the corresponding IR block
    return builder.getBasicBlock();
  }

  any SysYIRGenerator::visitAssignStmt(SysYParser::AssignStmtContext *ctx)
  {
    // generate the rhs expression
    auto rhs = any_cast<Value *>(ctx->exp()->accept(this));
    // get the address of the lhs variable
    auto lValue = ctx->lValue();
    auto name = lValue->ID()->getText();
    auto pointer = symbols.lookup(name);
    if (not pointer)
      error(ctx, "undefined variable");
    // update the variable
    Value *store = builder.createStoreInst(rhs, pointer);
    return store;
  }

  any SysYIRGenerator::visitNumberExp(SysYParser::NumberExpContext *ctx)
  {
    Value *result = nullptr;
    assert(ctx->number()->ILITERAL() or ctx->number()->FLITERAL());
    if (auto iLiteral = ctx->number()->ILITERAL())
    {
      std::string s = iLiteral->getText();
      int base = 10;
      // hex
      if (s.length() > 2 && s[0] == '0' && (s[1] == 'X' || s[1] == 'x'))
        base = 16;
      // oct
      else if (s[0] == '0')
        base = 8;
      // dec
      else
        base = 10;
      result = ConstantValue::get(std::stoi(iLiteral->getText(), 0, base));
    }
    else
      result =
          ConstantValue::get(std::stof(ctx->number()->FLITERAL()->getText()));
    return result;
  }

  any SysYIRGenerator::visitLValueExp(SysYParser::LValueExpContext *ctx)
  {
    auto name = ctx->lValue()->ID()->getText();
    Value *value = symbols.lookup(name);
    if (not value)
      error(ctx, "undefined variable");
    if (isa<GlobalValue>(value) or isa<AllocaInst>(value))
      value = builder.createLoadInst(value);
    return value;
  }

  any SysYIRGenerator::visitAdditiveExp(SysYParser::AdditiveExpContext *ctx)
  {
    // generate the operands
    auto lhs = any_cast<Value *>(ctx->exp(0)->accept(this));
    auto rhs = any_cast<Value *>(ctx->exp(1)->accept(this));
    // create convert instruction if needed
    auto lhsTy = lhs->getType();
    auto rhsTy = rhs->getType();
    auto type = getArithmeticResultType(lhsTy, rhsTy);
    if (lhsTy != type)
      lhs = builder.createIToFInst(lhs);
    if (rhsTy != type)
      rhs = builder.createIToFInst(rhs);
    // create the arithmetic instruction
    Value *result = nullptr;
    if (ctx->ADD())
      result = type->isInt() ? builder.createAddInst(lhs, rhs)
                             : builder.createFAddInst(lhs, rhs);
    else
      result = type->isInt() ? builder.createSubInst(lhs, rhs)
                             : builder.createFSubInst(lhs, rhs);
    return result;
  }

  any SysYIRGenerator::visitMultiplicativeExp(
      SysYParser::MultiplicativeExpContext *ctx)
  {
    // generate the operands
    auto lhs = any_cast<Value *>(ctx->exp(0)->accept(this));
    auto rhs = any_cast<Value *>(ctx->exp(1)->accept(this));
    // create convert instruction if needed
    auto lhsTy = lhs->getType();
    auto rhsTy = rhs->getType();
    auto type = getArithmeticResultType(lhsTy, rhsTy);
    if (lhsTy != type)
      lhs = builder.createIToFInst(lhs);
    if (rhsTy != type)
      rhs = builder.createIToFInst(rhs);
    // create the arithmetic instruction
    Value *result = nullptr;
    if (ctx->MUL())
      result = type->isInt() ? builder.createMulInst(lhs, rhs)
                             : builder.createFMulInst(lhs, rhs);
    else if (ctx->DIV())
      result = type->isInt() ? builder.createDivInst(lhs, rhs)
                             : builder.createFDivInst(lhs, rhs);

    else
      result = type->isInt() ? builder.createRemInst(lhs, rhs)
                             : builder.createFRemInst(lhs, rhs);
    return result;
  }

  any SysYIRGenerator::visitReturnStmt(SysYParser::ReturnStmtContext *ctx)
  {
    auto value =
        ctx->exp() ? any_cast<Value *>(ctx->exp()->accept(this)) : nullptr;
    Value *result = builder.createReturnInst(value);
    return result;
  }

  any SysYIRGenerator::visitCall(SysYParser::CallContext *ctx)
  {
    auto funcName = ctx->ID()->getText();
    auto func = dyncast<Function>(symbols.lookup(funcName));
    assert(func);
    vector<Value *> args;
    if (auto rArgs = ctx->funcRParams())
    {
      for (auto exp : rArgs->exp())
      {
        args.push_back(any_cast<Value *>(exp->accept(this)));
      }
    }
    Value *call = builder.createCallInst(func, args);
    return call;
  }

  any SysYIRGenerator::visitIfStmt(SysYParser::IfStmtContext *ctx)
  {
    builder.if_add();
    auto current_block = builder.getBasicBlock();
    auto func = current_block->getParent();
    // create then basicblock
    char thenname[20];
    sprintf(thenname, "then%d", builder.get_ifcnt());
    auto thenblock = func->addBasicBlock(thenname);
    current_block->getSuccessors().push_back(thenblock);
    thenblock->getPredecessors().push_back(current_block);
    // create exit basicblock
    char exitname[20];
    sprintf(exitname, "exit%d", builder.get_ifcnt() + builder.get_whilecnt());
    auto exitblock = func->addBasicBlock(exitname);
    exitblock->getPredecessors().push_back(thenblock);
    thenblock->getSuccessors().push_back(exitblock);

    // create condbr instr
    // visit thenblock(and elseblock)
    if (ctx->stmt().size() == 1)
    {
      // if-then
      // generate condition expression
      builder.push_truetarget(thenblock);
      builder.push_falsetarget(exitblock);
      auto cond = any_cast<Value *>(ctx->exp()->accept(this));
      builder.poptarget();
      char flagname[20];
      sprintf(flagname, "flag%d", builder.get_ifcnt() + builder.get_whilecnt());
      cond->setName(flagname);
      current_block->getSuccessors().push_back(exitblock);
      exitblock->getPredecessors().push_back(current_block);
      Value *CondBr = builder.createCondBrInst(cond, thenblock, exitblock, vector<Value *>(), vector<Value *>());
      builder.setPosition(thenblock, thenblock->begin());
      visitStmt(ctx->stmt()[0]);
      Value *then_br = builder.createUncondBrInst(exitblock, vector<Value *>());
    }
    if (ctx->stmt().size() == 2)
    {
      // if-then-else
      // create else basicblock
      char elsename[20];
      sprintf(elsename, "else%d", builder.get_ifcnt());
      auto elseblock = func->addBasicBlock(elsename);
      current_block->getSuccessors().push_back(elseblock);
      elseblock->getPredecessors().push_back(current_block);
      elseblock->getSuccessors().push_back(exitblock);
      exitblock->getPredecessors().push_back(current_block);
      // generate condition expression
      builder.push_truetarget(thenblock);
      builder.push_falsetarget(elseblock);
      auto cond = any_cast<Value *>(ctx->exp()->accept(this));
      builder.poptarget();
      CondBrInst *CondBr = builder.createCondBrInst(cond, thenblock, elseblock, vector<Value *>(), vector<Value *>());
      builder.setPosition(thenblock, thenblock->begin());
      visitStmt(ctx->stmt()[0]);
      Value *then_br = builder.createUncondBrInst(exitblock, vector<Value *>());
      builder.setPosition(elseblock, elseblock->begin());
      visitStmt(ctx->stmt()[1]);
      Value *else_br = builder.createUncondBrInst(exitblock, vector<Value *>());
    }
    // setup the instruction insert position
    builder.setPosition(exitblock, exitblock->begin());
    return builder.getBasicBlock();
  }

  any SysYIRGenerator::visitWhileStmt(SysYParser::WhileStmtContext *ctx)
  {
    builder.while_add();
    auto current_block = builder.getBasicBlock();
    auto func = current_block->getParent();
    // create header basicblock
    char headername[20];
    sprintf(headername, "header%d", builder.get_whilecnt());
    auto headerblock = func->addBasicBlock(headername);
    builder.setPosition(headerblock, headerblock->begin());
    current_block->getSuccessors().push_back(headerblock);
    headerblock->getPredecessors().push_back(current_block);
    // uncondbr:current->header
    // Value *Current_uncondbr = builder.createUncondBrInst(headerblock, vector<Value *>());
    // create body basicblock
    char bodyname[20];
    sprintf(bodyname, "body%d", builder.get_whilecnt());
    auto bodyblock = func->addBasicBlock(bodyname);
    headerblock->getSuccessors().push_back(bodyblock);
    bodyblock->getPredecessors().push_back(headerblock);
    // create exit basicblock
    char exitname[20];
    sprintf(exitname, "exit%d", builder.get_whilecnt() + builder.get_ifcnt());
    auto exitblock = func->addBasicBlock(exitname);
    headerblock->getSuccessors().push_back(exitblock);
    exitblock->getPredecessors().push_back(headerblock);
    // push header&exit into loopstack
    builder.pushheader(headerblock);
    builder.pushexit(exitblock);
    // generate condition expression
    builder.push_truetarget(bodyblock);
    builder.push_falsetarget(exitblock);
    auto cond = any_cast<Value *>(ctx->exp()->accept(this));
    builder.poptarget();
    char flagname[20];
    sprintf(flagname, "flag%d", builder.get_ifcnt() + builder.get_whilecnt());
    cond->setName(flagname);
    // create condbr in header
    Value *header_condbr = builder.createCondBrInst(cond, bodyblock, exitblock, vector<Value *>(), vector<Value *>());
    // generate code in body block
    builder.setPosition(bodyblock, bodyblock->begin());
    visitStmt(ctx->stmt());
    // pop header&exit from loopstack
    builder.poploop();
    // create uncondbr in body block
    Value *body_uncondbr = builder.createUncondBrInst(headerblock, vector<Value *>());
    // setup the instruction insert position
    builder.setPosition(exitblock, exitblock->begin());
    return builder.getBasicBlock();
  }
  any SysYIRGenerator::visitBreakStmt(SysYParser::BreakStmtContext *ctx)
  {
    Value *uncondbr = builder.createUncondBrInst(builder.getExit(), vector<Value *>());
    return uncondbr;
  }
  any SysYIRGenerator::visitContinueStmt(SysYParser::ContinueStmtContext *ctx)
  {
    Value *uncondbr = builder.createUncondBrInst(builder.getHeader(), vector<Value *>());
    return uncondbr;
  }
  //******************Revised by lyq BEGIN*************************************
  any SysYIRGenerator::visitUnaryExp(SysYParser::UnaryExpContext *ctx)
  {
    // generate the operands
    auto hs = any_cast<Value *>(ctx->exp()->accept(this));

    Value *result = nullptr;
    if (ctx->SUB())
      result = builder.createNegInst(hs);
    else if (ctx->NOT())
      result = builder.createNotInst(hs);
    else if (ctx->ADD())
      result = hs;
    return result;
  }

  any SysYIRGenerator::visitRelationExp(SysYParser::RelationExpContext *ctx)
  {
    // generate the operands
    auto lhs = any_cast<Value *>(ctx->exp(0)->accept(this));
    auto rhs = any_cast<Value *>(ctx->exp(1)->accept(this));
    // create convert instruction if needed
    auto lhsTy = lhs->getType();
    auto rhsTy = rhs->getType();
    auto type = getArithmeticResultType(lhsTy, rhsTy);
    if (lhsTy != type)
      lhs = builder.createIToFInst(lhs);
    if (rhsTy != type)
      rhs = builder.createIToFInst(rhs);

    Value *result = nullptr;
    if (ctx->LT())
      result = type->isInt() ? builder.createICmpLTInst(lhs, rhs)
                             : builder.createFCmpLTInst(lhs, rhs);
    else if (ctx->GT())
      result = type->isInt() ? builder.createICmpGTInst(lhs, rhs)
                             : builder.createFCmpGTInst(lhs, rhs);
    else if (ctx->LE())
      result = type->isInt() ? builder.createICmpLEInst(lhs, rhs)
                             : builder.createFCmpLEInst(lhs, rhs);
    else if (ctx->GE())
      result = type->isInt() ? builder.createICmpGEInst(lhs, rhs)
                             : builder.createFCmpGEInst(lhs, rhs);
    return result;
  }

  any SysYIRGenerator::visitEqualExp(SysYParser::EqualExpContext *ctx)
  {
    // generate the operands
    auto lhs = any_cast<Value *>(ctx->exp(0)->accept(this));
    auto rhs = any_cast<Value *>(ctx->exp(1)->accept(this));
    // create convert instruction if needed
    auto lhsTy = lhs->getType();
    auto rhsTy = rhs->getType();
    auto type = getArithmeticResultType(lhsTy, rhsTy);
    if (lhsTy != type)
      lhs = builder.createIToFInst(lhs);
    if (rhsTy != type)
      rhs = builder.createIToFInst(rhs);

    Value *result = nullptr;
    if (ctx->EQ())
      result = type->isInt() ? builder.createICmpEQInst(lhs, rhs)
                             : builder.createFCmpEQInst(lhs, rhs);
    else if (ctx->NE())
      result = type->isInt() ? builder.createICmpNEInst(lhs, rhs)
                             : builder.createFCmpNEInst(lhs, rhs);
    return result;
  }
  //******************Revised by lyq END***************************************

  any SysYIRGenerator::visitAndExp(SysYParser::AndExpContext *ctx)
  {
    // get lhs value
    auto lhs = any_cast<Value *>(ctx->exp()[0]->accept(this));
    // create rhs block
    builder.rhs_add();
    auto current_block = builder.getBasicBlock();
    auto func = current_block->getParent();
    char rhs_name[20];
    sprintf(rhs_name, "rhs%d", builder.get_rhscnt());
    auto rhs_block = func->addBasicBlock(rhs_name);
    rhs_block->getPredecessors().push_back(current_block);
    current_block->getSuccessors().push_back(rhs_block);
    // create condbr instr
    Value *condbr = builder.createCondBrInst(lhs, rhs_block, builder.get_falsetarget(), vector<Value *>(), vector<Value *>());
    // generate code for rhs block
    builder.setPosition(rhs_block, rhs_block->begin());
    return (ctx->exp()[1]->accept(this));
  }
  any SysYIRGenerator::visitOrExp(SysYParser::OrExpContext *ctx)
  {
    // get lhs value
    auto lhs = any_cast<Value *>(ctx->exp()[0]->accept(this));
    // create rhs block
    builder.rhs_add();
    auto current_block = builder.getBasicBlock();
    auto func = current_block->getParent();
    char rhs_name[20];
    sprintf(rhs_name, "rhs%d", builder.get_rhscnt());
    auto rhs_block = func->addBasicBlock(rhs_name);
    rhs_block->getPredecessors().push_back(current_block);
    current_block->getSuccessors().push_back(rhs_block);
    // create condbr instr
    Value *condbr = builder.createCondBrInst(lhs, builder.get_truetarget(), rhs_block, vector<Value *>(), vector<Value *>());
    // generate code for rhs block
    builder.setPosition(rhs_block, rhs_block->begin());
    return (ctx->exp()[1]->accept(this));
  }
  any SysYIRGenerator::visitParenExp(SysYParser::ParenExpContext *ctx)
  {
    return ctx->exp()->accept(this);
  }
} // namespace sysy
