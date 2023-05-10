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
        code += space + ".globl " + func->getName() + endl;
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
        if (haveCall)
        {
            // push fp,lr
            code += space + "push\t{fp,lr}" + endl;
            // set fp
            code += space + "add\tfp, sp, #4" + endl;
        }
        else
        {
            // push fp
            code += space + "push\t{fp}" + endl;
            // set fp
            code += space + "add\tfp, sp, #0" + endl;
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
        code += space + "bx\t" + endl;
        return code;
    }

    string CodeGen::function_gen(Function *func)
    {
        curFunc = func;
        clearFunctionRecord(func);
        string bbCode;
        auto bbs = func->getBasicBlocks();
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
        //
        code = funcHead + prologueCode + bbCode +
               epilogueCode + literalPoolsCode;
        return code;
    }

    string CodeGen::basicBlock_gen(BasicBlock *bb)
    {
        curBB = bb;
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
        /**
         *code in here
         */
        return {dstRegId, code};
    }

    string CodeGen::storeInst_gen(StoreInst *stInst)
    {
        string code;
        /**
         *code in here
         */
        return code;
    }
    pair<RegId, string>
    CodeGen::loadInst_gen(LoadInst *ldInst, RegId dstRegId)
    {
        string code;
        /**
         *code in here
         */
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
        /**
         *code in here
         */
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
            code += M_emitComment("load inst");
            code += tmp.second;
            dstRegId = tmp.first;
            break;
        }
        case Instruction::kStore:
        {
            StoreInst *stInst = dynamic_cast<StoreInst *>(instr);
            code += M_emitComment("store inst");
            code += storeInst_gen(stInst);
            return code;
            break;
        }
        case Instruction::kAlloca:
        {
            AllocaInst *aInst = dynamic_cast<AllocaInst *>(instr);
            tmp = allocaInst_gen(aInst, RegManager::RANY);
            code += M_emitComment("alloca inst");
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
