#include "codegen.hpp"

namespace backend
{
    using RegId = RegManager::RegId;

    string CodeGen::code_gen()
    {
        string code;
        code += module_gen(module);
        return code;
    }

    string CodeGen::module_gen(Module *module)
    {
        string code;
        string dataCode;
        string textCode;
        // clear last module's label record
        clearModuleRecord(module);
        code += space + ".arch armv7ve " + endl;
        code += space + ".text " + endl;
        // generate asmcode for all global values
        auto global_values = module->getGlobalValues();
        for (auto iter = global_values->begin(); iter != global_values->end(); ++iter)
        {
            GlobalValue *glbvl = iter->second;
            dataCode += globaldata_gen(glbvl);
        }
        // generate asmcode for all functions
        auto functions = module->getFunctions();

        for (auto iter = functions->begin(); iter != functions->end(); ++iter)
        {
            string name = iter->first;
            Function *func = iter->second;
            auto bblist = func->getBasicBlocks();
            if (bblist.empty())
                continue;
            // generate asmcode for each function
            textCode += function_gen(func) + endl;
        }
        code += (dataCode + textCode + endl);
        return code;
    }

    string CodeGen::functionHead_gen(Function *func)
    {
        string code;
        code += space + ".global " + func->getName() + endl;
        code += space + ".p2align " + std::to_string(int_p2align) + endl;
        code += space + ".type " + func->getName() + ", %function" + endl;
        code += func->getName() + ":" + endl;
        return code;
    }
    /**
     * stack structure:
     *
     * last function stack
     * --------------------------   <-- fp  point
     * callee preserved regs (include fp, sp, lr)
     * --------------------------
     * tempory variable(ir inst)
     * --------------------------
     * arg0,arg1,arg2,arg3(as tempory variable)
     * --------------------------
     * dynamic stack(by AllocaInst)  ********
     * --------------------------
     * caller preserved regs
     * --------------------------
     * arg4 ~ argX
     * --------------------------    <-- sp point
     * next function stack
     *
     */
    /**
     * prologue :
     *          preserve callee-saved register (lr, fp and other callee-saved regs)
     *          set new fp
     *          alloc stack space for local var/ args / return value.
     *          store args to stack
     * */
    string CodeGen::prologueCode_gen(Function *func)
    {
        string code;
        // if there is callinst in function
        bool haveCall = false;
        auto bbs = func->getBasicBlocks();
        for (auto iter = bbs.begin(); iter != bbs.end(); ++iter)
        {
            auto bb = iter->get();
            for (auto &instr : bb->getInstructions())
            {
                auto instrType = instr->getKind();
                if (instrType == Value::Kind::kCall)
                {
                    haveCall = true;
                    break;
                }
            }
        }
        // put arguments in the stack
        auto entry_block = func->getEntryBlock();
        auto args = entry_block->getArguments();
        int arg_num = 0;
        for (auto arg = args.begin(); arg != args.end(); ++arg)
        {
            // r0-r3
            if (arg_num < 4)
            {
                paramsStOffset.emplace(arg->get(), top_offset);
                code += space + "str\tr" + to_string(arg_num) + ", [fp, #" + to_string(top_offset) + "]" + endl;
                top_offset -= 4;
            }
            // other
            else
            {
                paramsStOffset.emplace(arg->get(), above_offset);
                above_offset += 4;
            }
            arg_num++;
        }
        if (max_param > 4)
            top_offset -= (max_param - 4) * 4;
        code = space + "sub\tsp, sp, #" + to_string(-top_offset - 4) + endl + code;
        if (haveCall)
        {
            // set fp
            code = space + "add\tfp, sp, #4" + endl + code;
            // push fp,lr
            code = space + "push\t{fp,lr}" + endl + code;
        }
        else
        {
            // set fp
            code = space + "add\tfp, sp, #0" + endl + code;
            // push fp
            code = space + "push\t{fp}" + endl + code;
        }
        return code;
    }

    /* epilogue :
     *          free stack space
     *          restore sp
     *          restore used callee-saved register(lr, fp and other callee-saved regs)
     *          bx lr
     */
    string CodeGen::epilogueCode_gen(Function *func)
    {
        string code;
        // if there is callinst in function
        bool haveCall = false;
        auto bbs = func->getBasicBlocks();
        for (auto iter = bbs.begin(); iter != bbs.end(); ++iter)
        {
            auto bb = iter->get();
            for (auto &instr : bb->getInstructions())
            {
                auto instrType = instr->getKind();
                if (instrType == Value::Kind::kCall)
                {
                    haveCall = true;
                    break;
                }
            }
        }
        if (haveCall)
            code += space + "pop\t{fp,lr}" + endl;
        else
            code += space + "pop\t{fp}" + endl;
        code += space + "bx\tlr" + endl;
        return code;
    }

    string CodeGen::function_gen(Function *func)
    {
        curFunc = func;
        clearFunctionRecord(func);
        string bbCode;
        auto bbs = func->getBasicBlocks();
        top_offset = -8;
        above_offset = 4;
        max_param = 0;
        backpatch.clear();
        for (auto iter = bbs.begin(); iter != bbs.end(); ++iter)
        {
            auto bb = iter->get();
            bbCode += basicBlock_gen(bb);
        }
        string code;
        string funcHead = functionHead_gen(func);
        string prologueCode = prologueCode_gen(func);
        string epilogueCode = epilogueCode_gen(func);
        string literalPoolsCode = literalPoolsCode_gen(func);
        // backpatch parasoffset
        int start_pos = 0;
        int index = 0;
        while ((start_pos = bbCode.find("unk", start_pos)) != std::string::npos)
        {
            int arg_offset = paramsStOffset[backpatch[index++]];
            bbCode.replace(start_pos, 3, to_string(arg_offset));
            start_pos += to_string(arg_offset).length();
        }
        code += funcHead + prologueCode + bbCode +
                epilogueCode + literalPoolsCode;

        return code;
    }

    string CodeGen::basicBlock_gen(BasicBlock *bb)
    {
        curBB = bb;
        // string bbLabel = bb->getName();
        string bbLabel = getBBLabel(bb);
        string code;
        code += bbLabel + ":" + endl;
        for (auto &instr : bb->getInstructions())
        {
            auto instrType = instr->getKind();
            code += instruction_gen(instr.get());
        }
        return code;
    }
    /**
     * RegId : binaryInst_gen returns RegId as its destination operand
     * code  : asmcode generated by binaryInst_gen
     */
    pair<RegId, string> CodeGen::binaryInst_gen(BinaryInst *bInst, RegId dstRegId)
    {
        string code;
        /**
         *code in here
         */
        string lname, rname;
        auto lhs = bInst->getLhs();
        if (isa<ConstantValue>(lhs))
            lname = to_string(dynamic_cast<ConstantValue *>(lhs)->getInt());
        else
            lname = "r" + lhs->getName();
        auto rhs = bInst->getRhs();
        if (isa<ConstantValue>(rhs))
            rname = to_string(dynamic_cast<ConstantValue *>(rhs)->getInt());
        else
            rname = "r" + rhs->getName();
        auto res = stoi(bInst->getName());
        if (bInst->getKind() == Instruction::kAdd)
            code += space + "add\tr" + to_string(res) + ", " + lname + ", " + rname + endl;
        else if (bInst->getKind() == Instruction::kSub)
            code += space + "sub\tr" + to_string(res) + ", " + lname + ", " + rname + endl;
        else if (bInst->getKind() == Instruction::kMul)
            code += space + "mul\tr" + to_string(res) + ", " + lname + ", " + rname + endl;
        else if (bInst->getKind() == Instruction::kDiv)
            code += space + "div\tr" + to_string(res) + ", " + lname + ", " + rname + endl;
        return {dstRegId, code};
    }

    pair<RegId, string> CodeGen::unaryInst_gen(UnaryInst *uInst, RegId dstRegId)
    {
        string code;
        /**
         *code in here
         */
        return {dstRegId, code};
    }
    pair<RegId, string>
    CodeGen::allocaInst_gen(AllocaInst *aInst, RegManager::RegId dstRegId)
    {
        string code;
        localVarStOffset.emplace(aInst, top_offset);
        top_offset -= 4;
        dstRegId = RegManager::RNONE;
        return {dstRegId, code};
    }

    string CodeGen::storeInst_gen(StoreInst *stInst)
    {
        string code;
        auto value = stInst->getValue();
        auto pointer = stInst->getPointer();
        int offset;
        if (localVarStOffset.find(dynamic_cast<Instruction *>(pointer)) != localVarStOffset.end())
            offset = localVarStOffset[dynamic_cast<Instruction *>(pointer)];
        else if (paramsStOffset.find(dynamic_cast<Argument *>(pointer)) != paramsStOffset.end())
            offset = paramsStOffset[dynamic_cast<Argument *>(pointer)];
        if (isa<ConstantValue>(value))
        {
            int constant_value = dynamic_cast<ConstantValue *>(value)->getInt();
            code += space + "mov\tr3, #" + to_string(constant_value) + endl;
            code += space + "str\tr3, [fp, #" + to_string(offset) + "]" + endl;
        }
        else
        {
            code += space + "str\tr" + value->getName() + ", [fp, #" + to_string(offset) + "]" + endl;
        }
        return code;
    }
    pair<RegId, string>
    CodeGen::loadInst_gen(LoadInst *ldInst, RegId dstRegId)
    {
        string code;
        // dst register
        int reg_num = dstRegId == RegManager::RANY ? stoi(ldInst->getName()) : dstRegId;
        // variable to be loaded
        auto var = ldInst->getPointer();
        // variable's position in the stack
        int pos;
        bool found = false;
        // find var in localvar
        // if (localVarStOffset.find(dynamic_cast<Instruction *>(var)) != localVarStOffset.end())
        //     pos = localVarStOffset[dynamic_cast<Instruction *>(var)];
        for (auto local_var : localVarStOffset)
        {
            // code += "load var in localvar\n";
            if (local_var.first == var)
            {
                pos = local_var.second;
                found = true;
                code += space + "ldr\tr" + to_string(reg_num) + ", [fp, #" + to_string(pos) + "]" + endl;
                break;
            }
        }
        // if var is not localvar but an argument
        if (not found)
        {
            code += space + "ldr\tr" + to_string(reg_num) + ", [fp, #unk]" + endl;
            backpatch.push_back(dynamic_cast<Argument *>(var));
        }
        // {
        //     code += "load var in paramvar\n";
        //     for (auto arg : paramsStOffset)
        //     {
        //         if (arg.first == var)
        //         {
        //             // pos = arg.second;
        //             break;
        //         }
        //     }
        // }
        return {dstRegId, code};
    }
    string CodeGen::returnInst_gen(ReturnInst *retInst)
    {
        string code;
        /**
         *code in here
         */
        return code;
    }
    string CodeGen::uncondBrInst_gen(UncondBrInst *ubInst)
    {
        string code;
        /**
         *code in here
         */
        return code;
    }
    string CodeGen::condBrInst_gen(CondBrInst *cbInst)
    {
        string code;
        /**
         *code in here
         */
        return code;
    }
    pair<RegId, string>
    CodeGen::callInst_gen(CallInst *callInst, RegId dstRegId)
    {
        string code;
        auto callee_fuc = callInst->getCallee();
        auto args = callInst->getArguments();
        RegId dst_reg = RegManager::R0;
        int arg_num = 0;
        int para_offset = 0;
        // the max number of all subfunctions' arguments
        max_param = args.size() > max_param ? args.size() : max_param;
        for (auto arg : args)
        {
            arg_num++;
            if (arg_num > 4)
            {
                int reg_num = stoi(arg->getName());
                if (para_offset == 0)
                    code += space + "str\tr" + to_string(reg_num) + ", [sp]" + endl;
                else
                    code += space + "str\tr" + to_string(reg_num) + ", [sp, #" + to_string(para_offset) + "]" + endl;
                para_offset += 4;
                continue;
            }
            // int src_reg = stoi(arg->getName()) + 4;
            int src_reg = stoi(arg->getName());
            code += space + "mov\tr" + to_string(arg_num - 1) + ", r" + to_string(src_reg) + endl;
            // code += space + "mov\tr" + to_string(arg_num - 1) + ", r" + src + endl;
        }
        code += space + "bl\t" + callee_fuc->getName() + endl;
        return {dstRegId, code};
    }

    string CodeGen::instruction_gen(Instruction *instr)
    {
        string code;
        string unkName = instr->getName();
        RegManager::RegId dstRegId = RegManager::RNONE;
        auto instrType = instr->getKind();
        pair<RegId, string> tmp;
        switch (instrType)
        {
        // binary inst
        case Instruction::kAdd:
        case Instruction::kMul:
        case Instruction::kSub:
        {
            BinaryInst *bInst = dynamic_cast<BinaryInst *>(instr);
            // registers are used only for instruction operation, consider use which register (any one that is free for use)
            tmp = binaryInst_gen(bInst, RegManager::RANY);
            code += tmp.second;
            dstRegId = tmp.first;
            break;
        }
        case Instruction::kLoad:
        {
            LoadInst *ldInst = dynamic_cast<LoadInst *>(instr);
            tmp = loadInst_gen(ldInst, RegManager::RANY);
            // code += M_emitComment("load inst");
            code += tmp.second;
            dstRegId = tmp.first;
            break;
        }
        case Instruction::kStore:
        {
            StoreInst *stInst = dynamic_cast<StoreInst *>(instr);
            // code += M_emitComment("store inst");
            code += storeInst_gen(stInst);
            return code;
            break;
        }
        case Instruction::kAlloca:
        {
            AllocaInst *aInst = dynamic_cast<AllocaInst *>(instr);
            tmp = allocaInst_gen(aInst, RegManager::RANY);
            // code += M_emitComment("alloca inst");
            code += tmp.second;
            dstRegId = tmp.first;
            break;
        }
        case Instruction::kReturn:
        {
            ReturnInst *retInst = dynamic_cast<ReturnInst *>(instr);
            code += M_emitComment("return inst");
            code += returnInst_gen(retInst);
            return code;
            break;
        }
        case Instruction::kCall:
        {
            CallInst *cInst = dynamic_cast<CallInst *>(instr);
            auto tmp = callInst_gen(cInst, RegManager::RANY);
            code += tmp.second;
            dstRegId = tmp.first;
            if (dstRegId == RegManager::R0)
                return code;
            break;
        }
        case Instruction::kBr:
        {
            UncondBrInst *ubInst = dynamic_cast<UncondBrInst *>(instr);
            code += uncondBrInst_gen(ubInst);
            return code;
            break;
        }
        case Instruction::kCondBr:
        {
            CondBrInst *cbInst = dynamic_cast<CondBrInst *>(instr);
            code += condBrInst_gen(cbInst);
            return code;
            break;
        }
        default:
        {
            code += "ERROR CODE : instruction " +
                    unkName + " is not implementation" + endl;
            break;
        }
        }
        if (!instr->getType()->isVoid())
        {
            code += storeRegToStack_gen(dstRegId, instr);
            // regm.freeReg(dstRegId);//TODO : code in here.
        }
        return code;
    }
    //
    string CodeGen::globaldata_gen(GlobalValue *glbvl)
    {
        string asmCode;
        string name = glbvl->getName();

        asmCode += space + ".global\t" + name + endl;
        auto type = static_cast<const PointerType *>(glbvl->getType())->getBaseType();
        // has init
        if (glbvl->init())
        {
            asmCode += space + ".data" + endl;
            //******************Revised by lyq BEGIN***************************************
            if (glbvl->isconst())
                asmCode += space + ".section" + space + "rodata" + endl;
            //******************Revised by lyq END*****************************************
            {
                asmCode += space + ".align\t2" + endl;
            }
            asmCode += space + ".type\t" + name + ", " + "%" + "object" + endl;
            asmCode += space + ".size\t" + name + ", 4" + endl;
            asmCode += name + ":\n";
            auto value = dyncast<ConstantValue>(glbvl->getOperand(0));
            std::stringstream ss;
            if (type->isInt())
                ss << value->getInt();
            else if (type->isFloat())
                ss << value->getFloat();
            asmCode += space + ".word\t" + ss.str() + endl;
        }
        // no init
        else
        {
            asmCode += space + ".bss" + endl;
            //******************Revised by lyq BEGIN***************************************
            if (glbvl->isconst())
                asmCode += space + ".section" + space + "rodata" + endl;
            //******************Revised by lyq END*****************************************
            {
                asmCode += space + ".align\t2" + endl;
            }
            asmCode += space + ".type\t" + name + ", %object" + endl;
            asmCode += space + ".size\t" + name + ", 4" + endl;
            asmCode += name + ":\n";
            asmCode += space + ".space\t4" + endl;
        }

        return asmCode;
    }

    string CodeGen::literalPoolsCode_gen(Function *func)
    {
        string code;
        /**
         *code in here
         */
        return code;
    }

} // namespace backend
