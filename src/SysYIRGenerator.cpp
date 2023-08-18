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
        // auto getint_entry = f_getint->addBasicBlock("getint_entry");
        // create function:getch
        auto getch_type = Type::getFunctionType(Type::getIntType());
        auto f_getch = pModule->createFunction("getch", getint_type);
        symbols.insert("getch", f_getch);
        // auto getch_entry = f_getch->addBasicBlock("getch_entry");
        // create function:getfloat
        auto getfloat_type = Type::getFunctionType(Type::getFloatType());
        auto f_getfloat = pModule->createFunction("getfloat", getfloat_type);
        symbols.insert("getfloat", f_getfloat);
        // auto getfloat_entry = f_getfloat->addBasicBlock("getfloat_entry");
        // create function:getarray
        auto getarray_type = Type::getFunctionType(Type::getIntType(), {Type::getPointerType(Type::getIntType())});
        auto f_getarray = pModule->createFunction("getarray", getarray_type);
        symbols.insert("getarray", f_getarray);
        // auto getarray_entry = f_getarray->addBasicBlock("getarray_entry");
        // auto getarray_arg = getarray_entry->createArgument(Type::getPointerType(Type::getIntType()));
        // create function:getfarray
        auto getfarray_type = Type::getFunctionType(Type::getIntType(), {Type::getPointerType(Type::getFloatType())});
        auto f_getfarray = pModule->createFunction("getfarray", getfarray_type);
        symbols.insert("getfarray", f_getfarray);
        // create function:putint
        auto putint_type = Type::getFunctionType(Type::getVoidType(), {Type::getPointerType(Type::getIntType())});
        auto f_putint = pModule->createFunction("putint", putint_type);
        symbols.insert("putint", f_putint);
        // auto putint_entry = f_putint->addBasicBlock("putint_entry");
        // auto putint_arg = putint_entry->createArgument(Type::getPointerType(Type::getIntType()));
        // create function:putch
        auto putch_type = Type::getFunctionType(Type::getVoidType(), {Type::getPointerType(Type::getIntType())});
        auto f_putch = pModule->createFunction("putch", putch_type);
        symbols.insert("putch", f_putch);
        // auto putint_entry = f_putint->addBasicBlock("putch_entry");
        // auto putint_arg = putint_entry->createArgument(Type::getPointerType(Type::getIntType()));
        // create function:putfloat
        auto putfloat_type = Type::getFunctionType(Type::getVoidType(), {Type::getPointerType(Type::getFloatType())});
        auto f_putfloat = pModule->createFunction("putfloat", putfloat_type);
        symbols.insert("putfloat", f_putfloat);
        auto putfloat_entry = f_putfloat->addBasicBlock("putfloat_entry");
        // auto putint_arg = putint_entry->createArgument(Type::getPointerType(Type::getIntType()));
        // create function:putarray
        auto putarray_type = Type::getFunctionType(Type::getVoidType(), {Type::getPointerType(Type::getIntType()), Type::getPointerType(Type::getIntType())});
        auto f_putarray = pModule->createFunction("putarray", putarray_type);
        symbols.insert("putarray", f_putarray);
        // auto putint_entry = f_putint->addBasicBlock("putfloat_entry");
        // auto putint_arg = putint_entry->createArgument(Type::getPointerType(Type::getIntType()));
        // create function:putfarray
        auto putfarray_type = Type::getFunctionType(Type::getVoidType(), {Type::getPointerType(Type::getIntType()), Type::getPointerType(Type::getFloatType())});
        auto f_putfarray = pModule->createFunction("putfarray", putfarray_type);
        symbols.insert("putfarray", f_putfarray);
        // auto putint_entry = f_putint->addBasicBlock("putfloat_entry");
        // auto putint_arg = putint_entry->createArgument(Type::getPointerType(Type::getIntType()));
        // create function:starttime
        auto starttime_type = Type::getFunctionType(Type::getVoidType());
        auto f_starttime = pModule->createFunction("starttime", starttime_type);
        symbols.insert("starttime", f_starttime);
        // create function:stoptime
        auto stoptime_type = Type::getFunctionType(Type::getVoidType());
        auto f_stoptime = pModule->createFunction("stoptime", stoptime_type);
        symbols.insert("stoptime", f_stoptime);
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
                if (init && isa<ConstantValue>(init))
                {
                    Type *btype = type->as<PointerType>()->getBaseType();
                    if (btype->isInt() && init->isFloat())
                        init = ConstantValue::get((int)dynamic_cast<ConstantValue *>(init)->getDouble());
                    else if (btype->isFloat() && init->isInt())
                        init = ConstantValue::get((double)dynamic_cast<ConstantValue *>(init)->getInt());
                }
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
                        if (ctx->btype()->INT() && value->isFloat())
                            value = ConstantValue::get((int)dynamic_cast<ConstantValue *>(value)->getDouble());
                        else if (ctx->btype()->FLOAT() && value->isInt())
                            value = ConstantValue::get((double)dynamic_cast<ConstantValue *>(value)->getInt());
                    }
                    else if (alloca->getType()->as<PointerType>()->getBaseType()->isInt() && value->getType()->isFloat())
                        value = builder.createFtoIInst(value);
                    else if (alloca->getType()->as<PointerType>()->getBaseType()->isFloat() && value->getType()->isInt())
                        value = builder.createIToFInst(value);
                    auto store = builder.createStoreInst(value, alloca);
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
                if (current_type->isInt() && value->isFloat())
                    value = ConstantValue::get((int)dynamic_cast<ConstantValue *>(value)->getDouble());
                else if (current_type->isFloat() && value->isInt())
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
                while (d >= 0 && current_alloca->getDim(d)->isInt() && n >= dynamic_cast<ConstantValue *>(current_alloca->getDim(d))->getInt())
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
                while (d >= 0 && current_global->getDim(d)->isInt() && n >= dynamic_cast<ConstantValue *>(current_global->getDim(d))->getInt())
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
                while (d >= 0 && current_alloca->getDim(d)->isInt() && n >= dynamic_cast<ConstantValue *>(current_alloca->getDim(d))->getInt())
                    n = path[--d] + 1;
            else
                while (d >= 0 && current_global->getDim(d)->isInt() && n >= dynamic_cast<ConstantValue *>(current_global->getDim(d))->getInt())
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
                if (rhs->isInt())
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
        bool const_indices = true;
        for (auto exp : ctx->lValue()->exp())
        {
            auto indice = any_cast<Value *>(exp->accept(this));
            if (!isa<ConstantValue>(indice))
                const_indices = false;
            indices.push_back(indice);
        }
        if (isa<GlobalValue>(value))
        {
            auto global_val = dynamic_cast<GlobalValue *>(value);
            if (global_val->isconst() && const_indices)
            {
                if (global_val->getNumDims() == 0)
                    value = global_val->init();
                else if (global_val->isInt())
                    value = ConstantValue::get(global_val->getInt(indices));
                else
                    value = ConstantValue::get(global_val->getDouble(indices));
                dynamic_cast<ConstantValue *>(value)->setLvalue(true);
            }
            else
                value = builder.createLoadInst(value, indices);
        }
        else if (isa<AllocaInst>(value))
        {
            auto alloca_inst = dynamic_cast<AllocaInst *>(value);
            if (alloca_inst->Const() && const_indices)
            {
                if (alloca_inst->isInt())
                    value = ConstantValue::get(alloca_inst->getInt(indices));
                else
                    value = ConstantValue::get(alloca_inst->getDouble(indices));
                dynamic_cast<ConstantValue *>(value)->setLvalue(true);
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
        if (isa<CallInst>(rhs) && !isa<ConstantValue>(lhs))
            // 将lhs设为保护变量,偏移为0
            dynamic_cast<Instruction *>(lhs)->set_protect();
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
        // create convert instruction if needed
        auto lhsTy = lhs->getType();
        auto rhsTy = rhs->getType();
        auto type = getArithmeticResultType(lhsTy, rhsTy);
        if (lhsTy != type && lconst)
        {
            bool flag = dynamic_cast<ConstantValue *>(lhs)->IsLvalue();
            if (lhs->isInt())
                lhs = ConstantValue::get((double)(dynamic_cast<ConstantValue *>(lhs)->getInt()));
            dynamic_cast<ConstantValue *>(lhs)->setLvalue(flag);
        }
        else if (lhsTy != type)
            lhs = builder.createIToFInst(lhs);
        if (rhsTy != type && rconst)
        {
            bool flag = dynamic_cast<ConstantValue *>(rhs)->IsLvalue();
            if (rhs->isInt())
                rhs = ConstantValue::get((double)(dynamic_cast<ConstantValue *>(rhs)->getInt()));
            dynamic_cast<ConstantValue *>(rhs)->setLvalue(flag);
        }
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
        if (isa<CallInst>(rhs) && !isa<ConstantValue>(lhs))
            // 将lhs设为保护变量,偏移为0
            dynamic_cast<Instruction *>(lhs)->set_protect();
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
        // create convert instruction if needed
        auto lhsTy = lhs->getType();
        auto rhsTy = rhs->getType();
        auto type = getArithmeticResultType(lhsTy, rhsTy);
        if (lhsTy != type && lconst)
        {
            bool flag = dynamic_cast<ConstantValue *>(lhs)->IsLvalue();
            if (lhs->isInt())
                lhs = ConstantValue::get((double)(dynamic_cast<ConstantValue *>(lhs)->getInt()));
            dynamic_cast<ConstantValue *>(lhs)->setLvalue(flag);
        }
        else if (lhsTy != type)
            lhs = builder.createIToFInst(lhs);
        if (rhsTy != type && rconst)
        {
            bool flag = dynamic_cast<ConstantValue *>(rhs)->IsLvalue();
            if (rhs->isInt())
                rhs = ConstantValue::get((double)(dynamic_cast<ConstantValue *>(rhs)->getInt()));
            dynamic_cast<ConstantValue *>(rhs)->setLvalue(flag);
        }
        else if (rhsTy != type)
            rhs = builder.createIToFInst(rhs);
        // create the arithmetic instruction
        Value *result = nullptr;
        if (ctx->MUL())
        {
            if (lconst && rconst)
            {
                result = type->isInt() ? ConstantValue::get(lint * rint) : ConstantValue::get(ldouble * rdouble);
                if (dynamic_cast<ConstantValue *>(lhs)->IsLvalue() || dynamic_cast<ConstantValue *>(rhs)->IsLvalue())
                    dynamic_cast<ConstantValue *>(result)->setLvalue(true);
            }
            else
                result = type->isInt() ? builder.createMulInst(lhs, rhs)
                                       : builder.createFMulInst(lhs, rhs);
        }
        else if (ctx->DIV())
        {
            if (lconst && rconst)
            {
                result = type->isInt() ? ConstantValue::get(lint / rint) : ConstantValue::get(ldouble / rdouble);
                if (dynamic_cast<ConstantValue *>(lhs)->IsLvalue() || dynamic_cast<ConstantValue *>(rhs)->IsLvalue())
                    dynamic_cast<ConstantValue *>(result)->setLvalue(true);
            }
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
        if (value)
        {
            if (func->getReturnType()->isInt() && value->isFloat())
            {
                if (isa<ConstantValue>(value))
                    value = ConstantValue::get((int)dynamic_cast<ConstantValue *>(value)->getDouble());
                else
                    value = builder.createFtoIInst(value);
            }
            else if (func->getReturnType()->isFloat() && value->isInt())
            {
                if (isa<ConstantValue>(value))
                    value = ConstantValue::get((double)dynamic_cast<ConstantValue *>(value)->getInt());
                else
                    value = builder.createIToFInst(value);
            }
        }
        Value *result = builder.createReturnInst(value, current_block);
        return result;
    }

    any SysYIRGenerator::visitCall(SysYParser::CallContext *ctx)
    {
        auto funcName = ctx->ID()->getText();
        auto func = dyncast<Function>(symbols.lookup(funcName));
        auto parent_func = builder.getBasicBlock()->getParent();
        assert(func);
        vector<Value *> args;
        int arg_num = 0;
        int last_call_num = 0;
        if (funcName == "starttime" || funcName == "stoptime")
        {
            int line = ctx->getStart()->getLine();
            args.push_back(ConstantValue::get(line));
        }
        if (auto rArgs = ctx->funcRParams())
        {
            auto iter = func->getParamTypes().begin();
            for (auto exp : rArgs->exp())
            {
                Type *arg_type = (*iter)->as<PointerType>()->getBaseType();
                Value *arg = any_cast<Value *>(exp->accept(this));
                // 记录最后出现的类型为callinst的参数的标号
                if (isa<CallInst>(arg))
                    last_call_num = arg_num;
                // 对参数进行类型转换
                if (isa<ConstantValue>(arg))
                {
                    int id = parent_func->allocateVariableID();
                    if (arg_type->isInt() && arg->isFloat())
                        arg = ConstantValue::get((int)dynamic_cast<ConstantValue *>(arg)->getDouble());
                    else if (arg_type->isFloat() && arg->isInt())
                        arg = ConstantValue::get((double)dynamic_cast<ConstantValue *>(arg)->getInt());
                }
                else if (arg_type->isInt() && arg->isFloat())
                    arg = builder.createFtoIInst(arg);
                else if (arg_type->isFloat() && arg->isInt())
                    arg = builder.createIToFInst(arg);
                // 第5个参数开始,设置为直接存到栈中,并记录偏移
                if (arg_num >= 4 && !isa<ConstantValue>(arg))
                    dynamic_cast<Instruction *>(arg)->set_pass(4 * (arg_num - 4));
                arg_num++;
                iter++;
                args.push_back(arg);
            }
            // 对于前四个参数,记录需要保护的非const参数,并设置偏移
            int offset = 0;
            last_call_num = last_call_num > 4 ? 4 : last_call_num;
            for (int i = 0; i < last_call_num; i++)
            {
                auto arg = args[i];
                if (!isa<ConstantValue>(arg))
                {
                    // 将arg设为保护变量,并记录相对第一个保护变量的偏移
                    dynamic_cast<Instruction *>(arg)->set_protect(offset);
                    offset += 4;
                }
            }
        }
        Value *call = builder.createCallInst(func, args);
        dynamic_cast<Instruction *>(call)->set_protect_cnt(last_call_num);
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
            builder.push_truetarget(thenblock);
            builder.push_falsetarget(exitblock);
            auto cond = any_cast<Value *>(ctx->exp()->accept(this));
            // cond->setName(flagname);
            current_block = builder.getBasicBlock();
            auto true_target = builder.get_truetarget();
            auto false_target = builder.get_falsetarget();
            current_block->getSuccessors().push_back(true_target);
            current_block->getSuccessors().push_back(false_target);
            true_target->getPredecessors().push_back(current_block);
            false_target->getPredecessors().push_back(current_block);
            Value *CondBr = builder.createCondBrInst(cond, true_target, false_target, vector<Value *>(), vector<Value *>());
            true_target->setDepth(current_block->getDepth() + 1);
            false_target->setDepth(current_block->getDepth() + 1);
            builder.poptarget();
            builder.setPosition(thenblock, thenblock->begin());
            visitStmt(ctx->stmt(0));
            current_block = builder.getBasicBlock();
            current_block->getSuccessors().push_back(exitblock);
            exitblock->getPredecessors().push_back(current_block);
            Value *then_br = builder.createUncondBrInst(exitblock, vector<Value *>());
            exitblock->setDepth(current_block->getDepth() + 1);
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
            // generate condition expression
            builder.push_truetarget(thenblock);
            builder.push_falsetarget(elseblock);
            auto cond = any_cast<Value *>(ctx->exp()->accept(this));
            current_block = builder.getBasicBlock();
            auto true_target = builder.get_truetarget();
            auto false_target = builder.get_falsetarget();
            current_block->getSuccessors().push_back(true_target);
            current_block->getSuccessors().push_back(false_target);
            true_target->getPredecessors().push_back(current_block);
            false_target->getPredecessors().push_back(current_block);
            CondBrInst *CondBr = builder.createCondBrInst(cond, true_target, false_target, vector<Value *>(), vector<Value *>());
            true_target->setDepth(current_block->getDepth() + 1);
            false_target->setDepth(current_block->getDepth() + 1);
            builder.poptarget();
            builder.setPosition(thenblock, thenblock->begin());
            visitStmt(ctx->stmt(0));
            current_block = builder.getBasicBlock();
            current_block->getSuccessors().push_back(exitblock);
            exitblock->getPredecessors().push_back(current_block);
            Value *then_br = builder.createUncondBrInst(exitblock, vector<Value *>());
            exitblock->setDepth(current_block->getDepth() + 1);
            builder.setPosition(elseblock, elseblock->begin());
            visitStmt(ctx->stmt(1));
            current_block = builder.getBasicBlock();
            current_block->getSuccessors().push_back(exitblock);
            exitblock->getPredecessors().push_back(current_block);
            Value *else_br = builder.createUncondBrInst(exitblock, vector<Value *>());
            exitblock->setDepth(current_block->getDepth() + 1);
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
        current_block->getSuccessors().push_back(headerblock);
        headerblock->getPredecessors().push_back(current_block);
        Value *head_uncondbr = builder.createUncondBrInst(headerblock, vector<Value *>{});
        headerblock->setDepth(current_block->getDepth() + 1);
        builder.setPosition(headerblock, headerblock->begin());
        // uncondbr:current->header
        // Value *Current_uncondbr = builder.createUncondBrInst(headerblock, vector<Value *>());
        // create body basicblock
        char bodyname[20];
        sprintf(bodyname, "body%d", builder.get_whilecnt());
        auto bodyblock = func->addBasicBlock(bodyname);
        // create exit basicblock
        char exitname[20];
        sprintf(exitname, "exit%d", builder.get_whilecnt() + builder.get_ifcnt());
        auto exitblock = func->addExitBlock(exitname);
        // push header&exit into loopstack
        builder.pushheader(headerblock);
        builder.pushexit(exitblock);
        // generate condition expression
        builder.push_truetarget(bodyblock);
        builder.push_falsetarget(exitblock);
        auto cond = any_cast<Value *>(ctx->exp()->accept(this));
        // create condbr in header
        current_block = builder.getBasicBlock();
        auto true_target = builder.get_truetarget();
        auto false_target = builder.get_falsetarget();
        current_block->getSuccessors().push_back(true_target);
        current_block->getSuccessors().push_back(false_target);
        true_target->getPredecessors().push_back(current_block);
        false_target->getPredecessors().push_back(current_block);
        Value *header_condbr = builder.createCondBrInst(cond, true_target, false_target, vector<Value *>(), vector<Value *>());
        true_target->setDepth(current_block->getDepth() + 1);
        false_target->setDepth(current_block->getDepth() + 1);
        builder.poptarget();
        // generate code in body block
        builder.setPosition(bodyblock, bodyblock->begin());
        visitStmt(ctx->stmt());
        // pop header&exit from loopstack
        builder.poploop();
        // create uncondbr in body block
        current_block = builder.getBasicBlock();
        current_block->getSuccessors().push_back(headerblock);
        headerblock->getPredecessors().push_back(current_block);
        Value *body_uncondbr = builder.createUncondBrInst(headerblock, vector<Value *>());
        exitblock->setDepth(current_block->getDepth() + 1);
        // setup the instruction insert position
        builder.setPosition(exitblock, exitblock->begin());
        func->moveExitBlock();
        return builder.getBasicBlock();
    }
    any SysYIRGenerator::visitBreakStmt(SysYParser::BreakStmtContext *ctx)
    {
        auto current_block = builder.getBasicBlock();
        auto exit_block = builder.getExit();
        current_block->getSuccessors().push_back(exit_block);
        exit_block->getPredecessors().push_back(current_block);
        Value *uncondbr = builder.createUncondBrInst(exit_block, vector<Value *>());
        exit_block->setDepth(current_block->getDepth() + 1);
        return uncondbr;
    }
    any SysYIRGenerator::visitContinueStmt(SysYParser::ContinueStmtContext *ctx)
    {
        auto current_block = builder.getBasicBlock();
        auto header_block = builder.getHeader();
        current_block->getSuccessors().push_back(header_block);
        header_block->getPredecessors().push_back(current_block);
        Value *uncondbr = builder.createUncondBrInst(header_block, vector<Value *>());
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
                if (hs->isInt())
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
        if (isa<CallInst>(rhs) && !isa<ConstantValue>(lhs))
            // 将lhs设为保护变量,偏移为0
            dynamic_cast<Instruction *>(lhs)->set_protect();
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
        // create convert instruction if needed
        auto lhsTy = lhs->getType();
        auto rhsTy = rhs->getType();
        auto type = getArithmeticResultType(lhsTy, rhsTy);
        if (lhsTy != type && lconst)
        {
            if (lhs->isInt())
                lhs = ConstantValue::get((double)(dynamic_cast<ConstantValue *>(lhs)->getInt()));
        }
        else if (lhsTy != type)
            lhs = builder.createIToFInst(lhs);
        if (rhsTy != type && rconst)
        {
            if (rhs->isInt())
                rhs = ConstantValue::get((double)(dynamic_cast<ConstantValue *>(rhs)->getInt()));
        }
        else if (rhsTy != type)
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
        if (isa<CallInst>(rhs) && !isa<ConstantValue>(lhs))
            // 将lhs设为保护变量,偏移为0
            dynamic_cast<Instruction *>(lhs)->set_protect();
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
        builder.push_truetarget(rhs_block);
        builder.push_falsetarget(builder.get_falsetarget());
        // get lhs value
        auto lhs = any_cast<Value *>(ctx->exp(0)->accept(this));
        // create condbr instr
        current_block = builder.getBasicBlock();
        auto true_target = builder.get_truetarget();
        auto false_target = builder.get_falsetarget();
        current_block->getSuccessors().push_back(true_target);
        current_block->getSuccessors().push_back(false_target);
        true_target->getPredecessors().push_back(current_block);
        false_target->getPredecessors().push_back(current_block);
        Value *condbr = builder.createCondBrInst(lhs, true_target, false_target, vector<Value *>(), vector<Value *>());
        true_target->setDepth(current_block->getDepth() + 1);
        false_target->setDepth(current_block->getDepth() + 1);
        builder.poptarget();
        // generate code for rhs block
        builder.setPosition(rhs_block, rhs_block->begin());
        return (ctx->exp(1)->accept(this));
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
        builder.push_truetarget(builder.get_truetarget());
        builder.push_falsetarget(rhs_block);
        // get lhs value
        auto lhs = any_cast<Value *>(ctx->exp(0)->accept(this));
        // create condbr instr
        current_block = builder.getBasicBlock();
        auto true_target = builder.get_truetarget();
        auto false_target = builder.get_falsetarget();
        current_block->getSuccessors().push_back(true_target);
        current_block->getSuccessors().push_back(false_target);
        true_target->getPredecessors().push_back(current_block);
        false_target->getPredecessors().push_back(current_block);
        Value *condbr = builder.createCondBrInst(lhs, true_target, false_target, vector<Value *>(), vector<Value *>());
        true_target->setDepth(current_block->getDepth() + 1);
        false_target->setDepth(current_block->getDepth() + 1);
        builder.poptarget();
        // generate code for rhs block
        builder.setPosition(rhs_block, rhs_block->begin());
        return (ctx->exp(1)->accept(this));
    }
    any SysYIRGenerator::visitParenExp(SysYParser::ParenExpContext *ctx)
    {
        return ctx->exp()->accept(this);
    }
    // LoadCut
    void LoadCut::print_KILL_GEN(std::ostream &os)
    {
        auto functions = OriginModule->getFunctions();
        for (auto fiter = functions->begin(); fiter != functions->end(); fiter++)
        {
            Function *func = fiter->second;
            if (func->getName() != "getint" && func->getName() != "getch" && func->getName() != "getfloat" && func->getName() != "getarray" && func->getName() != "getfarray" && func->getName() != "putint" && func->getName() != "putch" && func->getName() != "putfloat" && func->getName() != "putarray" && func->getName() != "putfarray" && func->getName() != "starttime" && func->getName() != "stoptime" && func->getName() != "putf")
            {
                auto bblist = func->getBasicBlocks();
                os << "**********" << func->getName() << "**********"
                   << "\n";
                for (auto biter = bblist.begin(); biter != bblist.end(); biter++)
                {
                    auto bb = biter->get();
                    os << "$$" << bb->getName() << "$$"
                       << "\n";
                    os << "[KILL]"
                       << "\n";
                    for (auto &k : bb->kill)
                    {
                        os << " " << k.first->getName();
                    }
                    os << "\n";
                    os << "[GEN]"
                       << "\n";
                    for (auto &g : bb->gen)
                    {
                        os << " " << g.second.first->getName() << "->" << g.first->getName();
                    }
                    os << "\n";
                }
            }
        }
    }
    void LoadCut::print_IN_OUT(std::ostream &os)
    {
        auto functions = OriginModule->getFunctions();
        for (auto fiter = functions->begin(); fiter != functions->end(); fiter++)
        {
            Function *func = fiter->second;
            auto bblist = func->getBasicBlocks();
            if (func->getName() != "getint" && func->getName() != "getch" && func->getName() != "getfloat" && func->getName() != "getarray" && func->getName() != "getfarray" && func->getName() != "putint" && func->getName() != "putch" && func->getName() != "putfloat" && func->getName() != "putarray" && func->getName() != "putfarray" && func->getName() != "starttime" && func->getName() != "stoptime" && func->getName() != "putf")
            {
                os << "**********" << func->getName() << "**********"
                   << "\n";
                for (auto biter = bblist.begin(); biter != bblist.end(); biter++)
                {
                    auto bb = biter->get();
                    os << "$$" << bb->getName() << "$$"
                       << "\n";
                    os << "[IN]"
                       << "\n";
                    for (auto &k : bb->in)
                    {
                        os << " " << k.second.first->getName();
                        auto indices = k.second.second;
                        for (auto indice : indices)
                        {
                            if (isa<ConstantValue>(indice))
                            {
                                os << "[";
                                os << dynamic_cast<ConstantValue *>(indice)->getInt();
                                os << "]";
                            }
                            else
                            {
                                os << "[";
                                os << "%" + indice->getName();
                                os << "]";
                            }
                        }
                        os << "->" << k.first->getName();
                    }
                    os << "\n";
                    os << "[OUT]"
                       << "\n";
                    for (auto &g : bb->out)
                    {
                        os << " " << g.second.first->getName();
                        auto indices = g.second.second;
                        for (auto indice : indices)
                        {
                            if (isa<ConstantValue>(indice))
                            {
                                os << "[";
                                os << dynamic_cast<ConstantValue *>(indice)->getInt();
                                os << "]";
                            }
                            else
                            {
                                os << "[";
                                os << "%" + indice->getName();
                                os << "]";
                            }
                        }
                        os << "->" << g.first->getName();
                    }
                    os << "\n";
                }
            }
        }
    }
    Module *LoadCut::Run()
    {
        auto functions = OriginModule->getFunctions();
        // 判断函数是否有函数调用
        for (auto iter = functions->begin(); iter != functions->end(); iter++)
        {
            Function *func = iter->second;
            havecall[func] = false;
            ModifyGlbvl[func] = false;
            if (func->getBasicBlocks().empty())
                continue;
            AnalyzeFunc(func);
        }
        // 计算kill集,gen集合
        for (auto iter = functions->begin(); iter != functions->end(); iter++)
        {
            Function *func = iter->second;
            hascall = havecall[func];
            auto bblist = func->getBasicBlocks();
            if (bblist.empty())
                continue;
            for (auto iter = bblist.begin(); iter != bblist.end(); iter++)
            {
                auto bb = iter->get();
                CalKill_Gen(bb);
            }
        }
        // 计算In,Out集合
        for (auto iter = functions->begin(); iter != functions->end(); iter++)
        {
            Function *func = iter->second;
            auto bblist = func->getBasicBlocks();
            if (bblist.empty())
                continue;
            CalIn_Out(func);
        }
        // print_IN_OUT(os);
        // 重新生成IR
        RegenerateIR();
        // //print KILL GEN
        // for (auto fiter = functions->begin(); fiter != functions->end(); fiter++)
        // {
        //   Function *func = fiter->second;
        //   auto bblist = func->getBasicBlocks();
        //   std::cout << "**********" << func->getName() << "**********" << std::endl;
        //   for (auto biter = bblist.begin(); biter != bblist.end(); biter++)
        //   {
        //     auto bb = biter->get();
        //     std::cout << "$$" <<  bb->getName() << "$$" << std::endl;
        //     std::cout << "[KILL]" << std::endl;
        //     for(auto &k : bb->kill){
        //       std::cout <<" "<<k.first->getName();
        //     }
        //     std::cout << std::endl;
        //     std::cout << "[GEN]" << std::endl;
        //     for (auto &g : bb->gen){
        //       std::cout << " " << g.second.first->getName() << "->" << g.first->getName();
        //     }
        //     std::cout << std::endl;
        //   }
        // }

        // print IN OUT
        //  for (auto fiter = functions->begin(); fiter != functions->end(); fiter++)
        //  {
        //    Function *func = fiter->second;
        //    auto bblist = func->getBasicBlocks();
        //    std::cout << "**********" << func->getName() << "**********" << std::endl;
        //    for (auto biter = bblist.begin(); biter != bblist.end(); biter++)
        //    {
        //      auto bb = biter->get();
        //      std::cout << "$$" <<  bb->getName() << "$$" << std::endl;
        //      std::cout << "[IN]" << std::endl;
        //      for(auto &k : bb->in){
        //        std::cout <<" "<< k.second.first->getName() << "->" << k.first->getName();
        //      }
        //      std::cout << std::endl;
        //      std::cout << "[OUT]" << std::endl;
        //      for (auto &g : bb->out){
        //        std::cout << " " << g.second.first->getName() << "->" << g.first->getName();
        //      }
        //      std::cout << std::endl;
        //    }
        //  }
        return pModule;
    }
    void LoadCut::CalKill_Gen(BasicBlock *curbb)
    {
        for (auto &instriter : curbb->getInstructions())
        {
            auto instr = instriter.get();
            if (isa<LoadInst>(instr))
            {
                LoadInst *ldInst = dynamic_cast<LoadInst *>(instr);
                auto pointer = ldInst->getPointer();
                auto indices = vector<Value *>(ldInst->getIndices().begin(), ldInst->getIndices().end());
                if (!isa<GlobalValue>(pointer) || !hascall)
                    curbb->gen.push_back({instr, {pointer, indices}});
            }
            else if (isa<StoreInst>(instr))
            {
                StoreInst *stInst = dynamic_cast<StoreInst *>(instr);
                auto value = stInst->getValue();
                auto pointer = stInst->getPointer();
                auto indices = vector<Value *>(stInst->getIndices().begin(), stInst->getIndices().end());
                curbb->kill.push_back({pointer, indices});
                if (isa<Instruction>(value))
                {
                    for (auto iter = curbb->gen.begin(); iter != curbb->gen.end();)
                    {
                        auto ptr = iter->second.first;
                        auto idx = iter->second.second;
                        if (ptr == pointer && idx == indices)
                        {
                            iter = curbb->gen.erase(iter);
                        }
                        else
                        {
                            iter++;
                        }
                    }
                    if (!isa<GlobalValue>(pointer) || !hascall)
                    {
                        Instruction *V = dynamic_cast<Instruction *>(value);
                        curbb->gen.push_back({V, {pointer, indices}});
                    }
                }
            }
            else if (isa<CallInst>(instr))
            {
                auto callInst = dynamic_cast<CallInst *>(instr);
                auto args = callInst->getArguments();
                for (auto iter = args.begin(); iter != args.end(); iter++)
                {
                    if (isa<LoadInst>(*iter))
                    {
                        auto ldInst = dynamic_cast<LoadInst *>(*iter);
                        auto pointer = ldInst->getPointer();
                        auto indices = vector<Value *>(ldInst->getIndices().begin(), ldInst->getIndices().end());
                        int numdims = 0;
                        if (isa<GlobalValue>(pointer))
                            numdims = dynamic_cast<GlobalValue *>(pointer)->getNumDims();
                        else if (isa<AllocaInst>(pointer))
                            numdims = dynamic_cast<AllocaInst *>(pointer)->getNumDims();
                        else if (isa<Argument>(pointer))
                            numdims = dynamic_cast<Argument *>(pointer)->getNumDims();
                        if (indices.size() < numdims)
                        {
                            curbb->killptr.push_back(pointer);
                            for (auto iter = curbb->gen.begin(); iter != curbb->gen.end();)
                            {
                                auto ptr = iter->second.first;
                                auto idx = iter->second.second;
                                if (ptr == pointer)
                                {
                                    iter = curbb->gen.erase(iter);
                                }
                                else
                                {
                                    iter++;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    void LoadCut::AnalyzeFunc(Function *curFunc)
    {
        auto bblist = curFunc->getBasicBlocks();
        // 判断是否修改全局变量
        for (auto iter = bblist.begin(); iter != bblist.end(); iter++)
        {
            BasicBlock *bb = iter->get();
            for (auto &instr : bb->getInstructions())
            {
                auto kind = instr->getKind();
                if (kind == Instruction::kStore)
                {
                    auto stInst = dynamic_cast<StoreInst *>(instr.get());
                    auto pointer = stInst->getPointer();
                    if (isa<GlobalValue>(pointer))
                    {
                        ModifyGlbvl[curFunc] = true;
                        break;
                    }
                }
            }
            if (ModifyGlbvl[curFunc])
                break;
        }
        for (auto iter = bblist.begin(); iter != bblist.end(); iter++)
        {
            auto bb = iter->get();
            for (auto &instr : bb->getInstructions())
            {
                auto instrType = instr->getKind();
                if (instrType == Instruction::Kind::kCall)
                {
                    auto callee = dynamic_cast<CallInst *>(instr.get())->getCallee();
                    if (libfunc.find(callee->getName()) == libfunc.end())
                    {
                        havecall[curFunc] = true;
                        break;
                    }
                }
            }
            if (havecall[curFunc])
                break;
        }
    }
    void LoadCut::CalIn_Out(Function *curFunc)
    {
        bool change = true;
        auto bblist = curFunc->getBasicBlocks();
        if (bblist.empty())
            return;
        int k = 0;
        while (change)
        {
            k++;
            // printf("%d\t", k);
            change = false;
            for (auto iter = bblist.begin(); iter != bblist.end(); iter++)
            {
                BasicBlock *bb = iter->get();
                set<pair<Instruction *, pair<Value *, vector<Value *>>>> oldin, oldout, temp, t;
                // copy(bb->in.begin(), bb->in.end(), oldin.begin());
                for (auto item : bb->in)
                {
                    oldin.insert(item);
                }
                // copy(bb->out.begin(), bb->out.end(), oldout.begin());
                for (auto item : bb->out)
                {
                    oldout.insert(item);
                }
                bb->in.clear();
                bb->out.clear();
                // cal IN
                auto pres = bb->getPredecessors();
                if (!pres.empty())
                {
                    for (int i = 0; i < pres.size(); i++)
                    {
                        if (i == 0)
                        {
                            // copy(pres[0]->out.begin(), pres[0]->out.end(), temp.begin());
                            for (auto item : pres[0]->out)
                            {
                                temp.insert(item);
                            }
                        }
                        else
                        {
                            // for (auto ii : temp)
                            //   for (auto jj : pres[i]->in)
                            //     if (ii.first == jj.first && ii.second.first == jj.second.first && ii.second.second == jj.second.second)
                            //       t.insert(ii);
                            set_intersection(temp.begin(), temp.end(), pres[i]->out.begin(), pres[i]->out.end(), inserter(t, t.begin()));
                            temp.clear();
                            for (auto item : t)
                            {
                                temp.insert(item);
                            }
                            t.clear();
                        }
                    }
                }
                // copy(temp.begin(), temp.end(), bb->in.begin());
                for (auto item : temp)
                {
                    bb->in.insert(item);
                }
                // cal OUT
                for (auto &item : bb->in)
                {
                    bool flag = true;
                    auto ptr = item.second.first;
                    auto idx = item.second.second;
                    for (auto kill : bb->kill)
                    {
                        if (ptr == kill.first && idx == kill.second)
                            flag = false;
                    }
                    for (auto killptr : bb->killptr)
                    {
                        if (ptr == killptr)
                            flag = false;
                    }
                    if (flag)
                    {
                        bb->out.insert(item);
                    }
                }
                for (auto &item : bb->gen)
                {
                    bb->out.insert(item);
                }
                // Check if changed
                if (oldin != bb->in || oldout != bb->out)
                    change = true;
            }

            // print in out
            //  for(auto iter = bblist.begin(); iter != bblist.end(); iter++)
            //  {
            //    BasicBlock* bb = iter->get();
            //    std::cout << bb->getName() << endl;
            //  }
        }
    }
    bool LoadCut::BBCmp(BasicBlock *a, BasicBlock *b)
    {
        return a->getDepth() < b->getDepth();
    }
    void LoadCut::OrderBasicBlock(Function *oldFunc, Function *myFunc)
    {
        // 按照深度给基本块排序
        vector<BasicBlock *> bbs;
        auto bblist = oldFunc->getBasicBlocks();
        for (auto iter = bblist.begin(); iter != bblist.end(); iter++)
            bbs.push_back(iter->get());
        std::sort(bbs.begin(), bbs.end(), BBCmp);
        for (auto oldbb : bbs)
        {
            auto mybb = myFunc->addBasicBlock(oldbb->getName());
            mybb->setDepth(oldbb->getDepth());
            oldbb->setAlter(mybb);
        }
        // 按照宽度优先遍历基本块
        // std::queue<BasicBlock *> bbq;
        // bbq.push(curbb);
        // BasicBlock *mybb;
        // while (!bbq.empty())
        // {
        //   curbb = bbq.front();
        //   bbq.pop();
        //   if (bbs.find(curbb) != bbs.end())
        //     continue;
        //   mybb = curFunc->addBasicBlock(curbb->getName());
        //   curbb->setAlter(mybb);
        //   bbs.insert(curbb);
        //   for (auto Sucbb : curbb->getSuccessors())
        //     bbq.push(Sucbb);
        // }
    }
    void LoadCut::RegenerateIR()
    {
        // 生成全局变量
        auto global_values = OriginModule->getGlobalValues();
        for (auto iter = global_values->begin(); iter != global_values->end(); iter++)
        {
            GlobalValue *glbvl = iter->second;
            auto name = glbvl->getName();
            auto type = glbvl->getType();
            pModule->addGlobalValue(glbvl);
        }
        // 生成函数
        auto functions = OriginModule->getFunctions();
        for (auto iter = functions->begin(); iter != functions->end(); iter++)
        {
            Function *func = iter->second;
            hascall = havecall[func];
            // os << func->getName() << ":";
            // if (hascall)
            //   os << "hascall\n";
            // else
            //   os << "nocall\n";
            Function *myFunc = pModule->createFunction(func->getName(), func->getType());
            auto bblist = func->getBasicBlocks();
            if (bblist.empty())
                continue;
            OrderBasicBlock(func, myFunc);
            auto entry_block = func->getEntryBlock();
            auto my_entry = myFunc->getEntryBlock();
            auto entry_args = entry_block->getArguments();
            for (auto i = entry_args.begin(); i != entry_args.end(); i++)
            {
                auto arg = i->get();
                auto my_arg = my_entry->createArgument(arg->getType(), vector<int>(arg->getDims().begin(), arg->getDims().end()), arg->getName());
                arg->setAlter(my_arg);
            }
            for (auto iter = bblist.begin(); iter != bblist.end(); iter++)
            {
                auto bb = iter->get();
                auto mybb = dynamic_cast<BasicBlock *>(bb->getAlter());
                builder.setPosition(mybb, mybb->end());
                // 初始化AVALUE,RVALUE
                AVALUE.clear();
                RVALUE.clear();
                // 将In集合中每个虚寄存器-变量对记录在AVALUE中
                for (auto p : bb->in)
                {
                    Instruction *instr = p.first;
                    if (!instr->hasAlter())
                        continue;
                    Value *value = p.second.first;
                    vector<Value *> indices = p.second.second;
                    AVALUE[value][indices] = instr;
                    RVALUE[instr].insert({value, indices});
                }
                // 扫描基本块的全部指令
                for (auto p : bb->getPredecessors())
                    mybb->getPredecessors().push_back(dynamic_cast<BasicBlock *>(p->getAlter()));
                for (auto s : bb->getSuccessors())
                    mybb->getSuccessors().push_back(dynamic_cast<BasicBlock *>(s->getAlter()));
                for (auto iter = bb->begin(); iter != bb->end(); iter++)
                {
                    auto instr = iter->get();
                    if (isa<LoadInst>(instr))
                    {
                        auto ldInst = dynamic_cast<LoadInst *>(instr);
                        auto pointer = ldInst->getPointer();
                        auto indices = vector<Value *>(ldInst->getIndices().begin(), ldInst->getIndices().end());
                        // 如果变量在AVALUE中
                        if (AVALUE.find(pointer) != AVALUE.end() && AVALUE[pointer].find(indices) != AVALUE[pointer].end())
                        {
                            // RVALUE[instr] = pair<Value *, vector<Value *>>(pointer, indices);
                            RVALUE[instr].insert({pointer, indices});
                        }
                        // 如果变量不在AVALUE中
                        else
                        {
                            vector<Value *> my_indices;
                            for (int i = 0; i < indices.size(); i++)
                            {
                                if (RVALUE.find(indices[i]) != RVALUE.end() && !RVALUE[indices[i]].empty())
                                {
                                    auto pointer = RVALUE[indices[i]].begin()->first;
                                    auto indices1 = RVALUE[indices[i]].begin()->second;
                                    my_indices.push_back(AVALUE[pointer][indices1]->getAlter());
                                }
                                else
                                    my_indices.push_back(indices[i]->getAlter());
                            }
                            auto my_ldInst = builder.createLoadInst(pointer->getAlter(), my_indices);
                            ldInst->setAlter(my_ldInst);
                            if (!isa<GlobalValue>(pointer) || !hascall)
                            {
                                AVALUE[pointer][indices] = instr;
                                RVALUE[ldInst].insert({pointer, indices});
                            }
                        }
                    }
                    else if (isa<StoreInst>(instr))
                    {
                        auto stInst = dynamic_cast<StoreInst *>(instr);
                        auto value = stInst->getValue();
                        auto pointer = stInst->getPointer();
                        auto indices = vector<Value *>(stInst->getIndices().begin(), stInst->getIndices().end());
                        // 将变量原来所在虚拟寄存器（如有）对应该变量的项去掉
                        if (AVALUE.find(pointer) != AVALUE.end() && AVALUE[pointer].find(indices) != AVALUE[pointer].end())
                        {
                            auto pre_Value = AVALUE[pointer][indices];
                            RVALUE[pre_Value].erase(RVALUE[pre_Value].find({pointer, indices}));
                        }
                        if (isa<ConstantValue>(value))
                        {
                            // 变量现在不在任何一个虚拟寄存器中
                            auto iter1 = AVALUE.find(pointer);
                            if (iter1 != AVALUE.end())
                            {
                                auto iter2 = AVALUE[pointer].find(indices);
                                if (iter2 != AVALUE[pointer].end())
                                    AVALUE[pointer].erase(iter2);
                            }
                            vector<Value *> my_indices;
                            for (int i = 0; i < indices.size(); i++)
                            {
                                if (RVALUE.find(indices[i]) != RVALUE.end() && !RVALUE[indices[i]].empty())
                                {
                                    // auto ldInst = dynamic_cast<LoadInst *>(indices[i]);
                                    auto pointer = RVALUE[indices[i]].begin()->first;
                                    auto indices1 = RVALUE[indices[i]].begin()->second;
                                    my_indices.push_back(AVALUE[pointer][indices1]->getAlter());
                                }
                                else
                                    my_indices.push_back(indices[i]->getAlter());
                            }
                            auto my_stInst = builder.createStoreInst(value->getAlter(), pointer->getAlter(), my_indices);
                            continue;
                        }
                        if (RVALUE.find(value) != RVALUE.end() && !RVALUE[value].empty())
                        {
                            auto pre_pointer = RVALUE[value].begin()->first;
                            auto pre_indices = RVALUE[value].begin()->second;
                            auto Vvalue = AVALUE[pre_pointer][pre_indices];
                            vector<Value *> my_indices;
                            for (int i = 0; i < indices.size(); i++)
                            {
                                if (RVALUE.find(indices[i]) != RVALUE.end() && !RVALUE[indices[i]].empty())
                                {
                                    // auto ldInst = dynamic_cast<LoadInst *>(indices[i]);
                                    auto pointer = RVALUE[indices[i]].begin()->first;
                                    auto indices1 = RVALUE[indices[i]].begin()->second;
                                    my_indices.push_back(AVALUE[pointer][indices1]->getAlter());
                                }
                                else
                                    my_indices.push_back(indices[i]->getAlter());
                            }
                            auto my_stInst = builder.createStoreInst(Vvalue->getAlter(), pointer->getAlter(), my_indices);
                            if (!isa<GlobalValue>(pointer) || !hascall)
                            {
                                RVALUE[Vvalue].insert({pointer, indices});
                                AVALUE[pointer][indices] = Vvalue;
                            }
                        }
                        else
                        {
                            if (!isa<GlobalValue>(pointer) || !hascall)
                            {
                                RVALUE[value].insert({pointer, indices});
                                AVALUE[pointer][indices] = dynamic_cast<Instruction *>(value);
                            }
                            vector<Value *> my_indices;
                            for (int i = 0; i < indices.size(); i++)
                            {
                                if (RVALUE.find(indices[i]) != RVALUE.end() && !RVALUE[indices[i]].empty())
                                {
                                    // auto ldInst = dynamic_cast<LoadInst *>(indices[i]);
                                    auto pointer = RVALUE[indices[i]].begin()->first;
                                    auto indices1 = RVALUE[indices[i]].begin()->second;
                                    my_indices.push_back(AVALUE[pointer][indices1]->getAlter());
                                }
                                else
                                    my_indices.push_back(indices[i]->getAlter());
                            }
                            auto my_stInst = builder.createStoreInst(value->getAlter(), pointer->getAlter(), my_indices);
                        }
                    }
                    else if (isa<BinaryInst>(instr))
                    {
                        auto bInst = dynamic_cast<BinaryInst *>(instr);
                        auto lhs = bInst->getLhs();
                        auto rhs = bInst->getRhs();
                        if (RVALUE.find(lhs) != RVALUE.end() && !RVALUE[lhs].empty())
                        {
                            auto pointer = RVALUE[lhs].begin()->first;
                            auto indices = RVALUE[lhs].begin()->second;
                            lhs = AVALUE[pointer][indices];
                        }
                        if (RVALUE.find(rhs) != RVALUE.end() && !RVALUE[rhs].empty())
                        {
                            auto pointer = RVALUE[rhs].begin()->first;
                            auto indices = RVALUE[rhs].begin()->second;
                            rhs = AVALUE[pointer][indices];
                        }
                        auto my_bInst = builder.createBinaryInst(bInst->getKind(), bInst->getType(), lhs->getAlter(), rhs->getAlter());
                        bInst->setAlter(my_bInst);
                    }
                    else if (isa<UnaryInst>(instr))
                    {
                        auto uInst = dynamic_cast<UnaryInst *>(instr);
                        auto hs = uInst->getOperand();
                        if (RVALUE.find(hs) != RVALUE.end())
                        {
                            auto pointer = RVALUE[hs].begin()->first;
                            auto indices = RVALUE[hs].begin()->second;
                            hs = AVALUE[pointer][indices];
                        }
                        auto my_uInst = builder.createUnaryInst(uInst->getKind(), uInst->getType(), hs->getAlter());
                        uInst->setAlter(my_uInst);
                    }
                    else if (isa<CallInst>(instr))
                    {
                        auto callInst = dynamic_cast<CallInst *>(instr);
                        auto args = callInst->getArguments();
                        auto callee = callInst->getCallee();
                        vector<Value *> my_args;
                        set<Value *> ruined; // 数组参数,则其原来的值可能被破坏
                        for (auto iter = args.begin(); iter != args.end(); iter++)
                        {
                            if (RVALUE.find(*iter) != RVALUE.end() && !RVALUE[*iter].empty())
                            {
                                auto ldInst = dynamic_cast<LoadInst *>(*iter);
                                auto pointer = RVALUE[*iter].begin()->first;
                                auto indices = RVALUE[*iter].begin()->second;
                                my_args.push_back(AVALUE[pointer][indices]->getAlter());
                                int numdims = 0;
                                if (isa<GlobalValue>(pointer))
                                    numdims = dynamic_cast<GlobalValue *>(pointer)->getNumDims();
                                else if (isa<AllocaInst>(pointer))
                                    numdims = dynamic_cast<AllocaInst *>(pointer)->getNumDims();
                                else if (isa<Argument>(pointer))
                                    numdims = dynamic_cast<Argument *>(pointer)->getNumDims();
                                if (indices.size() < numdims)
                                    ruined.insert(pointer);
                            }
                            else
                                my_args.push_back(iter->getAlter());
                        }
                        for (auto pointer : ruined)
                        {
                            auto iter = AVALUE.find(pointer);
                            if (iter != AVALUE.end())
                                AVALUE.erase(iter);
                        }
                        // for (auto iter1 = RVALUE.begin(); iter1 != RVALUE.end(); iter1++)
                        // {
                        //     auto Vvalue = iter1->first;
                        //     auto values = iter1->second;
                        //     for (auto iter2 = values.begin(); iter2 != values.end();)
                        //     {
                        //         auto pointer = iter2->first;
                        //         auto indices = iter2->second;
                        //         if (ruined.find(pointer) != ruined.end())
                        //             iter2 = RVALUE[Vvalue].erase(RVALUE[Vvalue].find({pointer, indices}));
                        //         else
                        //             iter2++;
                        //     }
                        // }
                        // if (libfunc.find(callee->getName()) == libfunc.end())
                        // {
                        //   // 函数调用删除AVALUE中的全局变量
                        //   for (auto iter1 = AVALUE.begin(); iter1 != AVALUE.end();)
                        //   {
                        //     auto pointer = iter1->first;
                        //     if (isa<GlobalValue>(pointer))
                        //       iter1 = AVALUE.erase(iter1);
                        //     else
                        //       iter1++;
                        //   }
                        //   // 删除RVALUE中的全局变量
                        //   for (auto iter = RVALUE.begin(); iter != RVALUE.end();)
                        //   {
                        //     auto ldInst = dynamic_cast<LoadInst *>(*iter);
                        //     auto pointer = ldInst->getPointer();
                        //     if (isa<GlobalValue>(pointer))
                        //       iter = RVALUE.erase(iter);
                        //     else
                        //       iter++;
                        //   }
                        // }
                        auto my_callInst = builder.createCallInst(callInst->getCallee(), my_args);
                        callInst->setAlter(my_callInst);
                    }
                    else if (isa<ReturnInst>(instr))
                    {
                        auto rInst = dynamic_cast<ReturnInst *>(instr);
                        auto retValue = rInst->getReturnValue();
                        if (retValue && RVALUE.find(retValue) != RVALUE.end() && !RVALUE[retValue].empty())
                        {
                            // auto ldInst = dynamic_cast<LoadInst *>(retValue);
                            auto pointer = RVALUE[retValue].begin()->first;
                            auto indices = RVALUE[retValue].begin()->second;
                            retValue = AVALUE[pointer][indices];
                        }
                        auto my_retValue = retValue ? retValue->getAlter() : nullptr;
                        auto my_rInst = builder.createReturnInst(my_retValue, mybb);
                        rInst->setAlter(my_rInst);
                    }
                    else if (isa<CondBrInst>(instr))
                    {
                        auto cbInst = dynamic_cast<CondBrInst *>(instr);
                        auto cond = cbInst->getCondition();
                        if (RVALUE.find(cond) != RVALUE.end() && !RVALUE[cond].empty())
                        {
                            auto pointer = RVALUE[cond].begin()->first;
                            auto indices = RVALUE[cond].begin()->second;
                            cond = AVALUE[pointer][indices];
                        }
                        BasicBlock *current = builder.getBasicBlock();
                        BasicBlock *my_then = dynamic_cast<BasicBlock *>(cbInst->getThenBlock()->getAlter());
                        BasicBlock *my_else = dynamic_cast<BasicBlock *>(cbInst->getElseBlock()->getAlter());
                        builder.createCondBrInst(cond->getAlter(), my_then, my_else, {}, {});
                    }
                    else if (isa<UncondBrInst>(instr))
                    {
                        auto ucbInst = dynamic_cast<UncondBrInst *>(instr);
                        auto my_block = dynamic_cast<BasicBlock *>(ucbInst->getBlock()->getAlter());
                        builder.createUncondBrInst(my_block, {});
                    }
                    else if (isa<AllocaInst>(instr))
                    {
                        auto allocaInst = dynamic_cast<AllocaInst *>(instr);
                        auto my_dims = vector<Value *>(allocaInst->getDims().begin(), allocaInst->getDims().end());
                        auto my_allocaInst = builder.createAllocaInst(allocaInst->getType(), my_dims, allocaInst->getName(), allocaInst->Const());
                        allocaInst->setAlter(my_allocaInst);
                    }
                }
            }
        }
    }
    // Lifetime
    Module *Lifetime::Run()
    {
        // cal Use and Def
        auto functions = pModule->getFunctions();
        for (auto fiter = functions->begin(); fiter != functions->end(); fiter++)
        {
            Function *func = fiter->second;
            auto bblist = func->getBasicBlocks();
            if (bblist.empty() || !(func->getName() != "getint" && func->getName() != "getch" && func->getName() != "getfloat" && func->getName() != "getarray" && func->getName() != "getfarray" && func->getName() != "putint" && func->getName() != "putch" && func->getName() != "putfloat" && func->getName() != "putarray" && func->getName() != "putfarray" && func->getName() != "starttime" && func->getName() != "stoptime" && func->getName() != "putf"))
                continue;
            for (auto biter = bblist.begin(); biter != bblist.end(); biter++)
            {
                auto bb = biter->get();
                bb->LiveIn.clear();
                bb->LiveOut.clear();
                bb->Use.clear();
                bb->Def.clear();
                bb->vLiveIn.clear();
                bb->vLiveOut.clear();
                bb->vUse.clear();
                bb->vDef.clear();
                CalUse_Def(bb);
            }
        }

        // cal In and Out
        for (auto fiter = functions->begin(); fiter != functions->end(); fiter++)
        {
            Function *func = fiter->second;
            auto bblist = func->getBasicBlocks();
            if (bblist.empty() || !(func->getName() != "getint" && func->getName() != "getch" && func->getName() != "getfloat" && func->getName() != "getarray" && func->getName() != "getfarray" && func->getName() != "putint" && func->getName() != "putch" && func->getName() != "putfloat" && func->getName() != "putarray" && func->getName() != "putfarray" && func->getName() != "starttime" && func->getName() != "stoptime" && func->getName() != "putf"))
                continue;
            CalIn_out(func);
        }
        return pModule;
    }
    void Lifetime::CalUse_Def(BasicBlock *curbb)
    {
        // auto Instrs = curbb->getInstructions();
        for (auto iiter = curbb->rbegin(); iiter != curbb->rend(); iiter++)
        {
            auto instr = iiter->get();
            auto instrType = instr->getKind();
            switch (instrType)
            {
            // Load Instr
            case Instruction::kLoad:
            {
                LoadInst *ldInst = dynamic_cast<LoadInst *>(instr);
                curbb->Def.insert(instr);
                curbb->Use.erase(instr);
                auto pointer = ldInst->getPointer();
                auto indices = vector<Value *>(ldInst->getIndices().begin(), ldInst->getIndices().end());
                curbb->vUse.insert(pointer);
                for (auto indice : indices)
                {
                    if (isa<Instruction>(indice))
                        curbb->Use.insert(dynamic_cast<Instruction *>(indice));
                }
                break;
            }

            // Store Instr
            case Instruction::kStore:
            {
                StoreInst *stInst = dynamic_cast<StoreInst *>(instr);
                auto value = stInst->getValue();
                auto pointer = stInst->getPointer();
                auto indices = vector<Value *>(stInst->getIndices().begin(), stInst->getIndices().end());
                if (isa<Instruction>(value))
                {
                    Instruction *t = dynamic_cast<Instruction *>(value);
                    curbb->Use.insert(t);
                }
                curbb->vDef.insert(pointer);
                curbb->vUse.erase(pointer);
                for (auto indice : indices)
                {
                    if (isa<Instruction>(indice))
                        curbb->Use.insert(dynamic_cast<Instruction *>(indice));
                }
                break;
            }

            // Binary Instr
            case Instruction::kICmpEQ:
            case Instruction::kICmpGE:
            case Instruction::kICmpGT:
            case Instruction::kICmpLE:
            case Instruction::kICmpLT:
            case Instruction::kICmpNE:
            case Instruction::kAdd:
            case Instruction::kMul:
            case Instruction::kSub:
            case Instruction::kDiv:
            case Instruction::kRem:
            case Instruction::kFAdd:
            case Instruction::kFSub:
            case Instruction::kFMul:
            case Instruction::kFDiv:
            case Instruction::kFRem:
            case Instruction::kFCmpEQ:
            case Instruction::kFCmpNE:
            case Instruction::kFCmpLT:
            case Instruction::kFCmpGT:
            case Instruction::kFCmpLE:
            case Instruction::kFCmpGE:
            {
                curbb->Def.insert(instr);
                curbb->Use.erase(instr);
                BinaryInst *bInst = dynamic_cast<BinaryInst *>(instr);
                auto lhs = bInst->getLhs();
                auto rhs = bInst->getRhs();
                if (isa<Instruction>(lhs))
                {
                    Instruction *t = dynamic_cast<Instruction *>(lhs);
                    curbb->Use.insert(t);
                }
                if (isa<Instruction>(rhs))
                {
                    Instruction *t = dynamic_cast<Instruction *>(rhs);
                    curbb->Use.insert(t);
                }
                break;
            }

            // Unary Instr
            case Instruction::kFNeg:
            case Instruction::kFtoI:
            case Instruction::kNeg:
            case Instruction::kNot:
            case Instruction::kItoF:
            {
                curbb->Def.insert(instr);
                curbb->Use.erase(instr);
                UnaryInst *unaryInst = dynamic_cast<UnaryInst *>(instr);
                auto val = unaryInst->getOperand();
                if (isa<Instruction>(val))
                {
                    Instruction *t = dynamic_cast<Instruction *>(val);
                    curbb->Use.insert(t);
                }
                break;
            }

            // Call Instr
            case Instruction::kCall:
            {
                CallInst *callInst = dynamic_cast<CallInst *>(instr);
                auto args = callInst->getArguments();
                for (auto arg : args)
                {
                    if (isa<Instruction>(arg))
                        curbb->Use.insert(dynamic_cast<Instruction *>(arg));
                }
                if (!callInst->getType()->isVoid())
                {
                    curbb->Def.insert(instr);
                    curbb->Use.erase(instr);
                }
                break;
            }

            // Return Instr
            case Instruction::kReturn:
            {
                ReturnInst *retInst = dynamic_cast<ReturnInst *>(instr);
                auto retval = retInst->getReturnValue();
                if (retval != nullptr)
                {
                    Instruction *t = dynamic_cast<Instruction *>(retval);
                    curbb->Use.insert(t);
                }
                break;
            }

            // CondBr Instr
            case Instruction::kCondBr:
            {
                CondBrInst *condbrInst = dynamic_cast<CondBrInst *>(instr);
                auto cond = condbrInst->getCondition();
                if (isa<Instruction>(cond))
                {
                    Instruction *t = dynamic_cast<Instruction *>(cond);
                    curbb->Use.insert(t);
                    curbb->Use.insert(t);
                }
                break;
            }

            // no use reg instruction
            case Instruction::kAlloca:
            case Instruction::kBr:
            {
                break;
            }
            default:
            {
                assert(1);
                break;
            }
            }
        }
    }
    void Lifetime::CalIn_out(Function *curFunc)
    {
        bool change = true;
        auto bblist = curFunc->getBasicBlocks();
        if (bblist.empty())
            return;
        int loop = 0;
        while (change)
        {
            change = false;
            for (auto iter = bblist.begin(); iter != bblist.end(); iter++)
            {
                BasicBlock *bb = iter->get();
                std::set<Instruction *> oldlive_in, temp, t;
                std::set<Value *> oldvlive_in, vtemp, vt;
                temp.clear();
                vtemp.clear();
                t.clear();
                vt.clear();
                for (auto item : bb->LiveIn)
                {
                    oldlive_in.insert(item);
                }
                for (auto item : bb->vLiveIn)
                {
                    oldvlive_in.insert(item);
                }
                bb->LiveOut.clear();
                bb->vLiveOut.clear();
                // Cal Live Out
                auto succs = bb->getSuccessors();
                if (bb->getNumSuccessors() > 0)
                {
                    // std::cout << "[" << bb->getName() << "]" << " " << bb->getNumSuccessors() << std::endl;
                    for (int i = 0; i < succs.size(); i++)
                    {
                        // std::cout << "-" <<succs[i]->getName() << std::endl;
                        if (i == 0)
                        {
                            for (auto item : succs[0]->LiveIn)
                            {
                                temp.insert(item);
                            }
                            for (auto item : succs[0]->vLiveIn)
                            {
                                vtemp.insert(item);
                            }
                        }
                        else
                        {
                            std::set_union(temp.begin(), temp.end(), succs[i]->LiveIn.begin(), succs[i]->LiveIn.end(), inserter(t, t.begin()));
                            // std::cout << "{";
                            // for(auto &item : temp)
                            // {
                            //   std::cout << item->getName() << " ";
                            // }
                            // std::cout << "} U {";
                            // for(auto &item : succs[i]->LiveIn)
                            // {
                            //   std::cout << item->getName() << " ";
                            // }
                            // std::cout << "} = {";
                            // for(auto &item : t)
                            // {
                            //   std::cout << item->getName() << " ";
                            // }
                            // std::cout << "}" << std::endl;
                            temp.clear();
                            for (auto &item : t)
                            {
                                temp.insert(item);
                            }
                            t.clear();
                            std::set_union(vtemp.begin(), vtemp.end(), succs[i]->vLiveIn.begin(), succs[i]->vLiveIn.end(), inserter(vt, vt.begin()));
                            vtemp.clear();
                            for (auto &item : vt)
                            {
                                vtemp.insert(item);
                            }
                            vt.clear();
                        }
                    }
                }
                for (auto &item : temp)
                {
                    bb->LiveOut.insert(item);
                }
                for (auto &item : vtemp)
                {
                    bb->vLiveOut.insert(item);
                }
                vtemp.clear();
                vt.clear();
                temp.clear();
                t.clear();

                // cal Live Intemp
                bb->LiveIn.clear();
                bb->vLiveIn.clear();

                std::set_difference(bb->LiveOut.begin(), bb->LiveOut.end(), bb->Def.begin(), bb->Def.end(), inserter(temp, temp.begin()));
                std::set_union(temp.begin(), temp.end(), bb->Use.begin(), bb->Use.end(), inserter(bb->LiveIn, bb->LiveIn.begin()));
                temp.clear();
                std::set_difference(bb->vLiveOut.begin(), bb->vLiveOut.end(), bb->vDef.begin(), bb->vDef.end(), inserter(vtemp, vtemp.begin()));
                std::set_union(vtemp.begin(), vtemp.end(), bb->vUse.begin(), bb->vUse.end(), inserter(bb->vLiveIn, bb->vLiveIn.begin()));
                vtemp.clear();
                // Check if changed
                if (bb->LiveIn != oldlive_in || bb->vLiveIn != oldvlive_in)
                    change = true;
            }
            loop++;
        }
        // std::cout << "TOTAL LOOP : " << loop << endl;
    }
    void Lifetime::print_USE_DEF(std::ostream &os)
    {
        auto functions = pModule->getFunctions();
        for (auto fiter = functions->begin(); fiter != functions->end(); fiter++)
        {
            Function *func = fiter->second;
            if (func->getName() != "getint" && func->getName() != "getch" && func->getName() != "getfloat" && func->getName() != "getarray" && func->getName() != "getfarray" && func->getName() != "putint" && func->getName() != "putch" && func->getName() != "putfloat" && func->getName() != "putarray" && func->getName() != "putfarray" && func->getName() != "starttime" && func->getName() != "stoptime" && func->getName() != "putf")
            {
                auto bblist = func->getBasicBlocks();
                os << "**********" << func->getName() << "**********"
                   << "\n";
                for (auto biter = bblist.begin(); biter != bblist.end(); biter++)
                {
                    auto bb = biter->get();
                    os << "$$" << bb->getName() << "$$"
                       << "\n";
                    os << "[USE]"
                       << "\n";
                    for (auto &k : bb->Use)
                    {
                        os << "%" << k->getName() << " ";
                    }
                    for (auto &i : bb->vUse)
                    {
                        os << "%" << i->getName();
                        // for (auto indice : indices)
                        // {
                        //   if (isa<ConstantValue>(indice))
                        //   {
                        //     os << "[";
                        //     os << dynamic_cast<ConstantValue *>(indice)->getInt();
                        //     os << "]";
                        //   }
                        //   else
                        //   {
                        //     os << "[";
                        //     os << "%" << indice->getName();
                        //     os << "]";
                        //   }
                        // }
                        os << " ";
                    }

                    os << "\n";
                    os << "[DEF]"
                       << "\n";
                    for (auto &g : bb->Def)
                    {
                        os << "%" << g->getName() << " ";
                    }
                    for (auto &i : bb->vDef)
                    {
                        os << "%" << i->getName();
                        // auto indices = i.second;
                        // for (auto indice : indices)
                        // {
                        //   if (isa<ConstantValue>(indice))
                        //   {
                        //     os << "[";
                        //     os << dynamic_cast<ConstantValue *>(indice)->getInt();
                        //     os << "]";
                        //   }
                        //   else
                        //   {
                        //     os << "[";
                        //     os << "%" << indice->getName();
                        //     os << "]";
                        //   }
                        // }
                        cout << " ";
                    }
                    os << "\n";
                }
            }
        }
    }
    void Lifetime::print_Live_IN_OUT(std::ostream &os)
    {
        auto functions = pModule->getFunctions();
        for (auto fiter = functions->begin(); fiter != functions->end(); fiter++)
        {
            Function *func = fiter->second;
            auto bblist = func->getBasicBlocks();
            if (func->getName() != "getint" && func->getName() != "getch" && func->getName() != "getfloat" && func->getName() != "getarray" && func->getName() != "getfarray" && func->getName() != "putint" && func->getName() != "putch" && func->getName() != "putfloat" && func->getName() != "putarray" && func->getName() != "putfarray" && func->getName() != "starttime" && func->getName() != "stoptime" && func->getName() != "putf")
            {
                os << "**********" << func->getName() << "**********"
                   << "\n";
                for (auto biter = bblist.begin(); biter != bblist.end(); biter++)
                {
                    auto bb = biter->get();
                    os << "$$" << bb->getName() << "$$"
                       << "\n";
                    os << "[LIVE_IN]"
                       << "\n";
                    for (auto &k : bb->LiveIn)
                    {
                        os << "%" << k->getName() << " ";
                    }
                    for (auto &i : bb->vLiveIn)
                    {
                        os << "%" << i->getName();
                        // auto indices = i.second;
                        // for (auto indice : indices)
                        // {
                        //   if (isa<ConstantValue>(indice))
                        //   {
                        //     os << "[";
                        //     os << dynamic_cast<ConstantValue *>(indice)->getInt();
                        //     os << "]";
                        //   }
                        //   else
                        //   {
                        //     os << "[";
                        //     os << "%" << indice->getName();
                        //     os << "]";
                        //   }
                        // }
                        os << " ";
                    }
                    os << "\n";
                    os << "[LIVE_OUT]"
                       << "\n";
                    for (auto &g : bb->LiveOut)
                    {
                        os << "%" << g->getName();
                    }
                    for (auto &i : bb->vLiveOut)
                    {
                        os << "%" << i->getName();
                        // auto indices = i.second;
                        // for (auto indice : indices)
                        // {
                        //   if (isa<ConstantValue>(indice))
                        //   {
                        //     os << "[";
                        //     os << dynamic_cast<ConstantValue *>(indice)->getInt();
                        //     os << "]";
                        //   }
                        //   else
                        //   {
                        //     os << "[";
                        //     os << "%" << indice->getName();
                        //     os << "]";
                        //   }
                        // }
                        os << " ";
                    }
                    os << "\n";
                }
            }
        }
    }
    // DCE
    Module *DCE::Run()
    {
        auto functions = pModule->getFunctions();
        std::set<Instruction *> live;
        std::set<Value *> vlive;
        int cnt = 0;
        // DCE for every functions
        for (auto fiter = functions->begin(); fiter != functions->end(); fiter++)
        {
            Function *func = fiter->second;
            auto bblist = func->getBasicBlocks();
            if (bblist.empty())
                continue;
            bool change = true;
            while (change)
            {
                change = false;
                // live var analyze for BasicBlocks
                Lifetime lifetime(pModule);
                pModule = lifetime.Run();

                // live var analyze for Instructions
                for (auto biter = bblist.begin(); biter != bblist.end(); biter++)
                {
                    BasicBlock *bb = biter->get();
                    live.clear();
                    vlive.clear();
                    for (auto item : bb->LiveOut)
                    {
                        live.insert(item);
                    }
                    for (auto item : bb->vLiveOut)
                    {
                        vlive.insert(item);
                    }
                    for (auto iiter = bb->rbegin(); iiter != bb->rend(); iiter++)
                    {
                        auto instr = iiter->get();
                        auto instrType = instr->getKind();
                        instr->back_live.clear();
                        instr->back_vlive.clear();
                        instr->front_live.clear();
                        instr->front_vlive.clear();
                        for (auto item : live)
                        {
                            instr->back_live.insert(item);
                        }
                        for (auto item : vlive)
                        {
                            instr->back_vlive.insert(item);
                        }
                        switch (instrType)
                        {
                        // Load Instr
                        case Instruction::kLoad:
                        {
                            LoadInst *ldInst = dynamic_cast<LoadInst *>(instr);
                            auto pointer = ldInst->getPointer();
                            auto indices = vector<Value *>(ldInst->getIndices().begin(), ldInst->getIndices().end());
                            if (live.count(instr) == 0)
                            {
                                bb->getInstructions().erase(--iiter.base());
                                iiter--;
                                cnt++;
                                change = true;
                                break;
                            }
                            live.erase(instr);
                            vlive.insert(pointer);
                            for (auto indice : indices)
                            {
                                if (isa<Instruction>(indice))
                                    live.insert(dynamic_cast<Instruction *>(indice));
                            }
                            break;
                        }

                        // Store Instr
                        case Instruction::kStore:
                        {
                            StoreInst *stInst = dynamic_cast<StoreInst *>(instr);
                            auto value = stInst->getValue();
                            auto pointer = stInst->getPointer();
                            auto indices = vector<Value *>(stInst->getIndices().begin(), stInst->getIndices().end());
                            if (vlive.count(pointer) == 0 && !isa<GlobalValue>(pointer) && indices.size() == 0)
                            {
                                bb->getInstructions().erase(--iiter.base());
                                change = true;
                                iiter--;
                                cnt++;
                                break;
                            }
                            if (indices.size() == 0)
                                vlive.erase(pointer);
                            if (isa<Instruction>(value))
                            {
                                Instruction *t = dynamic_cast<Instruction *>(value);
                                live.insert(t);
                            }
                            for (auto indice : indices)
                            {
                                if (isa<Instruction>(indice))
                                    live.insert(dynamic_cast<Instruction *>(indice));
                            }
                            break;
                        }

                        // Binary Instr
                        case Instruction::kICmpEQ:
                        case Instruction::kICmpGE:
                        case Instruction::kICmpGT:
                        case Instruction::kICmpLE:
                        case Instruction::kICmpLT:
                        case Instruction::kICmpNE:
                        case Instruction::kAdd:
                        case Instruction::kMul:
                        case Instruction::kSub:
                        case Instruction::kDiv:
                        case Instruction::kRem:
                        case Instruction::kFAdd:
                        case Instruction::kFSub:
                        case Instruction::kFMul:
                        case Instruction::kFDiv:
                        case Instruction::kFRem:
                        case Instruction::kFCmpEQ:
                        case Instruction::kFCmpNE:
                        case Instruction::kFCmpLT:
                        case Instruction::kFCmpGT:
                        case Instruction::kFCmpLE:
                        case Instruction::kFCmpGE:
                        {
                            if (live.count(instr) == 0)
                            {
                                bb->getInstructions().erase(--iiter.base());
                                change = true;
                                iiter--;
                                cnt++;
                                break;
                            }
                            else
                            {
                                live.erase(instr);
                            }
                            BinaryInst *bInst = dynamic_cast<BinaryInst *>(instr);
                            auto lhs = bInst->getLhs();
                            auto rhs = bInst->getRhs();
                            if (isa<Instruction>(lhs))
                            {
                                Instruction *t = dynamic_cast<Instruction *>(lhs);
                                live.insert(t);
                            }
                            if (isa<Instruction>(rhs))
                            {
                                Instruction *t = dynamic_cast<Instruction *>(rhs);
                                live.insert(t);
                            }
                            break;
                        }

                        // Unary Instr
                        case Instruction::kFNeg:
                        case Instruction::kFtoI:
                        case Instruction::kNeg:
                        case Instruction::kNot:
                        case Instruction::kItoF:
                        {
                            if (live.count(instr) == 0)
                            {
                                bb->getInstructions().erase(--iiter.base());
                                change = true;
                                iiter--;
                                cnt++;
                                break;
                            }
                            else
                            {
                                live.erase(instr);
                            }
                            UnaryInst *unaryInst = dynamic_cast<UnaryInst *>(instr);
                            auto val = unaryInst->getOperand();
                            if (isa<Instruction>(val))
                            {
                                Instruction *t = dynamic_cast<Instruction *>(val);
                                live.insert(t);
                            }
                            break;
                        }

                        // Call Instr
                        case Instruction::kCall:
                        {
                            CallInst *callInst = dynamic_cast<CallInst *>(instr);
                            if (!callInst->getType()->isVoid())
                            {
                                // if (live.count(instr) == 0)
                                // {
                                //   bb->getInstructions().erase(--iiter.base());
                                //   change = true;
                                //   iiter--;
                                //   cnt++;
                                //   break;
                                // }
                                // else
                                // {
                                live.erase(instr);
                                // }
                            }
                            auto args = callInst->getArguments();
                            for (auto arg : args)
                            {
                                if (isa<Instruction>(arg))
                                {
                                    Instruction *t = dynamic_cast<Instruction *>(arg);
                                    live.insert(t);
                                }
                            }
                            break;
                        }

                        // Return Instr
                        case Instruction::kReturn:
                        {
                            ReturnInst *retInst = dynamic_cast<ReturnInst *>(instr);
                            auto retval = retInst->getReturnValue();
                            if (retval != nullptr)
                            {
                                if (isa<Instruction>(retval))
                                {
                                    Instruction *t = dynamic_cast<Instruction *>(retval);
                                    live.insert(t);
                                }
                            }
                            break;
                        }

                        // CondBr Instr
                        case Instruction::kCondBr:
                        {
                            CondBrInst *condbrInst = dynamic_cast<CondBrInst *>(instr);
                            auto cond = condbrInst->getCondition();
                            if (isa<Instruction>(cond))
                            {
                                Instruction *t = dynamic_cast<Instruction *>(cond);
                                live.insert(t);
                            }
                            break;
                        }

                        case Instruction::kAlloca:
                        {
                            // Value *var = dynamic_cast<Value *>(instr);
                            // bool flag = false;
                            // for (auto &i : vlive)
                            // {
                            //   if(i.first == var)
                            //     flag = true;
                            // }
                            // if (!flag)
                            // {
                            //   bb->getInstructions().erase(--iiter.base());
                            //   change = true;
                            // }
                            break;
                        }
                        // no use reg instruction
                        case Instruction::kBr:
                        {
                            break;
                        }
                        default:
                        {
                            assert(1);
                            break;
                        }
                        }
                        for (auto item : live)
                        {
                            instr->front_live.insert(item);
                        }
                        for (auto item : vlive)
                        {
                            instr->front_vlive.insert(item);
                        }
                        if (change)
                        {
                            continue;
                            // break;
                        }
                    }
                    // if this instr is deleted, break the loop and recalculate the live var
                    if (change)
                    {
                        break;
                    }
                }
            }
        }
        // 找到所有出现过的变量
        set<Value *> pointers;
        for (auto fiter = functions->begin(); fiter != functions->end(); fiter++)
        {
            Function *func = fiter->second;
            auto bblist = func->getBasicBlocks();
            if (bblist.empty())
                continue;
            pointers.clear();
            for (auto biter = bblist.begin(); biter != bblist.end(); biter++)
            {
                BasicBlock *bb = biter->get();
                for (auto iiter = bb->begin(); iiter != bb->end(); iiter++)
                {
                    auto instr = iiter->get();
                    auto instrType = instr->getKind();
                    if (instrType == Instruction::kLoad)
                    {
                        pointers.insert(dynamic_cast<LoadInst *>(instr)->getPointer());
                    }
                    else if (instrType == Instruction ::kStore)
                    {
                        pointers.insert(dynamic_cast<StoreInst *>(instr)->getPointer());
                    }
                }
            }
            // DCE for useless allocaInst
            for (auto biter = bblist.begin(); biter != bblist.end(); biter++)
            {
                BasicBlock *bb = biter->get();
                for (auto iiter = bb->rbegin(); iiter != bb->rend(); iiter++)
                {
                    auto instr = iiter->get();
                    auto instrType = instr->getKind();
                    if (instrType == Instruction::kAlloca)
                    {
                        if (pointers.count(instr) == 0)
                        {
                            bb->getInstructions().erase(--iiter.base());
                            iiter--;
                        }
                    }
                }
            }
        }
        // std::cout << "DCE delete [" << cnt << "] Instr" << std::endl;

        return pModule;
    }
    // CommonExp
    Module *CommonExp::Run()
    // Module *CommonExp::Run(std::ostream &os)
    {
        auto functions = OriginModule->getFunctions();
        // 计算Eval集合
        for (auto iter = functions->begin(); iter != functions->end(); iter++)
        {
            Function *func = iter->second;
            auto bblist = func->getBasicBlocks();
            if (bblist.empty())
                continue;
            for (auto iter = bblist.begin(); iter != bblist.end(); iter++)
            {
                auto bb = iter->get();
                CalEval(bb, 1);
            }
        }
        // 计算In,Out集合
        for (auto iter = functions->begin(); iter != functions->end(); iter++)
        {
            Function *func = iter->second;
            auto bblist = func->getBasicBlocks();
            if (bblist.empty())
                continue;
            CalIn_Out(func);
        }
        // 重新生成IR
        RegenerateIR();
        OriginModule = pModule;
        pModule = ppModule;
        functions = OriginModule->getFunctions();
        // 计算Eval集合
        for (auto iter = functions->begin(); iter != functions->end(); iter++)
        {
            Function *func = iter->second;
            auto bblist = func->getBasicBlocks();
            if (bblist.empty())
                continue;
            for (auto iter = bblist.begin(); iter != bblist.end(); iter++)
            {
                auto bb = iter->get();
                CalEval(bb, 2);
            }
        }
        // 计算In,Out集合
        for (auto iter = functions->begin(); iter != functions->end(); iter++)
        {
            Function *func = iter->second;
            auto bblist = func->getBasicBlocks();
            if (bblist.empty())
                continue;
            CalIn_Out(func);
        }
        // 重新生成IR
        RegenerateIR();
        return pModule;
    }
    void CommonExp::RegenerateIR()
    {
        // 生成全局变量
        auto global_values = OriginModule->getGlobalValues();
        for (auto iter = global_values->begin(); iter != global_values->end(); iter++)
        {
            GlobalValue *glbvl = iter->second;
            pModule->addGlobalValue(glbvl);
        }
        // 生成函数
        auto functions = OriginModule->getFunctions();
        for (auto iter = functions->begin(); iter != functions->end(); iter++)
        {
            Function *func = iter->second;
            Function *myFunc = pModule->createFunction(func->getName(), func->getType());
            auto bblist = func->getBasicBlocks();
            if (bblist.empty())
                continue;
            // 生成该函数所有新BB
            for (auto iter = bblist.begin(); iter != bblist.end(); iter++)
            {
                BasicBlock *bb = iter->get();
                BasicBlock *mybb = myFunc->addBasicBlock(bb->getName());
                bb->setAlter(mybb);
                mybb->setDepth(bb->getDepth());
            }
            for (auto iter = bblist.begin(); iter != bblist.end(); iter++)
            {
                BasicBlock *bb = iter->get();
                BasicBlock *mybb = dynamic_cast<BasicBlock *>(bb->getAlter());
                builder.setPosition(mybb, mybb->end());
                if (iter == bblist.begin())
                {
                    auto entry_args = bb->getArguments();
                    for (auto i = entry_args.begin(); i != entry_args.end(); i++)
                    {
                        auto arg = i->get();
                        auto my_arg = mybb->createArgument(arg->getType(), vector<int>(arg->getDims().begin(), arg->getDims().end()), arg->getName());
                        arg->setAlter(my_arg);
                    }
                }
                // 为新BB设置前驱后继关系
                for (auto p : bb->getPredecessors())
                    mybb->getPredecessors().push_back(dynamic_cast<BasicBlock *>(p->getAlter()));
                for (auto s : bb->getSuccessors())
                    mybb->getSuccessors().push_back(dynamic_cast<BasicBlock *>(s->getAlter()));
                Alter.clear();
                bInsts.clear();
                uInsts.clear();
                // 接收BinaryIn集合
                for (auto p : bb->BinaryIn)
                {
                    Instruction *instr = p.first;
                    if (!instr->hasAlter())
                        continue;
                    auto instrType = p.second.first;
                    auto lhs = p.second.second.first;
                    auto rhs = p.second.second.second;
                    if (instrType == Instruction::Kind::kAdd || instrType == Instruction::Kind::kMul || instrType == Instruction::Kind::kFAdd || instrType == Instruction::Kind::kFMul)
                    {
                        bInsts[instrType][{lhs, rhs}] = instr;
                    }
                    else if (instrType == Instruction::Kind::kSub || instrType == Instruction::Kind::kDiv || instrType == Instruction::Kind::kRem || instrType == Instruction::Kind::kFSub || instrType == Instruction::Kind::kFDiv || instrType == Instruction::Kind::kFRem)
                    {
                        OrderbInsts[instrType][{lhs, rhs}] = instr;
                    }
                }
                for (auto iter = bb->begin(); iter != bb->end(); iter++)
                {
                    auto instr = iter->get();
                    auto instrType = instr->getKind();
                    if (isa<BinaryInst>(instr))
                    {
                        BinaryInst *bInst = dynamic_cast<BinaryInst *>(instr);
                        auto lhs = bInst->getLhs();
                        auto rhs = bInst->getRhs();
                        auto liter = Alter.find(lhs);
                        auto riter = Alter.find(rhs);
                        if (liter != Alter.end())
                            lhs = Alter[lhs];
                        if (riter != Alter.end())
                            rhs = Alter[rhs];
                        if (instrType == Instruction::Kind::kAdd || instrType == Instruction::Kind::kMul || instrType == Instruction::Kind::kFAdd || instrType == Instruction::Kind::kFMul)
                        {
                            if (bInsts.find(instrType) != bInsts.end() && bInsts[instrType].find({lhs, rhs}) != bInsts[instrType].end())
                            {
                                // os << "%" << bInst->getName() << "cut\n";
                                Alter[bInst] = bInsts[instrType][{lhs, rhs}];
                            }
                            else
                            {
                                // os << "%" << bInst->getName() << "keep\n";
                                auto my_bInst = builder.createBinaryInst(instrType, instr->getType(), lhs->getAlter(), rhs->getAlter());
                                bInst->setAlter(my_bInst);
                                bInsts[instrType][{lhs, rhs}] = bInst;
                            }
                        }
                        else if (instrType == Instruction::Kind::kSub || instrType == Instruction::Kind::kDiv || instrType == Instruction::Kind::kRem || instrType == Instruction::Kind::kFSub || instrType == Instruction::Kind::kFDiv || instrType == Instruction::Kind::kFRem)
                        {
                            if (OrderbInsts.find(instrType) != OrderbInsts.end() && OrderbInsts[instrType].find({lhs, rhs}) != OrderbInsts[instrType].end())
                            {
                                // os << "%" << bInst->getName() << "cut\n";
                                Alter[bInst] = OrderbInsts[instrType][{lhs, rhs}];
                            }
                            else
                            {
                                // os << "%" << bInst->getName() << "keep\n";
                                auto my_bInst = builder.createBinaryInst(instrType, instr->getType(), lhs->getAlter(), rhs->getAlter());
                                bInst->setAlter(my_bInst);
                                OrderbInsts[instrType][{lhs, rhs}] = bInst;
                            }
                        }
                        else
                        {
                            auto my_bInst = builder.createBinaryInst(instrType, instr->getType(), lhs->getAlter(), rhs->getAlter());
                            bInst->setAlter(my_bInst);
                        }
                    }
                    else if (isa<UnaryInst>(instr))
                    {
                        auto uInst = dynamic_cast<UnaryInst *>(instr);
                        auto hs = uInst->getOperand();
                        if (Alter.find(hs) != Alter.end())
                            hs = Alter[hs];
                        // if (uInsts.find(instrType) != uInsts.end() && uInsts[instrType].find(hs) != uInsts[instrType].end())
                        //   Alter[uInst] = uInsts[instrType][hs];
                        // else
                        {
                            auto my_uInst = builder.createUnaryInst(instrType, instr->getType(), hs->getAlter());
                            uInst->setAlter(my_uInst);
                        }
                    }
                    else if (isa<LoadInst>(instr))
                    {
                        auto ldInst = dynamic_cast<LoadInst *>(instr);
                        auto pointer = ldInst->getPointer();
                        auto indices = vector<Value *>(ldInst->getIndices().begin(), ldInst->getIndices().end());
                        vector<Value *> my_indices;
                        for (int i = 0; i < indices.size(); i++)
                        {
                            if (Alter.find(indices[i]) != Alter.end())
                                my_indices.push_back(Alter[indices[i]]->getAlter());
                            else
                                my_indices.push_back(indices[i]->getAlter());
                        }
                        auto my_ldInst = builder.createLoadInst(pointer->getAlter(), my_indices);
                        ldInst->setAlter(my_ldInst);
                    }
                    else if (isa<StoreInst>(instr))
                    {
                        auto stInst = dynamic_cast<StoreInst *>(instr);
                        auto value = stInst->getValue();
                        auto pointer = stInst->getPointer();
                        auto indices = vector<Value *>(stInst->getIndices().begin(), stInst->getIndices().end());
                        vector<Value *> my_indices;
                        for (int i = 0; i < indices.size(); i++)
                        {
                            if (Alter.find(indices[i]) != Alter.end())
                                my_indices.push_back(Alter[indices[i]]->getAlter());
                            else
                                my_indices.push_back(indices[i]->getAlter());
                        }
                        if (Alter.find(value) != Alter.end())
                            value = Alter[value];
                        auto my_stInst = builder.createStoreInst(value->getAlter(), pointer->getAlter(), my_indices);
                    }
                    else if (isa<CallInst>(instr))
                    {
                        auto callInst = dynamic_cast<CallInst *>(instr);
                        auto args = callInst->getArguments();
                        vector<Value *> my_args;
                        for (auto iter = args.begin(); iter != args.end(); iter++)
                        {
                            if (Alter.find(*iter) != Alter.end())
                                my_args.push_back(Alter[*iter]->getAlter());
                            else
                                my_args.push_back(iter->getAlter());
                        }
                        auto my_callInst = builder.createCallInst(callInst->getCallee(), my_args);
                        callInst->setAlter(my_callInst);
                    }
                    else if (isa<ReturnInst>(instr))
                    {
                        auto rInst = dynamic_cast<ReturnInst *>(instr);
                        auto retValue = rInst->getReturnValue();
                        if (retValue && Alter.find(retValue) != Alter.end())
                            retValue = Alter[retValue];
                        auto my_retValue = retValue ? retValue->getAlter() : nullptr;
                        auto my_rInst = builder.createReturnInst(my_retValue, mybb);
                        rInst->setAlter(my_rInst);
                    }
                    else if (isa<CondBrInst>(instr))
                    {
                        auto cbInst = dynamic_cast<CondBrInst *>(instr);
                        auto cond = cbInst->getCondition();
                        if (Alter.find(cond) != Alter.end())
                            cond = Alter[cond];
                        BasicBlock *current = builder.getBasicBlock();
                        BasicBlock *my_then = dynamic_cast<BasicBlock *>(cbInst->getThenBlock()->getAlter());
                        BasicBlock *my_else = dynamic_cast<BasicBlock *>(cbInst->getElseBlock()->getAlter());
                        builder.createCondBrInst(cond->getAlter(), my_then, my_else, {}, {});
                    }
                    else if (isa<UncondBrInst>(instr))
                    {
                        auto ucbInst = dynamic_cast<UncondBrInst *>(instr);
                        auto my_block = dynamic_cast<BasicBlock *>(ucbInst->getBlock()->getAlter());
                        builder.createUncondBrInst(my_block, {});
                    }
                    else if (isa<AllocaInst>(instr))
                    {
                        auto allocaInst = dynamic_cast<AllocaInst *>(instr);
                        auto my_dims = vector<Value *>(allocaInst->getDims().begin(), allocaInst->getDims().end());
                        auto my_allocaInst = builder.createAllocaInst(allocaInst->getType(), my_dims, allocaInst->getName(), allocaInst->Const());
                        allocaInst->setAlter(my_allocaInst);
                    }
                }
            }
        }
    }
    void CommonExp::CalEval(BasicBlock *curbb, int pass)
    {
        for (auto &instriter : curbb->getInstructions())
        {
            auto instr = instriter.get();
            auto instrType = instriter->getKind();
            if (isa<BinaryInst>(instr))
            {
                auto bInst = dynamic_cast<BinaryInst *>(instr);
                auto lhs = bInst->getLhs();
                auto rhs = bInst->getRhs();
                bool lflag = false, rflag = false;
                if (isa<LoadInst>(lhs) || isa<ConstantValue>(lhs))
                    lflag = true;
                if (isa<LoadInst>(rhs) || isa<ConstantValue>(rhs))
                    rflag = true;
                if (pass == 2)
                {
                    lflag = true;
                    rflag = true;
                }
                if (lflag && rflag && ((instrType >= 0 && instrType <= 4) || (instrType >= 14 && instrType <= 18)))
                    curbb->BinaryEval.insert({bInst, {instrType, {lhs, rhs}}});
            }
            // else if (isa<UnaryInst>(instr))
            // {
            //   auto uInst = dynamic_cast<UnaryInst *>(instr);
            //   auto hs = uInst->getOperand();
            //   if (isa<LoadInst>(hs) || isa<ConstantValue>(hs))
            //     curbb->UnaryEval.insert({uInst, {instrType, hs}});
            // }
        }
    }
    void CommonExp::CalIn_Out(Function *curFunc)
    {
        bool change = true;
        auto bblist = curFunc->getBasicBlocks();
        if (bblist.empty())
            return;
        int k = 0;
        while (change)
        {
            k++;
            // printf("%d\t", k);
            change = false;
            for (auto iter = bblist.begin(); iter != bblist.end(); iter++)
            {
                BasicBlock *bb = iter->get();
                set<pair<Instruction *, pair<Instruction::Kind, pair<Value *, Value *>>>> olduin, oldbin, olduout, oldbout, temp, t;
                // copy(bb->in.begin(), bb->in.end(), oldin.begin());
                for (auto item : bb->BinaryIn)
                {
                    oldbin.insert(item);
                }
                // copy(bb->out.begin(), bb->out.end(), oldout.begin());
                for (auto item : bb->BinaryOut)
                {
                    oldbout.insert(item);
                }
                bb->BinaryIn.clear();
                bb->BinaryOut.clear();
                // cal IN
                auto pres = bb->getPredecessors();
                if (!pres.empty())
                {
                    for (int i = 0; i < pres.size(); i++)
                    {
                        if (i == 0)
                        {
                            // copy(pres[0]->out.begin(), pres[0]->out.end(), temp.begin());
                            for (auto item : pres[0]->BinaryOut)
                            {
                                temp.insert(item);
                            }
                        }
                        else
                        {
                            set_intersection(temp.begin(), temp.end(), pres[i]->BinaryOut.begin(), pres[i]->BinaryOut.end(), inserter(t, t.begin()));
                            temp.clear();
                            for (auto item : t)
                            {
                                temp.insert(item);
                            }
                            t.clear();
                        }
                    }
                }
                // copy(temp.begin(), temp.end(), bb->in.begin());
                for (auto item : temp)
                {
                    bb->BinaryIn.insert(item);
                }
                // cal OUT
                for (auto &item : bb->BinaryIn)
                {
                    bb->BinaryOut.insert(item);
                }
                for (auto &item : bb->BinaryEval)
                {
                    bb->BinaryOut.insert(item);
                }
                // Check if changed
                if (oldbin != bb->BinaryIn || oldbout != bb->BinaryOut)
                    change = true;
            }

            // print in out
            //  for(auto iter = bblist.begin(); iter != bblist.end(); iter++)
            //  {
            //    BasicBlock* bb = iter->get();
            //    std::cout << bb->getName() << endl;
            //  }
        }
    }
    // ConstSpread
    Module *ConstSpread::Run()
    {
        auto functions = OriginModule->getFunctions();
        for (auto iter = functions->begin(); iter != functions->end(); iter++)
        {
            Function *func = iter->second;
            auto bblist = func->getBasicBlocks();
            if (bblist.empty())
                continue;
            CalIn_Out(func);
        }
        RegenerateIR();
        return pModule;
    }
    void ConstSpread::CalIn_Out(Function *curFunc)
    {
        auto bblist = curFunc->getBasicBlocks();
        // 计算out[Entry]
        BasicBlock *entry = curFunc->getEntryBlock();
        Fs(entry);
        // 求不动点
        bool change = true;
        while (change)
        {
            change = false;
            for (auto iter = bblist.begin(); iter != bblist.end(); iter++)
            {
                if (iter == bblist.begin())
                    continue;
                BasicBlock *bb = iter->get();
                set<pair<Instruction *, ConstantValue *>> oldin, oldout, temp, t;
                set<pair<pair<Value *, vector<Value *>>, ConstantValue *>> oldvin, oldvout, vtemp, vt;
                oldin.insert(bb->ConstIn.begin(), bb->ConstIn.end());
                oldvin.insert(bb->vConstIn.begin(), bb->vConstIn.end());
                oldout.insert(bb->ConstOut.begin(), bb->ConstOut.end());
                oldvout.insert(bb->vConstOut.begin(), bb->vConstOut.end());
                // 计算In
                auto pres = bb->getPredecessors();
                for (int i = 0; i < pres.size(); i++)
                {
                    if (i == 0)
                        temp.insert(pres[0]->ConstOut.begin(), pres[0]->ConstOut.end());
                    else
                    {
                        set_intersection(temp.begin(), temp.end(), pres[i]->ConstOut.begin(), pres[i]->ConstOut.end(), inserter(t, t.begin()));
                        temp.clear();
                        temp.insert(t.begin(), t.end());
                        t.clear();
                    }
                }
                bb->ConstIn.insert(temp.begin(), temp.end());
                // 计算vIn
                for (int i = 0; i < pres.size(); i++)
                {
                    if (i == 0)
                        vtemp.insert(pres[0]->vConstOut.begin(), pres[0]->vConstOut.end());
                    else
                    {
                        set_intersection(vtemp.begin(), vtemp.end(), pres[i]->vConstOut.begin(), pres[i]->vConstOut.end(), inserter(vt, vt.begin()));
                        vtemp.clear();
                        vtemp.insert(vt.begin(), vt.end());
                        vt.clear();
                    }
                }
                bb->vConstIn.insert(vtemp.begin(), vtemp.end());
                // 计算Out
                Fs(bb);

                if (oldin != bb->ConstIn || oldout != bb->ConstOut || oldvin != bb->vConstIn || oldvout != bb->vConstOut)
                    change = true;
            }
        }
    }
    void ConstSpread::Fs(BasicBlock *curbb)
    {
        map<Instruction *, ConstantValue *> In(curbb->ConstIn.begin(), curbb->ConstIn.end());
        map<Value *, map<vector<Value *>, ConstantValue *>> vIn;
        for (auto iter : curbb->vConstIn)
        {
            Value *value = iter.first.first;
            vector<Value *> indices = iter.first.second;
            ConstantValue *Const = iter.second;
            vIn[value][indices] = Const;
        }
        for (auto iter = curbb->begin(); iter != curbb->end(); iter++)
        {
            auto instr = iter->get();
            if (isa<LoadInst>(instr))
            {
                LoadInst *ldInst = dynamic_cast<LoadInst *>(instr);
                auto pointer = ldInst->getPointer();
                auto indices = vector<Value *>(ldInst->getIndices().begin(), ldInst->getIndices().end());
                if (vIn.find(pointer) != vIn.end() && vIn[pointer].find(indices) != vIn[pointer].end())
                {
                    In[ldInst] = vIn[pointer][indices];
                }
            }
            else if (isa<StoreInst>(instr))
            {
                StoreInst *stInst = dynamic_cast<StoreInst *>(instr);
                auto pointer = stInst->getPointer();
                auto indices = vector<Value *>(stInst->getIndices().begin(), stInst->getIndices().end());
                auto value = stInst->getValue();
                if (isa<ConstantValue>(value) && !isa<GlobalValue>(pointer))
                    vIn[pointer][indices] = dynamic_cast<ConstantValue *>(value);
                else
                {
                    Instruction *inst = dynamic_cast<Instruction *>(value);
                    if (In.find(inst) != In.end() && !isa<GlobalValue>(pointer))
                        vIn[pointer][indices] = In[inst];
                    else if (vIn.find(pointer) != vIn.end() && vIn[pointer].find(indices) != vIn[pointer].end())
                        vIn[pointer].erase(vIn[pointer].find(indices));
                }
            }
            else if (isa<BinaryInst>(instr))
            {
                BinaryInst *bInst = dynamic_cast<BinaryInst *>(instr);
                auto lhs = bInst->getLhs();
                auto rhs = bInst->getRhs();
                if (isa<Instruction>(lhs) && In.find(dynamic_cast<Instruction *>(lhs)) != In.end())
                    lhs = In[dynamic_cast<Instruction *>(lhs)];
                if (isa<Instruction>(rhs) && In.find(dynamic_cast<Instruction *>(rhs)) != In.end())
                    rhs = In[dynamic_cast<Instruction *>(rhs)];
                if (isa<ConstantValue>(lhs) && isa<ConstantValue>(rhs))
                {
                    auto lConst = dynamic_cast<ConstantValue *>(lhs);
                    auto rConst = dynamic_cast<ConstantValue *>(rhs);
                    auto kind = bInst->getKind();
                    Type *type = bInst->getType();
                    if (type->isInt())
                    {
                        int lint = lConst->getInt();
                        int rint = rConst->getInt();
                        if (kind == Value::kAdd)
                            In[bInst] = ConstantValue::get(lint + rint);
                        else if (kind == Value::kSub)
                            In[bInst] = ConstantValue::get(lint - rint);
                        else if (kind == Value::kMul)
                            In[bInst] = ConstantValue::get(lint * rint);
                        else if (kind == Value::kDiv)
                            In[bInst] = ConstantValue::get(lint / rint);
                        else if (kind == Value::kRem)
                            In[bInst] = ConstantValue::get(lint % rint);
                    }
                    else if (type->isFloat())
                    {
                        double ldouble = lConst->getDouble();
                        double rdouble = rConst->getDouble();
                        if (kind == Value::kFAdd)
                            In[bInst] = ConstantValue::get(ldouble + rdouble);
                        else if (kind == Value::kFSub)
                            In[bInst] = ConstantValue::get(ldouble - rdouble);
                        else if (kind == Value::kFMul)
                            In[bInst] = ConstantValue::get(ldouble * rdouble);
                        else if (kind == Value::kFDiv)
                            In[bInst] = ConstantValue::get(ldouble / rdouble);
                    }
                }
            }
            else if (isa<UnaryInst>(instr))
            {
                UnaryInst *uInst = dynamic_cast<UnaryInst *>(instr);
                auto hs = uInst->getOperand();
                if (isa<Instruction>(hs) && In.find(dynamic_cast<Instruction *>(hs)) != In.end())
                    hs = In[dynamic_cast<Instruction *>(hs)];
                if (isa<ConstantValue>(hs))
                {
                    auto Const = dynamic_cast<ConstantValue *>(hs);
                    auto kind = uInst->getKind();
                    Type *type = uInst->getType();
                    if (kind == Value::kNeg)
                        In[uInst] = ConstantValue::get(-Const->getInt());
                    else if (kind == Value::kFNeg)
                        In[uInst] = ConstantValue::get(-Const->getDouble());
                    else if (kind == Value::kItoF)
                        In[uInst] = ConstantValue::get((double)Const->getInt());
                    else if (kind == Value::kFtoI)
                        In[uInst] = ConstantValue::get((int)Const->getDouble());
                }
            }
            else if (isa<CallInst>(instr))
            {
                CallInst *callInst = dynamic_cast<CallInst *>(instr);
                auto args = callInst->getArguments();
                for (auto iter = args.begin(); iter != args.end(); iter++)
                {
                    if (isa<LoadInst>(*iter))
                    {
                        auto ldInst = dynamic_cast<LoadInst *>(*iter);
                        auto pointer = ldInst->getPointer();
                        auto indices = vector<Value *>(ldInst->getIndices().begin(), ldInst->getIndices().end());
                        int numdims = 0;
                        if (isa<GlobalValue>(pointer))
                            numdims = dynamic_cast<GlobalValue *>(pointer)->getNumDims();
                        else if (isa<AllocaInst>(pointer))
                            numdims = dynamic_cast<AllocaInst *>(pointer)->getNumDims();
                        else if (isa<Argument>(pointer))
                            numdims = dynamic_cast<Argument *>(pointer)->getNumDims();
                        if (indices.size() < numdims && vIn.find(pointer) != vIn.end())
                        {
                            vIn.erase(vIn.find(pointer));
                        }
                    }
                }
            }
        }
        curbb->ConstOut.clear();
        curbb->vConstOut.clear();
        for (auto iter : In)
        {
            auto instr = iter.first;
            auto Const = iter.second;
            curbb->ConstOut.insert({instr, Const});
        }
        for (auto iter1 : vIn)
        {
            Value *value = iter1.first;
            for (auto iter2 : iter1.second)
            {
                curbb->vConstOut.insert({{value, iter2.first}, iter2.second});
            }
        }
    }
    void ConstSpread::RegenerateIR()
    {
        // 生成全局变量
        auto global_values = OriginModule->getGlobalValues();
        for (auto iter = global_values->begin(); iter != global_values->end(); iter++)
        {
            GlobalValue *glbvl = iter->second;
            pModule->addGlobalValue(glbvl);
        }
        // 生成函数
        auto functions = OriginModule->getFunctions();
        for (auto iter = functions->begin(); iter != functions->end(); iter++)
        {
            Function *func = iter->second;
            Function *myFunc = pModule->createFunction(func->getName(), func->getType());
            auto bblist = func->getBasicBlocks();
            if (bblist.empty())
                continue;
            // 生成该函数所有新BB
            for (auto iter = bblist.begin(); iter != bblist.end(); iter++)
            {
                BasicBlock *bb = iter->get();
                BasicBlock *mybb = myFunc->addBasicBlock(bb->getName());
                bb->setAlter(mybb);
                mybb->setDepth(bb->getDepth());
            }
            for (auto iter = bblist.begin(); iter != bblist.end(); iter++)
            {
                BasicBlock *bb = iter->get();
                BasicBlock *mybb = dynamic_cast<BasicBlock *>(bb->getAlter());
                builder.setPosition(mybb, mybb->end());
                if (iter == bblist.begin())
                {
                    auto entry_args = bb->getArguments();
                    for (auto i = entry_args.begin(); i != entry_args.end(); i++)
                    {
                        auto arg = i->get();
                        auto my_arg = mybb->createArgument(arg->getType(), vector<int>(arg->getDims().begin(), arg->getDims().end()), arg->getName());
                        arg->setAlter(my_arg);
                    }
                }
                // 为新BB设置前驱后继关系
                for (auto p : bb->getPredecessors())
                    mybb->getPredecessors().push_back(dynamic_cast<BasicBlock *>(p->getAlter()));
                for (auto s : bb->getSuccessors())
                    mybb->getSuccessors().push_back(dynamic_cast<BasicBlock *>(s->getAlter()));
                map<Instruction *, ConstantValue *> In(bb->ConstIn.begin(), bb->ConstIn.end());
                map<Value *, map<vector<Value *>, ConstantValue *>> vIn;
                for (auto iter : bb->vConstIn)
                {
                    Value *value = iter.first.first;
                    vector<Value *> indices = iter.first.second;
                    ConstantValue *Const = iter.second;
                    vIn[value][indices] = Const;
                }
                for (auto iter = bb->begin(); iter != bb->end(); iter++)
                {
                    auto instr = iter->get();
                    if (isa<LoadInst>(instr))
                    {
                        LoadInst *ldInst = dynamic_cast<LoadInst *>(instr);
                        auto pointer = ldInst->getPointer();
                        auto indices = vector<Value *>(ldInst->getIndices().begin(), ldInst->getIndices().end());
                        if (vIn.find(pointer) != vIn.end() && vIn[pointer].find(indices) != vIn[pointer].end())
                        {
                            In[ldInst] = vIn[pointer][indices];
                            instr->setAlter(In[ldInst]);
                        }
                        else
                        {
                            vector<Value *> my_indices;
                            for (int i = 0; i < indices.size(); i++)
                            {
                                if (isa<Instruction>(indices[i]) && In.find(dynamic_cast<Instruction *>(indices[i])) != In.end())
                                    my_indices.push_back(In[dynamic_cast<Instruction *>(indices[i])]);
                                else
                                    my_indices.push_back(indices[i]->getAlter());
                            }
                            auto my_ldInst = builder.createLoadInst(pointer->getAlter(), my_indices);
                            instr->setAlter(my_ldInst);
                        }
                    }
                    else if (isa<StoreInst>(instr))
                    {
                        StoreInst *stInst = dynamic_cast<StoreInst *>(instr);
                        auto pointer = stInst->getPointer();
                        auto indices = vector<Value *>(stInst->getIndices().begin(), stInst->getIndices().end());
                        auto value = stInst->getValue();
                        if (isa<ConstantValue>(value) && !isa<GlobalValue>(pointer))
                            vIn[pointer][indices] = dynamic_cast<ConstantValue *>(value);
                        else
                        {
                            Instruction *inst = dynamic_cast<Instruction *>(value);
                            if (In.find(inst) != In.end() && !isa<GlobalValue>(pointer))
                            {
                                vIn[pointer][indices] = In[inst];
                                value = In[inst];
                            }
                            else if (vIn.find(pointer) != vIn.end() && vIn[pointer].find(indices) != vIn[pointer].end())
                                vIn[pointer].erase(vIn[pointer].find(indices));
                        }
                        vector<Value *> my_indices;
                        for (int i = 0; i < indices.size(); i++)
                        {
                            if (isa<Instruction>(indices[i]) && In.find(dynamic_cast<Instruction *>(indices[i])) != In.end())
                                my_indices.push_back(In[dynamic_cast<Instruction *>(indices[i])]);
                            else
                                my_indices.push_back(indices[i]->getAlter());
                        }
                        auto my_stInst = builder.createStoreInst(value->getAlter(), pointer->getAlter(), my_indices);
                    }
                    else if (isa<BinaryInst>(instr))
                    {
                        BinaryInst *bInst = dynamic_cast<BinaryInst *>(instr);
                        auto lhs = bInst->getLhs();
                        auto rhs = bInst->getRhs();
                        if (isa<Instruction>(lhs) && In.find(dynamic_cast<Instruction *>(lhs)) != In.end())
                            lhs = In[dynamic_cast<Instruction *>(lhs)];
                        if (isa<Instruction>(rhs) && In.find(dynamic_cast<Instruction *>(rhs)) != In.end())
                            rhs = In[dynamic_cast<Instruction *>(rhs)];
                        auto kind = bInst->getKind();
                        Type *type = bInst->getType();
                        if (isa<ConstantValue>(lhs) && isa<ConstantValue>(rhs))
                        {
                            auto lConst = dynamic_cast<ConstantValue *>(lhs);
                            auto rConst = dynamic_cast<ConstantValue *>(rhs);
                            int lint, rint;
                            double ldouble, rdouble;
                            if (type->isInt())
                            {
                                lint = lConst->getInt();
                                rint = rConst->getInt();
                            }
                            else if (type->isFloat())
                            {
                                ldouble = lConst->getDouble();
                                rdouble = rConst->getDouble();
                            }
                            if (kind == Value::kAdd)
                                In[bInst] = ConstantValue::get(lint + rint);
                            else if (kind == Value::kSub)
                                In[bInst] = ConstantValue::get(lint - rint);
                            else if (kind == Value::kMul)
                                In[bInst] = ConstantValue::get(lint * rint);
                            else if (kind == Value::kDiv)
                                In[bInst] = ConstantValue::get(lint / rint);
                            else if (kind == Value::kRem)
                                In[bInst] = ConstantValue::get(lint % rint);
                            else if (kind == Value::kFAdd)
                                In[bInst] = ConstantValue::get(ldouble + rdouble);
                            else if (kind == Value::kFSub)
                                In[bInst] = ConstantValue::get(ldouble - rdouble);
                            else if (kind == Value::kFMul)
                                In[bInst] = ConstantValue::get(ldouble * rdouble);
                            else if (kind == Value::kFDiv)
                                In[bInst] = ConstantValue::get(ldouble / rdouble);
                            if (kind == Value::kAdd || kind == Value::kSub || kind == Value::kMul || kind == Value::kDiv || kind == Value::kRem || kind == Value::kFAdd || kind == Value::kFSub || kind == Value::kFMul || kind == Value::kFDiv)
                                instr->setAlter(In[bInst]);
                            else
                            {
                                auto my_bInst = builder.createBinaryInst(kind, bInst->getType(), lhs->getAlter(), rhs->getAlter());
                                instr->setAlter(my_bInst);
                            }
                        }
                        else
                        {
                            auto my_bInst = builder.createBinaryInst(kind, bInst->getType(), lhs->getAlter(), rhs->getAlter());
                            instr->setAlter(my_bInst);
                        }
                    }
                    else if (isa<UnaryInst>(instr))
                    {
                        UnaryInst *uInst = dynamic_cast<UnaryInst *>(instr);
                        auto hs = uInst->getOperand();
                        auto kind = uInst->getKind();
                        Type *type = uInst->getType();
                        if (isa<Instruction>(hs) && In.find(dynamic_cast<Instruction *>(hs)) != In.end())
                            hs = In[dynamic_cast<Instruction *>(hs)];
                        if (isa<ConstantValue>(hs))
                        {
                            auto Const = dynamic_cast<ConstantValue *>(hs);
                            if (kind == Value::kNeg)
                                In[uInst] = ConstantValue::get(-Const->getInt());
                            else if (kind == Value::kFNeg)
                                In[uInst] = ConstantValue::get(-Const->getDouble());
                            else if (kind == Value::kItoF)
                                In[uInst] = ConstantValue::get((double)Const->getInt());
                            else if (kind == Value::kFtoI)
                                In[uInst] = ConstantValue::get((int)Const->getDouble());
                            instr->setAlter(In[uInst]);
                        }
                        else
                        {
                            auto my_uInst = builder.createUnaryInst(kind, type, hs->getAlter());
                            instr->setAlter(my_uInst);
                        }
                    }
                    else if (isa<CallInst>(instr))
                    {
                        CallInst *callInst = dynamic_cast<CallInst *>(instr);
                        auto args = callInst->getArguments();
                        vector<Value *> my_args;
                        for (auto iter = args.begin(); iter != args.end(); iter++)
                        {
                            if (isa<LoadInst>(*iter))
                            {
                                auto ldInst = dynamic_cast<LoadInst *>(*iter);
                                auto pointer = ldInst->getPointer();
                                auto indices = vector<Value *>(ldInst->getIndices().begin(), ldInst->getIndices().end());
                                int numdims = 0;
                                if (isa<GlobalValue>(pointer))
                                    numdims = dynamic_cast<GlobalValue *>(pointer)->getNumDims();
                                else if (isa<AllocaInst>(pointer))
                                    numdims = dynamic_cast<AllocaInst *>(pointer)->getNumDims();
                                else if (isa<Argument>(pointer))
                                    numdims = dynamic_cast<Argument *>(pointer)->getNumDims();
                                if (indices.size() < numdims && vIn.find(pointer) != vIn.end())
                                {
                                    vIn.erase(vIn.find(pointer));
                                }
                            }
                            my_args.push_back(iter->getAlter());
                        }
                        auto my_callInst = builder.createCallInst(callInst->getCallee(), my_args);
                        instr->setAlter(my_callInst);
                    }
                    else if (isa<CondBrInst>(instr))
                    {
                        auto cbInst = dynamic_cast<CondBrInst *>(instr);
                        auto cond = cbInst->getCondition();
                        if (isa<Instruction>(cond) && In.find(dynamic_cast<Instruction *>(cond)) != In.end())
                            cond = In[dynamic_cast<Instruction *>(cond)];
                        BasicBlock *my_then = dynamic_cast<BasicBlock *>(cbInst->getThenBlock()->getAlter());
                        BasicBlock *my_else = dynamic_cast<BasicBlock *>(cbInst->getElseBlock()->getAlter());
                        builder.createCondBrInst(cond->getAlter(), my_then, my_else, {}, {});
                    }
                    else if (isa<UncondBrInst>(instr))
                    {
                        auto ucbInst = dynamic_cast<UncondBrInst *>(instr);
                        auto my_block = dynamic_cast<BasicBlock *>(ucbInst->getBlock()->getAlter());
                        builder.createUncondBrInst(my_block, {});
                    }
                    else if (isa<ReturnInst>(instr))
                    {
                        auto rInst = dynamic_cast<ReturnInst *>(instr);
                        auto retValue = rInst->getReturnValue();
                        if (retValue && isa<Instruction>(retValue) && In.find(dynamic_cast<Instruction *>(retValue)) != In.end())
                            retValue = In[dynamic_cast<Instruction *>(retValue)];
                        auto my_retValue = retValue ? retValue->getAlter() : nullptr;
                        auto my_rInst = builder.createReturnInst(my_retValue, mybb);
                        rInst->setAlter(my_rInst);
                    }
                    else if (isa<AllocaInst>(instr))
                    {
                        auto allocaInst = dynamic_cast<AllocaInst *>(instr);
                        auto my_dims = vector<Value *>(allocaInst->getDims().begin(), allocaInst->getDims().end());
                        auto my_allocaInst = builder.createAllocaInst(allocaInst->getType(), my_dims, allocaInst->getName(), allocaInst->Const());
                        allocaInst->setAlter(my_allocaInst);
                    }
                }
            }
        }
    }
    // InstrCombine
    Module *InstrCombine::Run()
    {
        RegenerateIR();
        return pModule;
    }
    void InstrCombine::RegenerateIR()
    {
        // 生成全局变量
        auto global_values = OriginModule->getGlobalValues();
        for (auto iter = global_values->begin(); iter != global_values->end(); iter++)
        {
            GlobalValue *glbvl = iter->second;
            pModule->addGlobalValue(glbvl);
        }
        // 生成函数
        auto functions = OriginModule->getFunctions();
        for (auto iter = functions->begin(); iter != functions->end(); iter++)
        {
            Function *func = iter->second;
            Function *myFunc = pModule->createFunction(func->getName(), func->getType());
            auto bblist = func->getBasicBlocks();
            if (bblist.empty())
                continue;
            // 生成该函数所有新BB
            for (auto iter = bblist.begin(); iter != bblist.end(); iter++)
            {
                BasicBlock *bb = iter->get();
                BasicBlock *mybb = myFunc->addBasicBlock(bb->getName());
                bb->setAlter(mybb);
                mybb->setDepth(bb->getDepth());
            }
            for (auto iter = bblist.begin(); iter != bblist.end(); iter++)
            {
                BasicBlock *bb = iter->get();
                BasicBlock *mybb = dynamic_cast<BasicBlock *>(bb->getAlter());
                builder.setPosition(mybb, mybb->end());
                if (iter == bblist.begin())
                {
                    auto entry_args = bb->getArguments();
                    for (auto i = entry_args.begin(); i != entry_args.end(); i++)
                    {
                        auto arg = i->get();
                        auto my_arg = mybb->createArgument(arg->getType(), vector<int>(arg->getDims().begin(), arg->getDims().end()), arg->getName());
                        arg->setAlter(my_arg);
                    }
                }
                // 为新BB设置前驱后继关系
                for (auto p : bb->getPredecessors())
                    mybb->getPredecessors().push_back(dynamic_cast<BasicBlock *>(p->getAlter()));
                for (auto s : bb->getSuccessors())
                    mybb->getSuccessors().push_back(dynamic_cast<BasicBlock *>(s->getAlter()));
                for (auto iter = bb->begin(); iter != bb->end(); iter++)
                {
                    auto instr = iter->get();
                    if (isa<LoadInst>(instr))
                    {
                        LoadInst *ldInst = dynamic_cast<LoadInst *>(instr);
                        auto pointer = ldInst->getPointer();
                        auto indices = vector<Value *>(ldInst->getIndices().begin(), ldInst->getIndices().end());
                        vector<Value *> my_indices;
                        for (int i = 0; i < indices.size(); i++)
                        {
                            my_indices.push_back(indices[i]->getAlter());
                        }
                        auto my_ldInst = builder.createLoadInst(pointer->getAlter(), my_indices);
                        instr->setAlter(my_ldInst);
                    }
                    else if (isa<StoreInst>(instr))
                    {
                        StoreInst *stInst = dynamic_cast<StoreInst *>(instr);
                        auto pointer = stInst->getPointer();
                        auto indices = vector<Value *>(stInst->getIndices().begin(), stInst->getIndices().end());
                        auto value = stInst->getValue();
                        vector<Value *> my_indices;
                        for (int i = 0; i < indices.size(); i++)
                        {
                            my_indices.push_back(indices[i]->getAlter());
                        }
                        auto my_stInst = builder.createStoreInst(value->getAlter(), pointer->getAlter(), my_indices);
                    }
                    else if (isa<BinaryInst>(instr))
                    {
                        BinaryInst *bInst = dynamic_cast<BinaryInst *>(instr);
                        auto lhs = bInst->getLhs();
                        auto rhs = bInst->getRhs();
                        auto kind = bInst->getKind();
                        Type *type = bInst->getType();
                        auto my_lhs = lhs->getAlter();
                        auto my_rhs = rhs->getAlter();
                        // X +- 0 => X
                        if ((kind == Value::kAdd || kind == Value::kSub) && isa<ConstantValue>(my_rhs) && dynamic_cast<ConstantValue *>(my_rhs)->getInt() == 0)
                        {
                            instr->setAlter(my_lhs);
                        }
                        // 0 + X => X
                        else if (kind == Value::kAdd && isa<ConstantValue>(my_lhs) && dynamic_cast<ConstantValue *>(my_lhs)->getInt() == 0)
                        {
                            instr->setAlter(my_rhs);
                        }
                        // (X + C1) + C2 => X + (C1 + C2)
                        else if (kind == Value::kAdd && isa<BinaryInst>(my_lhs) && isa<ConstantValue>(my_rhs))
                        {
                            BinaryInst *bLhs = dynamic_cast<BinaryInst *>(my_lhs);
                            auto my_kind = bLhs->getKind();
                            auto llhs = bLhs->getLhs();
                            auto rlhs = bLhs->getRhs();
                            if (my_kind == Value::kAdd && isa<Instruction>(llhs) && isa<ConstantValue>(rlhs))
                            {
                                int my_const = dynamic_cast<ConstantValue *>(rlhs)->getInt() + dynamic_cast<ConstantValue *>(my_rhs)->getInt();
                                auto my_bInst = builder.createAddInst(llhs, ConstantValue::get(my_const));
                                instr->setAlter(my_bInst);
                            }
                            else
                            {
                                auto my_bInst = builder.createAddInst(my_lhs, my_rhs);
                                instr->setAlter(my_bInst);
                            }
                        }
                        // X + X => X * 2
                        else if (kind == Value::kAdd && (my_lhs == my_rhs))
                        {
                            auto my_bInst = builder.createMulInst(my_lhs, ConstantValue::get(2));
                            instr->setAlter(my_bInst);
                        }
                        // (X * C) + X => X * (C + 1)
                        else if (kind == Value::kAdd && isa<BinaryInst>(my_lhs) && isa<Instruction>(my_rhs))
                        {
                            BinaryInst *bLhs = dynamic_cast<BinaryInst *>(my_lhs);
                            auto my_kind = bLhs->getKind();
                            auto llhs = bLhs->getLhs();
                            auto rlhs = bLhs->getRhs();
                            if (my_kind == Value::kMul && isa<Instruction>(llhs) && llhs == my_rhs && isa<ConstantValue>(rlhs))
                            {
                                int my_const = dynamic_cast<ConstantValue *>(rlhs)->getInt() + 1;
                                auto my_bInst = builder.createMulInst(llhs, ConstantValue::get(my_const));
                                instr->setAlter(my_bInst);
                            }
                            else
                            {
                                auto my_bInst = builder.createAddInst(my_lhs, my_rhs);
                                instr->setAlter(my_bInst);
                            }
                        }
                        // (X + C1) - C2 => X + (C1 - C2)
                        else if (kind == Value::kSub && isa<BinaryInst>(my_lhs) && isa<ConstantValue>(my_rhs))
                        {
                            BinaryInst *bLhs = dynamic_cast<BinaryInst *>(my_lhs);
                            auto my_kind = bLhs->getKind();
                            auto llhs = bLhs->getLhs();
                            auto rlhs = bLhs->getRhs();
                            if (my_kind == Value::kAdd && isa<Instruction>(llhs) && isa<ConstantValue>(rlhs))
                            {
                                int my_const = dynamic_cast<ConstantValue *>(rlhs)->getInt() - dynamic_cast<ConstantValue *>(my_rhs)->getInt();
                                if (my_const == 0)
                                    instr->setAlter(llhs);
                                else
                                {
                                    auto my_bInst = builder.createAddInst(llhs, ConstantValue::get(my_const));
                                    instr->setAlter(my_bInst);
                                }
                            }
                            else
                            {
                                auto my_bInst = builder.createSubInst(my_lhs, my_rhs);
                                instr->setAlter(my_bInst);
                            }
                        }
                        else
                        {
                            auto my_bInst = builder.createBinaryInst(kind, bInst->getType(), lhs->getAlter(), rhs->getAlter());
                            instr->setAlter(my_bInst);
                        }
                    }

                    else if (isa<UnaryInst>(instr))
                    {
                        UnaryInst *uInst = dynamic_cast<UnaryInst *>(instr);
                        auto hs = uInst->getOperand();
                        auto kind = uInst->getKind();
                        Type *type = uInst->getType();
                        auto my_uInst = builder.createUnaryInst(kind, type, hs->getAlter());
                        instr->setAlter(my_uInst);
                    }
                    else if (isa<CallInst>(instr))
                    {
                        CallInst *callInst = dynamic_cast<CallInst *>(instr);
                        auto args = callInst->getArguments();
                        vector<Value *> my_args;
                        for (auto iter = args.begin(); iter != args.end(); iter++)
                        {
                            my_args.push_back(iter->getAlter());
                        }
                        auto my_callInst = builder.createCallInst(callInst->getCallee(), my_args);
                        instr->setAlter(my_callInst);
                    }
                    else if (isa<CondBrInst>(instr))
                    {
                        auto cbInst = dynamic_cast<CondBrInst *>(instr);
                        auto cond = cbInst->getCondition();
                        BasicBlock *my_then = dynamic_cast<BasicBlock *>(cbInst->getThenBlock()->getAlter());
                        BasicBlock *my_else = dynamic_cast<BasicBlock *>(cbInst->getElseBlock()->getAlter());
                        builder.createCondBrInst(cond->getAlter(), my_then, my_else, {}, {});
                    }
                    else if (isa<UncondBrInst>(instr))
                    {
                        auto ucbInst = dynamic_cast<UncondBrInst *>(instr);
                        auto my_block = dynamic_cast<BasicBlock *>(ucbInst->getBlock()->getAlter());
                        builder.createUncondBrInst(my_block, {});
                    }
                    else if (isa<ReturnInst>(instr))
                    {
                        auto rInst = dynamic_cast<ReturnInst *>(instr);
                        auto retValue = rInst->getReturnValue();
                        auto my_retValue = retValue ? retValue->getAlter() : nullptr;
                        auto my_rInst = builder.createReturnInst(my_retValue, mybb);
                        rInst->setAlter(my_rInst);
                    }
                    else if (isa<AllocaInst>(instr))
                    {
                        auto allocaInst = dynamic_cast<AllocaInst *>(instr);
                        auto my_dims = vector<Value *>(allocaInst->getDims().begin(), allocaInst->getDims().end());
                        auto my_allocaInst = builder.createAllocaInst(allocaInst->getType(), my_dims, allocaInst->getName(), allocaInst->Const());
                        allocaInst->setAlter(my_allocaInst);
                    }
                }
            }
        }
    }
    // Inline
    Module *Inline::Run()
    {
        auto functions = OriginModule->getFunctions();
        for (auto iter = functions->begin(); iter != functions->end(); iter++)
        {
            Function *func = iter->second;
            string funcname = iter->first;
            if (funcname == "getint" || funcname == "getch" || funcname == "getfloat" || funcname == "getarray" || funcname == "getfarray" || funcname == "putint" || funcname == "putch" || funcname == "putfloat" || funcname == "putarray" || funcname == "putfarray" || funcname == "starttime" || funcname == "stoptime" || funcname == "putf")
                continue;
            AnalyzeFunc(func);
        }
        RegenerateIR();
        return pModule;
    }
    void Inline::AnalyzeFunc(Function *curFunc)
    {
        auto bblist = curFunc->getBasicBlocks();
        bool noCall = true;
        int instr_cnt = 0;          // 函数内指令个数
        int ret_cnt = 0;            // 函数内return指令个数
        int bb_cnt = bblist.size(); // 函数基本块个数
        auto Entry = curFunc->getEntryBlock();
        auto args = Entry->getArguments();
        // 函数的参数不能有数组
        for (auto iter = args.begin(); iter != args.end(); iter++)
        {
            Argument *arg = iter->get();
            if (arg->getNumDims() > 0)
            {
                curFunc->Inline = false;
                return;
            }
        }
        for (auto iter = bblist.begin(); iter != bblist.end(); iter++)
        {
            BasicBlock *bb = iter->get();
            instr_cnt += bb->getNumInstructions();
            for (auto iter1 = bb->begin(); iter1 != bb->end(); iter1++)
            {
                Instruction *instr = iter1->get();
                if (isa<CallInst>(instr))
                    noCall = false;
                else if (isa<ReturnInst>(instr))
                    ret_cnt++;
            }
        }
        if (noCall && ret_cnt <= 1 && (instr_cnt < 50 || bb_cnt == 1))
            curFunc->Inline = true;
        else
            curFunc->Inline = false;
    }
    void Inline::RegenerateIR()
    {
        // 生成全局变量
        auto global_values = OriginModule->getGlobalValues();
        for (auto iter = global_values->begin(); iter != global_values->end(); iter++)
        {
            GlobalValue *glbvl = iter->second;
            auto name = glbvl->getName();
            auto type = glbvl->getType();
            pModule->addGlobalValue(glbvl);
        }
        // 生成函数
        auto functions = OriginModule->getFunctions();
        int InlineCnt = 0; // 整个module进行内联的次数
        for (auto iter = functions->begin(); iter != functions->end(); iter++)
        {
            Function *func = iter->second;
            Function *myFunc = pModule->createFunction(func->getName(), func->getType());
            auto bblist = func->getBasicBlocks();
            if (bblist.empty())
                continue;
            // 生成该函数所有新BB
            for (auto iter = bblist.begin(); iter != bblist.end(); iter++)
            {
                BasicBlock *bb = iter->get();
                BasicBlock *mybb = myFunc->addBasicBlock(bb->getName());
                bb->setAlter(mybb);
                mybb->setDepth(bb->getDepth());
            }
            int bb_cnt = 0; // 记录新增的基本块个数
            for (auto iter = bblist.begin(); iter != bblist.end(); iter++)
            {
                auto bb = iter->get();
                auto mybb = dynamic_cast<BasicBlock *>(bb->getAlter());
                mybb->setDepth(mybb->getDepth() + bb_cnt);
                builder.setPosition(mybb, mybb->end());
                if (iter == bblist.begin())
                {
                    auto entry_args = bb->getArguments();
                    for (auto i = entry_args.begin(); i != entry_args.end(); i++)
                    {
                        auto arg = i->get();
                        auto my_arg = mybb->createArgument(arg->getType(), vector<int>(arg->getDims().begin(), arg->getDims().end()), arg->getName());
                        arg->setAlter(my_arg);
                    }
                }
                // 扫描基本块的全部指令
                for (auto iter = bb->begin(); iter != bb->end(); iter++)
                {
                    auto instr = iter->get();
                    if (isa<LoadInst>(instr))
                    {
                        auto ldInst = dynamic_cast<LoadInst *>(instr);
                        auto pointer = ldInst->getPointer();
                        auto indices = vector<Value *>(ldInst->getIndices().begin(), ldInst->getIndices().end());
                        vector<Value *> my_indices;
                        for (int i = 0; i < indices.size(); i++)
                        {
                            my_indices.push_back(indices[i]->getAlter());
                        }
                        auto my_ldInst = builder.createLoadInst(pointer->getAlter(), my_indices);
                        ldInst->setAlter(my_ldInst);
                    }
                    else if (isa<StoreInst>(instr))
                    {
                        auto stInst = dynamic_cast<StoreInst *>(instr);
                        auto value = stInst->getValue();
                        auto pointer = stInst->getPointer();
                        auto indices = vector<Value *>(stInst->getIndices().begin(), stInst->getIndices().end());
                        vector<Value *> my_indices;
                        for (int i = 0; i < indices.size(); i++)
                        {
                            my_indices.push_back(indices[i]->getAlter());
                        }
                        auto my_stInst = builder.createStoreInst(value->getAlter(), pointer->getAlter(), my_indices);
                    }
                    else if (isa<BinaryInst>(instr))
                    {
                        auto bInst = dynamic_cast<BinaryInst *>(instr);
                        auto lhs = bInst->getLhs();
                        auto rhs = bInst->getRhs();
                        auto my_bInst = builder.createBinaryInst(bInst->getKind(), bInst->getType(), lhs->getAlter(), rhs->getAlter());
                        bInst->setAlter(my_bInst);
                    }
                    else if (isa<UnaryInst>(instr))
                    {
                        auto uInst = dynamic_cast<UnaryInst *>(instr);
                        auto hs = uInst->getOperand();
                        auto my_uInst = builder.createUnaryInst(uInst->getKind(), uInst->getType(), hs->getAlter());
                        uInst->setAlter(my_uInst);
                    }
                    else if (isa<CallInst>(instr))
                    {
                        auto callInst = dynamic_cast<CallInst *>(instr);
                        auto args = callInst->getArguments();
                        auto callee = callInst->getCallee();
                        if (callee->Inline == false)
                        {
                            vector<Value *> my_args;
                            for (auto iter = args.begin(); iter != args.end(); iter++)
                            {
                                my_args.push_back(iter->getAlter());
                            }
                            auto my_callInst = builder.createCallInst(callInst->getCallee(), my_args);
                            callInst->setAlter(my_callInst);
                            continue;
                        }
                        auto Inlinebbs = callee->getBasicBlocks();
                        auto InlineEntry = callee->getEntryBlock();
                        auto InlineArgs = InlineEntry->getArguments();
                        auto caller_iter = args.begin();
                        // 将参数转换为本地变量
                        for (auto iter = InlineArgs.begin(); iter != InlineArgs.end(); iter++)
                        {
                            Argument *arg = iter->get();
                            if (arg->getNumDims() > 0)
                                continue;
                            auto my_allocaInst = builder.createAllocaInst(arg->getType(), {}, arg->getName(), false);
                            arg->setAlter(my_allocaInst);
                        }
                        // 传递参数(store)
                        auto callee_iter = InlineArgs.begin();
                        for (auto caller_iter = args.begin(); caller_iter != args.end(); caller_iter++)
                        {
                            if (callee_iter->get()->getNumDims() > 0)
                            {
                                callee_iter++;
                                continue;
                            }
                            auto my_stInst = builder.createStoreInst(caller_iter->getAlter(), callee_iter->get()->getAlter(), {});
                            callee_iter++;
                        }
                        // 生成基本块
                        int CurDepth = builder.getBasicBlock()->getDepth();
                        for (auto iter = Inlinebbs.begin(); iter != Inlinebbs.end(); iter++)
                        {
                            if (iter == Inlinebbs.begin())
                                continue;
                            BasicBlock *bb = iter->get();
                            string my_name = callee->getName() + to_string(InlineCnt) + "_" + bb->getName();
                            BasicBlock *mybb = myFunc->addBasicBlock(my_name);
                            bb->setAlter(mybb);
                            mybb->setDepth(bb->getDepth() + CurDepth);
                            bb_cnt++;
                        }
                        InlineCnt++;
                        BasicBlock *exitblock = builder.getBasicBlock();
                        // 为内联函数生成指令
                        for (auto iter = Inlinebbs.begin(); iter != Inlinebbs.end(); iter++)
                        {
                            auto Inlinebb = iter->get();
                            if (iter != Inlinebbs.begin())
                                builder.setPosition(dynamic_cast<BasicBlock *>(Inlinebb->getAlter()), dynamic_cast<BasicBlock *>(Inlinebb->getAlter())->end());
                            for (auto iter1 = Inlinebb->begin(); iter1 != Inlinebb->end(); iter1++)
                            {
                                auto InlineInstr = iter1->get();
                                if (isa<LoadInst>(InlineInstr))
                                {
                                    auto ldInst = dynamic_cast<LoadInst *>(InlineInstr);
                                    auto pointer = ldInst->getPointer();
                                    auto indices = vector<Value *>(ldInst->getIndices().begin(), ldInst->getIndices().end());
                                    vector<Value *> my_indices;
                                    for (int i = 0; i < indices.size(); i++)
                                    {
                                        my_indices.push_back(indices[i]->getAlter());
                                    }
                                    auto my_ldInst = builder.createLoadInst(pointer->getAlter(), my_indices);
                                    ldInst->setAlter(my_ldInst);
                                }
                                else if (isa<StoreInst>(InlineInstr))
                                {
                                    auto stInst = dynamic_cast<StoreInst *>(InlineInstr);
                                    auto value = stInst->getValue();
                                    auto pointer = stInst->getPointer();
                                    auto indices = vector<Value *>(stInst->getIndices().begin(), stInst->getIndices().end());
                                    vector<Value *> my_indices;
                                    for (int i = 0; i < indices.size(); i++)
                                    {
                                        my_indices.push_back(indices[i]->getAlter());
                                    }
                                    auto my_stInst = builder.createStoreInst(value->getAlter(), pointer->getAlter(), my_indices);
                                }
                                else if (isa<BinaryInst>(InlineInstr))
                                {
                                    auto bInst = dynamic_cast<BinaryInst *>(InlineInstr);
                                    auto lhs = bInst->getLhs();
                                    auto rhs = bInst->getRhs();
                                    auto my_bInst = builder.createBinaryInst(bInst->getKind(), bInst->getType(), lhs->getAlter(), rhs->getAlter());
                                    bInst->setAlter(my_bInst);
                                }
                                else if (isa<UnaryInst>(InlineInstr))
                                {
                                    auto uInst = dynamic_cast<UnaryInst *>(InlineInstr);
                                    auto hs = uInst->getOperand();
                                    auto my_uInst = builder.createUnaryInst(uInst->getKind(), uInst->getType(), hs->getAlter());
                                    uInst->setAlter(my_uInst);
                                }
                                else if (isa<ReturnInst>(InlineInstr))
                                {
                                    auto rInst = dynamic_cast<ReturnInst *>(InlineInstr);
                                    auto retValue = rInst->getReturnValue();
                                    auto my_retValue = retValue ? retValue->getAlter() : nullptr;
                                    callInst->setAlter(my_retValue);
                                    exitblock = builder.getBasicBlock();
                                }
                                else if (isa<CondBrInst>(InlineInstr))
                                {
                                    auto cbInst = dynamic_cast<CondBrInst *>(InlineInstr);
                                    auto cond = cbInst->getCondition();
                                    BasicBlock *current = builder.getBasicBlock();
                                    BasicBlock *my_then = dynamic_cast<BasicBlock *>(cbInst->getThenBlock()->getAlter());
                                    BasicBlock *my_else = dynamic_cast<BasicBlock *>(cbInst->getElseBlock()->getAlter());
                                    current->getSuccessors().push_back(my_then);
                                    current->getSuccessors().push_back(my_else);
                                    my_then->getPredecessors().push_back(current);
                                    my_else->getPredecessors().push_back(current);
                                    builder.createCondBrInst(cond->getAlter(), my_then, my_else, {}, {});
                                }
                                else if (isa<UncondBrInst>(InlineInstr))
                                {
                                    auto ucbInst = dynamic_cast<UncondBrInst *>(InlineInstr);
                                    auto my_block = dynamic_cast<BasicBlock *>(ucbInst->getBlock()->getAlter());
                                    BasicBlock *current = builder.getBasicBlock();
                                    current->getSuccessors().push_back(my_block);
                                    builder.createUncondBrInst(my_block, {});
                                }
                                else if (isa<AllocaInst>(InlineInstr))
                                {
                                    auto allocaInst = dynamic_cast<AllocaInst *>(InlineInstr);
                                    auto my_dims = vector<Value *>(allocaInst->getDims().begin(), allocaInst->getDims().end());
                                    auto my_allocaInst = builder.createAllocaInst(allocaInst->getType(), my_dims, allocaInst->getName(), allocaInst->Const());
                                    allocaInst->setAlter(my_allocaInst);
                                }
                            }
                        }
                        builder.setPosition(exitblock, exitblock->end());
                    }
                    else if (isa<ReturnInst>(instr))
                    {
                        auto rInst = dynamic_cast<ReturnInst *>(instr);
                        auto retValue = rInst->getReturnValue();
                        auto my_retValue = retValue ? retValue->getAlter() : nullptr;
                        auto my_rInst = builder.createReturnInst(my_retValue, mybb);
                        rInst->setAlter(my_rInst);
                    }
                    else if (isa<CondBrInst>(instr))
                    {
                        auto cbInst = dynamic_cast<CondBrInst *>(instr);
                        auto cond = cbInst->getCondition();
                        BasicBlock *current = builder.getBasicBlock();
                        BasicBlock *my_then = dynamic_cast<BasicBlock *>(cbInst->getThenBlock()->getAlter());
                        BasicBlock *my_else = dynamic_cast<BasicBlock *>(cbInst->getElseBlock()->getAlter());
                        current->getSuccessors().push_back(my_then);
                        current->getSuccessors().push_back(my_else);
                        my_then->getPredecessors().push_back(current);
                        my_else->getPredecessors().push_back(current);
                        builder.createCondBrInst(cond->getAlter(), my_then, my_else, {}, {});
                    }
                    else if (isa<UncondBrInst>(instr))
                    {
                        auto ucbInst = dynamic_cast<UncondBrInst *>(instr);
                        auto my_block = dynamic_cast<BasicBlock *>(ucbInst->getBlock()->getAlter());
                        BasicBlock *current = builder.getBasicBlock();
                        current->getSuccessors().push_back(my_block);
                        my_block->getPredecessors().push_back(current);
                        builder.createUncondBrInst(my_block, {});
                    }
                    else if (isa<AllocaInst>(instr))
                    {
                        auto allocaInst = dynamic_cast<AllocaInst *>(instr);
                        auto my_dims = vector<Value *>(allocaInst->getDims().begin(), allocaInst->getDims().end());
                        auto my_allocaInst = builder.createAllocaInst(allocaInst->getType(), my_dims, allocaInst->getName(), allocaInst->Const());
                        allocaInst->setAlter(my_allocaInst);
                    }
                }
            }
        }
    }
    // LoopFind
    Module *LoopFind::Run()
    {
        // 计算必经节点
        auto functions = pModule->getFunctions();
        for (auto iter = functions->begin(); iter != functions->end(); iter++)
        {
            Function *func = iter->second;
            auto bblist = func->getBasicBlocks();
            if (bblist.empty())
                continue;
            CalDom(func);
        }
        // 计算可达节点集合
        CalAccess();
        // 计算一个函数内所有循环
        GetLoop();
        return pModule;
    }
    void LoopFind::CalAccess()
    {
        auto functions = pModule->getFunctions();
        int i = 0;
        for (auto iter = functions->begin(); iter != functions->end(); iter++, i++)
        {
            string name = iter->first;
            auto bblist = iter->second->getBasicBlocks();
            if (bblist.empty())
                continue;
            stack<BasicBlock *> s;
            map<BasicBlock *, int> vis;
            // set<BasicBlock *> accessible;
            for (auto biter = bblist.begin(); biter != bblist.end(); biter++)
            {
                auto bb = biter->get();
                // cout << "bb: " << bb->getName() << endl;
                // cout << "bb's fitst successor: " << (*suc.begin())->getName() << endl;
                vis.clear();
                bb->accessible.clear();
                for (auto biter = bblist.begin(); biter != bblist.end(); biter++)
                {
                    vis.insert(map<BasicBlock *, int>::value_type(biter->get(), 0));
                }
                vis[bb] = 1;
                s.push(bb);
                while (!s.empty())
                {
                    BasicBlock *top = s.top();
                    s.pop();
                    // cout << (top)->getName() << endl;
                    bb->accessible.insert(top);
                    auto Sucs = top->getSuccessors();
                    if (Sucs.empty())
                        continue;
                    for (auto iter = Sucs.begin(); iter != Sucs.end(); iter++)
                    {
                        if (vis[*iter] == 0)
                        {
                            s.push(*iter);
                            vis[*iter] = 1;
                        }
                    }
                }
            }
        }
    }
    void LoopFind::GetLoop()
    {
        auto functions = pModule->getFunctions();
        int i = 0;
        // 找出所有回边，<b,a>为由b指向a的边。
        vector<pair<BasicBlock *, BasicBlock *>> back_edge;
        for (auto iter = functions->begin(); iter != functions->end(); iter++, i++)
        {
            back_edge.clear();
            auto bblist = iter->second->getBasicBlocks();
            for (auto biter = bblist.begin(); biter != bblist.end(); biter++)
            {
                auto bb = biter->get();
                auto sucs = bb->getSuccessors();
                for (auto siter = sucs.begin(); siter != sucs.end(); siter++)
                {
                    if (bb->dom.find(*siter) != bb->dom.end())
                    {
                        back_edge.push_back(make_pair(bb, (*siter)));
                    }
                }
            }
            // 找出经过循环首节点的所有节点
            for (auto be : back_edge)
            {
                vector<BasicBlock *> loop;
                vector<BasicBlock *> pass_snode;
                pass_snode.clear();
                auto node = be.first;
                auto snode = be.second;
                for (auto biter = bblist.begin(); biter != bblist.end(); biter++)
                {
                    auto bb = biter->get();
                    if (bb->dom.find(snode) != bb->dom.end())
                    {
                        pass_snode.push_back(bb);
                    }
                }
                for (auto pnode : pass_snode)
                {
                    if (pnode->accessible.find(node) != pnode->accessible.end())
                    {
                        loop.push_back(pnode);
                    }
                }
                Loop Loop(loop, node, snode);
                iter->second->Loops.push_back(Loop);
                iter->second->addLoop(loop);
            }
        }
    }
    void LoopFind::PRINT_ACCESS(std::ostream &os)
    {
        auto functions = pModule->getFunctions();
        int i = 0;
        for (auto iter = functions->begin(); iter != functions->end(); iter++, i++)
        {
            string name = iter->first;
            auto bblist = iter->second->getBasicBlocks();
            if (bblist.empty())
                continue;
            os << "*******" << name << "*******\n";
            for (auto biter = bblist.begin(); biter != bblist.end(); biter++)
            {
                auto bb = biter->get();
                auto sucs = bb->getSuccessors();
                // cout << "basic_block: " << bb->getName() << endl;
                cout << "accssible set of " << bb->getName() << ":" << endl;
                // cout << "bb's fitst successor: " << (*suc.begin())->getName() << endl;
                for (auto acc : bb->accessible)
                {
                    cout << acc->getName() << " ";
                }
                cout << endl
                     << endl;
            }
        }
    }
    void LoopFind::CalDom(Function *curFunc)
    {
        // 初始化
        auto bblist = curFunc->getBasicBlocks();
        Allbbs.clear();
        for (auto iter = bblist.begin(); iter != bblist.end(); iter++)
            Allbbs.insert(iter->get());
        for (auto iter = bblist.begin(); iter != bblist.end(); iter++)
        {
            BasicBlock *bb = iter->get();
            // bb->dom.clear();
            if (iter == bblist.begin())
                bb->dom.insert(bb);
            else
                bb->dom.insert(Allbbs.begin(), Allbbs.end());
        }
        // 求解不动点
        bool change = true;
        while (change)
        {
            change = false;
            for (auto iter = bblist.begin(); iter != bblist.end(); iter++)
            {
                if (iter == bblist.begin())
                    continue;
                BasicBlock *bb = iter->get();
                set<BasicBlock *> olddom, temp, t;
                olddom.insert(bb->dom.begin(), bb->dom.end());
                bb->dom.clear();
                auto pres = bb->getPredecessors();
                for (int i = 0; i < pres.size(); i++)
                {
                    if (i == 0)
                        temp.insert(pres[0]->dom.begin(), pres[0]->dom.end());
                    else
                    {
                        set_intersection(temp.begin(), temp.end(), pres[i]->dom.begin(), pres[i]->dom.end(), inserter(t, t.begin()));
                        temp.clear();
                        temp.insert(t.begin(), t.end());
                        t.clear();
                    }
                }
                bb->dom.insert(temp.begin(), temp.end());
                bb->dom.insert(bb);
                if (olddom != bb->dom)
                    change = true;
            }
        }
    }
    void LoopFind::PRINT_DOM(std::ostream &os)
    {
        auto functions = pModule->getFunctions();
        for (auto iter = functions->begin(); iter != functions->end(); iter++)
        {
            string name = iter->first;
            auto bblist = iter->second->getBasicBlocks();
            if (bblist.empty())
                continue;
            os << "**********" << name << "**********"
               << "\n";
            for (auto biter = bblist.begin(); biter != bblist.end(); biter++)
            {
                auto bb = biter->get();
                os << "$$" << bb->getName() << "$$"
                   << "\n";
                os << "[DOM]"
                   << "\n";
                for (auto &k : bb->dom)
                {
                    os << k->getName() << " ";
                }
                os << "\n";
            }
        }
    }
    void LoopFind::PRINT_LOOP(std::ostream &os)
    {
        auto functions = pModule->getFunctions();
        for (auto iter = functions->begin(); iter != functions->end(); iter++)
        {
            string name = iter->first;
            auto bblist = iter->second->getBasicBlocks();
            if (bblist.empty())
                continue;
            os << "**********" << name << "**********"
               << "\n";
            // set<Loop *> loops = iter->second->Loops;
            for (auto loop : iter->second->Loops)
            {
                os << "loop's blocks are:" << endl;
                for (auto b : loop.getLoopBasicBlocks())
                {
                    os << b->getName() << " ";
                }
                os << endl;
                os << "loop's header is:" << endl
                   << loop.getHeader()->getName() << endl;
                os << "loop's latch is:" << endl
                   << loop.getLatchBlock()->getName() << endl;
                os << "loop's exitingblocks are:" << endl;
                for (auto b : loop.getExitingBlocks())
                {
                    os << b->getName() << " ";
                }
                os << endl;
                os << "loop's exitblocks are:" << endl;
                for (auto b : loop.getExitBlocks())
                {
                    os << b->getName() << " ";
                }
                os << endl;
            }
        }
    }

    // LoopUnroll
    Module *LoopUnroll::Run()
    {
        auto functions = pModule->getFunctions();
        for (auto iter = functions->begin(); iter != functions->end(); iter++)
        {
            auto func = iter->second;
            for (auto loop : func->Loops)
            {
                if (IsUnrollable(loop))
                {
                    Unroll(loop);
                }
            }
        }
    }
    bool LoopUnroll::IsUnrollable(Loop curLoop)
    {
        if (curLoop.getLoopBasicBlocks().size() > 2)
            return false;
        BasicBlock *header = curLoop.getHeader();
        BasicBlock *preheader = curLoop.getPreHeader();
        auto iter = header->getInstructions().end();
        iter--;
        CondBrInst *condbrInst = dynamic_cast<CondBrInst *>(iter->get());
        auto cond = condbrInst->getCondition();
        auto kind = cond->getKind();
        if (kind != Instruction::kICmpLE)
            return false;
        auto ltInst = dynamic_cast<BinaryInst *>(cond);
        auto lhs = ltInst->getLhs();
        auto rhs = ltInst->getRhs();
        if (isa<Instruction>(lhs) && isa<ConstantValue>(rhs))
        {
            // int iter_cnt = dynamic_cast<ConstantValue *>(rhs)->getInt();
            return true;
        }
        return false;
    }
    void LoopUnroll::ModifyIR()
    {
    }
    void LoopUnroll::Unroll(Loop curLoop)
    {
        BasicBlock *header = curLoop.getHeader();
        BasicBlock *preheader = curLoop.getPreHeader();
        BasicBlock *body, *exitblock;
        for (auto s : header->getSuccessors())
        {
            if (s->getName()[0] == 'e')
                exitblock = s;
            else if (s->getName()[0] == 'b')
                body = s;
        }
        auto iter = header->getInstructions().rbegin();
        CondBrInst *condbrInst = dynamic_cast<CondBrInst *>(iter->get());
        auto cond = condbrInst->getCondition();
        auto kind = cond->getKind();
        auto ltInst = dynamic_cast<BinaryInst *>(cond);
        auto lhs = ltInst->getLhs();
        auto rhs = ltInst->getRhs();
        int iter_cnt = dynamic_cast<ConstantValue *>(rhs)->getInt();
        // delete "br ^header" in preheader
        iter = preheader->getInstructions().rbegin();
        preheader->getInstructions().erase(--iter.base());
        // 将body中的指令插入preheader后,iter_cnt遍
        builder.setPosition(preheader, preheader->end());
        while (iter_cnt--)
        {
            for (auto &iiter : body->getInstructions())
            {
                auto instr = iiter.get();
                if (isa<LoadInst>(instr))
                {
                    auto ldInst = dynamic_cast<LoadInst *>(instr);
                    auto pointer = ldInst->getPointer();
                    auto indices = vector<Value *>(ldInst->getIndices().begin(), ldInst->getIndices().end());
                    vector<Value *> my_indices;
                    for (int i = 0; i < indices.size(); i++)
                        my_indices.push_back(indices[i]->getAlter());
                    auto my_ldInst = builder.createLoadInst(pointer->getAlter(), my_indices);
                    ldInst->setAlter(my_ldInst);
                }
                else if (isa<StoreInst>(instr))
                {
                    auto stInst = dynamic_cast<StoreInst *>(instr);
                    auto pointer = stInst->getPointer();
                    auto value = stInst->getValue();
                    auto indices = vector<Value *>(stInst->getIndices().begin(), stInst->getIndices().end());
                    vector<Value *> my_indices;
                    for (int i = 0; i < indices.size(); i++)
                        my_indices.push_back(indices[i]->getAlter());
                    auto my_stInst = builder.createStoreInst(value->getAlter(), pointer->getAlter(), my_indices);
                }
                else if (isa<BinaryInst>(instr))
                {
                    auto bInst = dynamic_cast<BinaryInst *>(instr);
                    auto lhs = bInst->getLhs();
                    auto rhs = bInst->getRhs();
                    auto my_bInst = builder.createBinaryInst(bInst->getKind(), bInst->getType(), lhs->getAlter(), rhs->getAlter());
                    bInst->setAlter(my_bInst);
                }
            }
        }
        // insert "br ^exit" in  preheader
        auto my_uncondbrInst = builder.createUncondBrInst(exitblock, {});
        // preheader<->exitblock
        preheader->getSuccessors().clear();
        preheader->getSuccessors().push_back(exitblock);
        exitblock->getPredecessors().clear();
        exitblock->getPredecessors().push_back(preheader);
        // clear header&body
        header->getPredecessors().clear();
        header->getSuccessors().clear();
        body->getPredecessors().clear();
        body->getSuccessors().clear();
    }
} // namespace sysy
