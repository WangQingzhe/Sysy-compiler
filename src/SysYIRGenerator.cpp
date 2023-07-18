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
    auto getint_entry = f_getint->addBasicBlock("getint_entry");
    // create function:putint
    auto putint_type = Type::getFunctionType(Type::getVoidType(), vector<Type *>({Type::getIntType()}));
    auto f_putint = pModule->createFunction("putint", putint_type);
    symbols.insert("putint", f_putint);
    // SymbolTable::FunctionScope putint_scope(symbols);
    auto putint_entry = f_putint->addBasicBlock("putint_entry");
    auto putint_arg = putint_entry->createArgument(Type::getPointerType(Type::getIntType()));
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
      // if is a scalar
      if (dims.size() == 0)
      {
        auto init = varDef->ASSIGN() ? any_cast<Value *>((varDef->initValue()->exp()->accept(this)))
                                     : nullptr;
        //******************Revised by lyq BEGIN***************************************
        auto global_value = module->createGlobalValue(name, type, dims, init, isConst);
        //******************Revised by lyq END*****************************************
        symbols.insert(name, global_value);
        values.push_back(global_value);
      }
      // if is an array
      else
      {
        auto init = varDef->ASSIGN() ? any_cast<Value *>(dims[0])
                                     : nullptr;
        auto global_value = module->createGlobalValue(name, type, dims, init, isConst);
        // if initialize
        if (varDef->ASSIGN())
        {
          d = 0;
          n = 0;
          path.clear();
          path = vector<int>(dims.size(), 0);
          isalloca = false;
          current_type = global_value->getType()->as<PointerType>()->getBaseType();
          current_global = global_value;
          numdims = global_value->getNumDims();
          for (auto init : varDef->initValue()->initValue())
            visitInitValue(init);
        }
        symbols.insert(name, global_value);
        values.push_back(global_value);
      }
    }
    // FIXME const
    return values;
  }

  any SysYIRGenerator::visitLocalDecl(SysYParser::DeclContext *ctx)
  {
    // a single declaration statement can declare several variables,
    // collect them in a vector
    std::vector<Value *> values;
    bool isConst = ctx->CONST();
    // obtain the declared type
    auto type = Type::getPointerType(any_cast<Type *>(visitBtype(ctx->btype())));
    // handle variables
    for (auto varDef : ctx->varDef())
    {
      // obtain the variable name and allocate space on the stack
      auto name = varDef->lValue()->ID()->getText();
      // if var is an array, get its dimensions
      vector<Value *> dims;
      for (auto dim : varDef->lValue()->exp())
        dims.push_back(any_cast<Value *>(dim->accept(this)));
      // if var is a constant
      auto alloca = builder.createAllocaInst(type, dims, name, isConst);
      // update the symbol table
      symbols.insert(name, alloca);
      if (varDef->ASSIGN())
      {
        // if alloca is scalar,only one store inst
        if (alloca->getNumDims() == 0)
        {
          auto value = any_cast<Value *>(varDef->initValue()->exp()->accept(this));
          if (isa<ConstantValue>(value))
          {
            // if var is int, convert the constant into int type
            if (ctx->btype()->INT() && dynamic_cast<ConstantValue *>(value)->isFloat())
              value = ConstantValue::get((int)dynamic_cast<ConstantValue *>(value)->getDouble());
            else if (ctx->btype()->FLOAT() && dynamic_cast<ConstantValue *>(value)->isInt())
              value = ConstantValue::get((double)dynamic_cast<ConstantValue *>(value)->getInt());
          }
          else if (alloca->getType()->as<PointerType>()->getBaseType()->isInt() && value->getType()->isFloat())
            value = builder.createFtoIInst(value);
          else if (alloca->getType()->as<PointerType>()->getBaseType()->isFloat() && value->getType()->isInt())
            value = builder.createIToFInst(value);
          auto store = builder.createStoreInst(value, alloca);
          builder.getBasicBlock()->getParent()->resetVariableID();
          // if var is a constant scalar, store its value to alloca inst
          if (isConst)
          {
            auto constant_var = dynamic_cast<ConstantValue *>(value);
            if (constant_var->isInt())
              alloca->setInt(constant_var->getInt());
            else if (constant_var->isFloat())
              alloca->setDouble(constant_var->getDouble());
          }
        }
        // if alloca is an array,initialize
        else
        {
          d = 0;
          n = 0;
          path.clear();
          path = vector<int>(alloca->getNumDims(), 0);
          isalloca = true;
          current_alloca = alloca;
          current_type = alloca->getType()->as<PointerType>()->getBaseType();
          numdims = alloca->getNumDims();
          for (auto init : varDef->initValue()->initValue())
            visitInitValue(init);
        }
      }
      // collect the created variable (pointer)
      values.push_back(alloca);
    }
    return values;
  }
  any SysYIRGenerator::visitInitValue(SysYParser::InitValueContext *ctx)
  {
    if (ctx->exp())
    {
      auto value = any_cast<Value *>(ctx->exp()->accept(this));
      // convert the constant into type of the lvalue
      if (isa<ConstantValue>(value))
      {
        if (current_type->isInt() && value->getType()->isFloat())
          value = ConstantValue::get((int)dynamic_cast<ConstantValue *>(value)->getDouble());
        else if (current_type->isFloat() && value->getType()->isInt())
          value = ConstantValue::get((double)dynamic_cast<ConstantValue *>(value)->getInt());
      }
      else if (current_type->isInt() && value->getType()->isFloat())
        value = builder.createFtoIInst(value);
      else if (current_type->isFloat() && value->getType()->isInt())
        value = builder.createIToFInst(value);
      // goto the last dimension
      while (d < numdims - 1)
      {
        path[d++] = n;
        n = 0;
      }
      vector<Value *> indices;
      for (int i = 0; i < numdims - 1; i++)
        indices.push_back(ConstantValue::get(path[i]));
      indices.push_back(ConstantValue::get(n));
      if (isalloca)
      {
        // store exp into alloca
        auto store = builder.createStoreInst(value, current_alloca, indices);
        builder.getBasicBlock()->getParent()->resetVariableID();
        // if array is const store value into alloca
        if (current_alloca->Const())
        {
          auto constant_var = dynamic_cast<ConstantValue *>(value);
          if (constant_var->isInt())
            current_alloca->setInt(constant_var->getInt(), indices);
          else if (constant_var->isFloat())
            current_alloca->setDouble(constant_var->getDouble(), indices);
        }
        // goto next element
        n++;
        while (d >= 0 && n >= dynamic_cast<ConstantValue *>(current_alloca->getDim(d))->getInt())
          n = path[--d] + 1;
      }
      else
      {
        auto constant_var = dynamic_cast<ConstantValue *>(value);
        if (constant_var->isInt())
          current_global->setInt(constant_var->getInt(), indices);
        else if (constant_var->isFloat())
          current_global->setDouble(constant_var->getDouble(), indices);
        // goto next element
        n++;
        while (d >= 0 && n >= dynamic_cast<ConstantValue *>(current_global->getDim(d))->getInt())
          n = path[--d] + 1;
      }
      return value;
    }
    else
    {
      int cur_d = d;
      int cur_n = n;
      Value *value;
      for (auto init : ctx->initValue())
        visitInitValue(init);
      d = cur_d;
      n = cur_n;
      n++;
      if (isalloca)
        while (d >= 0 && n >= dynamic_cast<ConstantValue *>(current_alloca->getDim(d))->getInt())
          n = path[--d] + 1;
      else
        while (d >= 0 && n >= dynamic_cast<ConstantValue *>(current_global->getDim(d))->getInt())
          n = path[--d] + 1;
      return value;
    }
  }

  any SysYIRGenerator::visitFunc(SysYParser::FuncContext *ctx)
  {
    // obtain function name and type signature
    builder.func_add();
    auto name = ctx->ID()->getText();
    vector<Type *> paramTypes;
    vector<string> paramNames;
    if (ctx->funcFParams())
    {
      auto params = ctx->funcFParams()->funcFParam();
      for (auto param : params)
      {
        paramTypes.push_back(Type::getPointerType(any_cast<Type *>(visitBtype(param->btype()))));
        paramNames.push_back(param->ID()->getText());
      }
    }
    Type *returnType = any_cast<Type *>(visitFuncType(ctx->funcType()));
    auto funcType = Type::getFunctionType(returnType, paramTypes);
    // create the IR function
    auto function = module->createFunction(name, funcType);
    char entry_name[10];
    sprintf(entry_name, "entry%d", builder.get_funccnt());
    // update the symbol table
    symbols.insert(name, function);
    // create the function scope
    SymbolTable::FunctionScope scope(symbols);
    // create the entry block with the same parameters of the function,
    // and update the symbol table
    auto entry = function->addBasicBlock(entry_name);
    // auto entry = function->getEntryBlock();
    if (ctx->funcFParams())
    {
      auto params = ctx->funcFParams()->funcFParam();
      vector<int> dims;
      for (std::size_t i = 0; i < paramTypes.size(); ++i)
      {
        dims.clear();
        // if param is an array
        if (params[i]->LBRACKET().size())
        {
          dims.push_back(0);
          for (auto exp : params[i]->exp())
            dims.push_back(dynamic_cast<ConstantValue *>(any_cast<Value *>(exp->accept(this)))->getInt());
        }
        auto arg = entry->createArgument(paramTypes[i], dims, paramNames[i]);
        // auto arg = entry->createArgument(paramTypes[i], paramNames[i]);
        symbols.insert(paramNames[i], arg);
      }
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
    // if rhs is constant,convert it into the same type with pointer
    if (isa<ConstantValue>(rhs))
    {
      if (pointer->getType()->as<PointerType>()->getBaseType()->isInt())
      {
        if (dynamic_cast<ConstantValue *>(rhs)->isFloat())
          rhs = ConstantValue::get((int)dynamic_cast<ConstantValue *>(rhs)->getDouble());
      }
      else if (pointer->getType()->as<PointerType>()->getBaseType()->isFloat())
      {
        if (dynamic_cast<ConstantValue *>(rhs)->isInt())
          rhs = ConstantValue::get((double)dynamic_cast<ConstantValue *>(rhs)->getInt());
      }
    }
    else if (pointer->getType()->as<PointerType>()->getBaseType()->isInt() && rhs->getType()->isFloat())
      rhs = builder.createFtoIInst(rhs);
    else if (pointer->getType()->as<PointerType>()->getBaseType()->isFloat() && rhs->getType()->isInt())
      rhs = builder.createIToFInst(rhs);
    // update the variable
    vector<Value *> indices;
    for (auto exp : ctx->lValue()->exp())
      indices.push_back(any_cast<Value *>(exp->accept(this)));
    Value *store = builder.createStoreInst(rhs, pointer, indices);
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
          ConstantValue::get(std::stod(ctx->number()->FLITERAL()->getText()));
    return result;
  }

  any SysYIRGenerator::visitLValueExp(SysYParser::LValueExpContext *ctx)
  {
    auto name = ctx->lValue()->ID()->getText();
    Value *value = symbols.lookup(name);
    if (not value)
      error(ctx, "undefined variable");
    vector<Value *> indices;
    for (auto exp : ctx->lValue()->exp())
      indices.push_back(any_cast<Value *>(exp->accept(this)));
    if (isa<GlobalValue>(value))
    {
      auto global_val = dynamic_cast<GlobalValue *>(value);
      if (global_val->isconst())
      {
        if (global_val->getNumDims() == 0)
          value = global_val->init();
        else if (global_val->isInt())
          value = ConstantValue::get(global_val->getInt(indices));
        else
          value = ConstantValue::get(global_val->getDouble(indices));
      }
      else
        value = builder.createLoadInst(value, indices);
    }
    else if (isa<AllocaInst>(value))
    {
      auto alloca_inst = dynamic_cast<AllocaInst *>(value);
      if (alloca_inst->Const())
      {
        if (alloca_inst->isInt())
          value = ConstantValue::get(alloca_inst->getInt(indices));
        else
          value = ConstantValue::get(alloca_inst->getDouble(indices));
      }
      else
        value = builder.createLoadInst(value, indices);
    }
    else if (isa<Argument>(value))
    {
      value = builder.createLoadInst(value, indices);
    }
    return value;
  }

  any SysYIRGenerator::visitAdditiveExp(SysYParser::AdditiveExpContext *ctx)
  {
    // generate the operands
    auto lhs = any_cast<Value *>(ctx->exp(0)->accept(this));
    auto rhs = any_cast<Value *>(ctx->exp(1)->accept(this));
    // judge if lhs is a constant
    bool lconst = false;
    int lint = 0;
    float lfloat = 0;
    double ldouble = 0;
    if (isa<ConstantValue>(lhs))
    {
      lconst = true;
      if (lhs->isInt())
      {
        lint = dynamic_cast<ConstantValue *>(lhs)->getInt();
        ldouble = dynamic_cast<ConstantValue *>(lhs)->getInt();
      }
      else if (lhs->isFloat())
        // lfloat = dynamic_cast<ConstantValue *>(lhs)->getFloat();
        ldouble = dynamic_cast<ConstantValue *>(lhs)->getDouble();
    }
    // else if (isa<AllocaInst>(lhs) && dynamic_cast<AllocaInst *>(lhs)->Const())
    // {
    //   lconst = true;
    //   if (dynamic_cast<AllocaInst *>(lhs)->isInt())
    //   {
    //     lint = dynamic_cast<AllocaInst *>(lhs)->getInt();
    //     ldouble = dynamic_cast<AllocaInst *>(lhs)->getInt();
    //   }
    //   else
    //     // lfloat = dynamic_cast<AllocaInst *>(lhs)->getFloat();
    //     ldouble = dynamic_cast<AllocaInst *>(lhs)->getDouble();
    // }
    // judge if rhs is a constant
    bool rconst = false;
    int rint = 0;
    float rfloat = 0;
    double rdouble = 0;
    if (isa<ConstantValue>(rhs))
    {
      rconst = true;
      if (rhs->isInt())
      {
        rint = dynamic_cast<ConstantValue *>(rhs)->getInt();
        rdouble = dynamic_cast<ConstantValue *>(rhs)->getInt();
      }
      else if (rhs->isFloat())
        // rfloat = dynamic_cast<ConstantValue *>(rhs)->getFloat();
        rdouble = dynamic_cast<ConstantValue *>(rhs)->getDouble();
    }
    // else if (isa<AllocaInst>(rhs) && dynamic_cast<AllocaInst *>(rhs)->Const())
    // {
    //   rconst = true;
    //   if (dynamic_cast<AllocaInst *>(rhs)->isInt())
    //   {
    //     rint = dynamic_cast<AllocaInst *>(rhs)->getInt();
    //     rdouble = dynamic_cast<AllocaInst *>(rhs)->getInt();
    //   }
    //   else
    //     // rfloat = dynamic_cast<AllocaInst *>(rhs)->getFloat();
    //     rdouble = dynamic_cast<AllocaInst *>(rhs)->getDouble();
    // }
    // create convert instruction if needed
    auto lhsTy = lhs->getType();
    auto rhsTy = rhs->getType();
    auto type = getArithmeticResultType(lhsTy, rhsTy);
    if (lhsTy != type && lconst)
      lhs = ConstantValue::get((double)(dynamic_cast<ConstantValue *>(lhs)->getInt()));
    else if (lhsTy != type)
      lhs = builder.createIToFInst(lhs);
    if (rhsTy != type && rconst)
      rhs = ConstantValue::get((double)(dynamic_cast<ConstantValue *>(rhs)->getInt()));
    else if (rhsTy != type)
      rhs = builder.createIToFInst(rhs);
    // create the arithmetic instruction
    Value *result = nullptr;
    if (ctx->ADD())
    {
      if (lconst && rconst)
        // result = type->isInt() ? ConstantValue::get(lint + rint) : ConstantValue::get(lfloat + rfloat);
        result = type->isInt() ? ConstantValue::get(lint + rint) : ConstantValue::get(ldouble + rdouble);
      else
        result = type->isInt() ? builder.createAddInst(lhs, rhs)
                               : builder.createFAddInst(lhs, rhs);
    }
    else
    {
      if (lconst && rconst)
        result = type->isInt() ? ConstantValue::get(lint - rint) : ConstantValue::get(ldouble - rdouble);
      else
        result = type->isInt() ? builder.createSubInst(lhs, rhs)
                               : builder.createFSubInst(lhs, rhs);
    }
    return result;
  }

  any SysYIRGenerator::visitMultiplicativeExp(
      SysYParser::MultiplicativeExpContext *ctx)
  {
    // generate the operands
    auto lhs = any_cast<Value *>(ctx->exp(0)->accept(this));
    auto rhs = any_cast<Value *>(ctx->exp(1)->accept(this));
    // judge if lhs is a constant
    bool lconst = false;
    int lint = 0;
    float lfloat = 0;
    double ldouble = 0;
    if (isa<ConstantValue>(lhs))
    {
      lconst = true;
      if (lhs->isInt())
      {
        lint = dynamic_cast<ConstantValue *>(lhs)->getInt();
        ldouble = dynamic_cast<ConstantValue *>(lhs)->getInt();
      }
      else if (lhs->isFloat())
        // lfloat = dynamic_cast<ConstantValue *>(lhs)->getFloat();
        ldouble = dynamic_cast<ConstantValue *>(lhs)->getDouble();
    }
    // else if (isa<AllocaInst>(lhs) && dynamic_cast<AllocaInst *>(lhs)->Const())
    // {
    //   lconst = true;
    //   if (dynamic_cast<AllocaInst *>(lhs)->isInt())
    //   {
    //     lint = dynamic_cast<AllocaInst *>(lhs)->getInt();
    //     ldouble = dynamic_cast<AllocaInst *>(lhs)->getInt();
    //   }
    //   else
    //     // lfloat = dynamic_cast<AllocaInst *>(lhs)->getFloat();
    //     ldouble = dynamic_cast<AllocaInst *>(lhs)->getDouble();
    // }
    // judge if rhs is a constant
    bool rconst = false;
    int rint = 0;
    float rfloat = 0;
    double rdouble = 0;
    if (isa<ConstantValue>(rhs))
    {
      rconst = true;
      if (rhs->isInt())
      {
        rint = dynamic_cast<ConstantValue *>(rhs)->getInt();
        rdouble = dynamic_cast<ConstantValue *>(rhs)->getInt();
      }
      else if (rhs->isFloat())
        // rfloat = dynamic_cast<ConstantValue *>(rhs)->getFloat();
        rdouble = dynamic_cast<ConstantValue *>(rhs)->getDouble();
    }
    // else if (isa<AllocaInst>(rhs) && dynamic_cast<AllocaInst *>(rhs)->Const())
    // {
    //   rconst = true;
    //   if (dynamic_cast<AllocaInst *>(rhs)->isInt())
    //   {
    //     rint = dynamic_cast<AllocaInst *>(rhs)->getInt();
    //     rdouble = dynamic_cast<AllocaInst *>(rhs)->getInt();
    //   }
    //   else
    //     // rfloat = dynamic_cast<AllocaInst *>(rhs)->getFloat();
    //     rdouble = dynamic_cast<AllocaInst *>(rhs)->getDouble();
    // }
    // create convert instruction if needed
    auto lhsTy = lhs->getType();
    auto rhsTy = rhs->getType();
    auto type = getArithmeticResultType(lhsTy, rhsTy);
    if (lhsTy != type && lconst)
      lhs = ConstantValue::get((double)(dynamic_cast<ConstantValue *>(lhs)->getInt()));
    else if (lhsTy != type)
      lhs = builder.createIToFInst(lhs);
    if (rhsTy != type && rconst)
      rhs = ConstantValue::get((double)(dynamic_cast<ConstantValue *>(rhs)->getInt()));
    else if (rhsTy != type)
      rhs = builder.createIToFInst(rhs);
    // create the arithmetic instruction
    Value *result = nullptr;
    if (ctx->MUL())
    {
      if (lconst && rconst)
        result = type->isInt() ? ConstantValue::get(lint * rint) : ConstantValue::get(ldouble * rdouble);
      else
        result = type->isInt() ? builder.createMulInst(lhs, rhs)
                               : builder.createFMulInst(lhs, rhs);
    }
    else if (ctx->DIV())
    {
      if (lconst && rconst)
        result = type->isInt() ? ConstantValue::get(lint / rint) : ConstantValue::get(ldouble / rdouble);
      else
        result = type->isInt() ? builder.createDivInst(lhs, rhs)
                               : builder.createFDivInst(lhs, rhs);
    }

    else
    {
      if (lconst && rconst)
        result = type->isInt() ? ConstantValue::get(lint % rint) : ConstantValue::get(lint % rint);
      else
        result = type->isInt() ? builder.createRemInst(lhs, rhs)
                               : builder.createFRemInst(lhs, rhs);
    }
    return result;
  }

  any SysYIRGenerator::visitReturnStmt(SysYParser::ReturnStmtContext *ctx)
  {
    auto value =
        ctx->exp() ? any_cast<Value *>(ctx->exp()->accept(this)) : nullptr;
    auto current_block = builder.getBasicBlock();
    auto func = current_block->getParent();
    if (func->getReturnType()->isInt() && value->getType()->isFloat())
    {
      if (isa<ConstantValue>(value))
        value = ConstantValue::get((int)dynamic_cast<ConstantValue *>(value)->getDouble());
      else
        value = builder.createFtoIInst(value);
    }
    else if (func->getReturnType()->isFloat() && value->getType()->isInt())
    {
      if (isa<ConstantValue>(value))
        value = ConstantValue::get((float)dynamic_cast<ConstantValue *>(value)->getInt());
      else
        value = builder.createIToFInst(value);
    }
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
      auto iter = func->getParamTypes().begin();
      for (auto exp : rArgs->exp())
      {
        Value *arg = any_cast<Value *>(exp->accept(this));
        if ((*iter)->isInt() && arg->getType()->isFloat())
          arg = builder.createFtoIInst(arg);
        else if ((*iter)->isFloat() && arg->getType()->isInt())
          arg = builder.createIToFInst(arg);
        args.push_back(arg);
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
    // create condbr instr
    // visit thenblock(and elseblock)
    if (ctx->stmt().size() == 1)
    {
      // if-then
      // generate condition expression
      // create exit basicblock
      char exitname[20];
      sprintf(exitname, "exit%d", builder.get_ifcnt() + builder.get_whilecnt());
      auto exitblock = func->addExitBlock(exitname);
      // auto exitblock = func->addBasicBlock(exitname);
      exitblock->getPredecessors().push_back(thenblock);
      thenblock->getSuccessors().push_back(exitblock);
      builder.push_truetarget(thenblock);
      builder.push_falsetarget(exitblock);
      auto cond = any_cast<Value *>(ctx->exp()->accept(this));
      char flagname[20];
      sprintf(flagname, "flag%d", builder.get_ifcnt() + builder.get_whilecnt());
      // cond->setName(flagname);
      current_block->getSuccessors().push_back(exitblock);
      exitblock->getPredecessors().push_back(current_block);
      Value *CondBr = builder.createCondBrInst(cond, builder.get_truetarget(), builder.get_falsetarget(), vector<Value *>(), vector<Value *>());
      builder.poptarget();
      func->resetVariableID();
      builder.setPosition(thenblock, thenblock->begin());
      visitStmt(ctx->stmt()[0]);
      Value *then_br = builder.createUncondBrInst(exitblock, vector<Value *>());
      // setup the instruction insert position
      builder.setPosition(exitblock, exitblock->begin());
      func->moveExitBlock();
    }
    if (ctx->stmt().size() == 2)
    {
      // if-then-else
      // create else basicblock
      char elsename[20];
      sprintf(elsename, "else%d", builder.get_ifcnt());
      auto elseblock = func->addBasicBlock(elsename);
      // create exit basicblock
      char exitname[20];
      sprintf(exitname, "exit%d", builder.get_ifcnt() + builder.get_whilecnt());
      // auto exitblock = func->addBasicBlock(exitname);
      auto exitblock = func->addExitBlock(exitname);
      exitblock->getPredecessors().push_back(thenblock);
      thenblock->getSuccessors().push_back(exitblock);
      current_block->getSuccessors().push_back(elseblock);
      elseblock->getPredecessors().push_back(current_block);
      elseblock->getSuccessors().push_back(exitblock);
      exitblock->getPredecessors().push_back(current_block);
      // generate condition expression
      builder.push_truetarget(thenblock);
      builder.push_falsetarget(elseblock);
      auto cond = any_cast<Value *>(ctx->exp()->accept(this));
      CondBrInst *CondBr = builder.createCondBrInst(cond, builder.get_truetarget(), builder.get_falsetarget(), vector<Value *>(), vector<Value *>());
      builder.poptarget();
      func->resetVariableID();
      builder.setPosition(thenblock, thenblock->begin());
      visitStmt(ctx->stmt()[0]);
      Value *then_br = builder.createUncondBrInst(exitblock, vector<Value *>());
      builder.setPosition(elseblock, elseblock->begin());
      visitStmt(ctx->stmt()[1]);
      Value *else_br = builder.createUncondBrInst(exitblock, vector<Value *>());
      // setup the instruction insert position
      builder.setPosition(exitblock, exitblock->begin());
      func->moveExitBlock();
    }
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
    Value *head_uncondbr = builder.createUncondBrInst(headerblock, vector<Value *>{});
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
    auto exitblock = func->addExitBlock(exitname);
    headerblock->getSuccessors().push_back(exitblock);
    exitblock->getPredecessors().push_back(headerblock);
    // push header&exit into loopstack
    builder.pushheader(headerblock);
    builder.pushexit(exitblock);
    // generate condition expression
    builder.push_truetarget(bodyblock);
    builder.push_falsetarget(exitblock);
    auto cond = any_cast<Value *>(ctx->exp()->accept(this));
    // char flagname[20];
    // sprintf(flagname, "flag%d", builder.get_ifcnt() + builder.get_whilecnt());
    // cond->setName(flagname);
    // create condbr in header
    Value *header_condbr = builder.createCondBrInst(cond, builder.get_truetarget(), builder.get_falsetarget(), vector<Value *>(), vector<Value *>());
    builder.poptarget();
    func->resetVariableID();
    // generate code in body block
    builder.setPosition(bodyblock, bodyblock->begin());
    visitStmt(ctx->stmt());
    // pop header&exit from loopstack
    builder.poploop();
    // create uncondbr in body block
    Value *body_uncondbr = builder.createUncondBrInst(headerblock, vector<Value *>());
    // setup the instruction insert position
    builder.setPosition(exitblock, exitblock->begin());
    func->moveExitBlock();
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

    Value *result = nullptr;
    if (ctx->SUB())
    {
      auto hs = any_cast<Value *>(ctx->exp()->accept(this));
      if (isa<ConstantValue>(hs))
      {
        if (hs->isInt())
          result = ConstantValue::get(-dynamic_cast<ConstantValue *>(hs)->getInt());
        else if (hs->isFloat())
          result = ConstantValue::get(-dynamic_cast<ConstantValue *>(hs)->getDouble());
      }
      else if (isa<AllocaInst>(hs) && dynamic_cast<AllocaInst *>(hs)->getNumDims() == 0)
      {
        if (dynamic_cast<AllocaInst *>(hs)->isInt())
        {
          result = ConstantValue::get(-dynamic_cast<AllocaInst *>(hs)->getInt());
        }
        else
          result = ConstantValue::get(-dynamic_cast<AllocaInst *>(hs)->getDouble());
      }
      else if (hs->getType()->isInt())
        result = builder.createNegInst(hs);
      else if (hs->getType()->isFloat())
        result = builder.createFNegInst(hs);
    }
    else if (ctx->NOT())
    // result = builder.createNotInst(hs);
    {
      auto true_target = builder.get_falsetarget();
      auto false_target = builder.get_truetarget();
      builder.poptarget();
      builder.push_truetarget(true_target);
      builder.push_falsetarget(false_target);
      auto hs = any_cast<Value *>(ctx->exp()->accept(this));
      result = hs;
    }
    else if (ctx->ADD())
    {
      auto hs = any_cast<Value *>(ctx->exp()->accept(this));
      result = hs;
    }
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
    // create rhs block
    builder.rhs_add();
    auto current_block = builder.getBasicBlock();
    auto func = current_block->getParent();
    char rhs_name[20];
    sprintf(rhs_name, "rhs%d", builder.get_rhscnt());
    auto rhs_block = func->addBasicBlock(rhs_name);
    rhs_block->getPredecessors().push_back(current_block);
    current_block->getSuccessors().push_back(rhs_block);
    builder.push_truetarget(rhs_block);
    builder.push_falsetarget(builder.get_falsetarget());
    // get lhs value
    auto lhs = any_cast<Value *>(ctx->exp()[0]->accept(this));
    // create condbr instr
    Value *condbr = builder.createCondBrInst(lhs, builder.get_truetarget(), builder.get_falsetarget(), vector<Value *>(), vector<Value *>());
    builder.poptarget();
    func->resetVariableID();
    // generate code for rhs block
    builder.setPosition(rhs_block, rhs_block->begin());
    return (ctx->exp()[1]->accept(this));
  }
  any SysYIRGenerator::visitOrExp(SysYParser::OrExpContext *ctx)
  {
    // create rhs block
    builder.rhs_add();
    auto current_block = builder.getBasicBlock();
    auto func = current_block->getParent();
    char rhs_name[20];
    sprintf(rhs_name, "rhs%d", builder.get_rhscnt());
    auto rhs_block = func->addBasicBlock(rhs_name);
    rhs_block->getPredecessors().push_back(current_block);
    current_block->getSuccessors().push_back(rhs_block);
    builder.push_truetarget(builder.get_truetarget());
    builder.push_falsetarget(rhs_block);
    // get lhs value
    auto lhs = any_cast<Value *>(ctx->exp()[0]->accept(this));
    // create condbr instr
    Value *condbr = builder.createCondBrInst(lhs, builder.get_truetarget(), builder.get_falsetarget(), vector<Value *>(), vector<Value *>());
    builder.poptarget();
    func->resetVariableID();
    // generate code for rhs block
    builder.setPosition(rhs_block, rhs_block->begin());
    return (ctx->exp()[1]->accept(this));
  }
  any SysYIRGenerator::visitParenExp(SysYParser::ParenExpContext *ctx)
  {
    return ctx->exp()->accept(this);
  }
} // namespace sysy
