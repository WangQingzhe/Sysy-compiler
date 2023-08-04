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
        code += space + ".fpu neon" + endl;
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
            if (libfunc.find(func->getName()) == libfunc.end())
                textCode += function_gen(func);
        }
        code += dataCode + textCode;
        code += space + ".section	.note.GNU-stack,\"\",%progbits" + endl;
        return code;
    }

    string CodeGen::functionHead_gen(Function *func)
    {
        string code;
        code += space + ".text" + endl;
        code += space + ".global\t" + func->getName() + endl;
        code += space + ".arm" + endl;
        code += space + ".align\t" + std::to_string(int_p2align) + endl;
        code += space + ".type\t" + func->getName() + ", %function" + endl;
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
        // 保护fp,lr
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
        // 开辟栈空间
        top_offset -= 4 * max_protect;
        int imm = -(top_offset + 4);
        if (imm % 8 != 0)
        {
            top_offset -= 4;
            imm += 4;
        }
        if(imm < 256)
            code += space + "sub\tsp, sp, #" + to_string(imm) + endl;
        else if (imm < 65536)
        {
            code += space + "sub\tsp, sp, #" + to_string(imm / 256 * 256) + endl;
            if (imm % 256 != 0)
                code += space + "sub\tsp, sp, #" + to_string(imm % 256) + endl;
        }
        else
        {
            code += space + "sub\tsp, sp, #" + to_string(imm / 4096 * 4096) + endl;
            if (imm % 4096 != 0)
            {
                imm = imm % 4096;
                code += space + "sub\tsp, sp, #" + to_string(imm / 256*256) + endl;
                if(imm % 256 != 0)
                code += space + "sub\tsp, sp, #" + to_string(imm % 256) + endl;
            }
        }
        // r0-r3入栈
        auto entry_block = func->getEntryBlock();
        auto args = entry_block->getArguments();
        // int arg_num = 0;
        int int_num = 0;
        int float_num = 0;
        for (auto arg = args.begin(); arg != args.end(); ++arg)
        {
            if (int_num >= 4 && float_num >= 5)
                break;
            int para_offset = paramsStOffset[arg->get()];
            int imm = -para_offset;
            // r0-r3
            if (arg->get()->getType()->as<PointerType>()->getBaseType()->isInt() || arg->get()->getNumDims() > 0)
            {
                if (int_num >= 4)
                    continue;
                if (imm < 256)
                    code += space + "str\tr" + to_string(int_num) + ", [fp, #" + to_string(para_offset) + "]" + endl;
                else if(imm < 65536)
                {
                    code += space + "sub\tr9, fp, #" + to_string(imm / 256 * 256) + endl;
                    if (imm % 256 != 0)
                        code += space + "sub\tr9, r9, #" + to_string(imm % 256) + endl;
                    code += space + "str\tr" + to_string(int_num) + ", [r9]" + endl;
                }
                else{
                    code += space + "sub\tr9, fp, #" + to_string(imm / 4096 * 4096) + endl;
                    if (imm % 4096 != 0){
                        imm = imm % 4096;
                        code += space + "sub\tr9, r9, #" + to_string(imm / 256 * 256) + endl;
                        if(imm % 256 != 0)
                        code += space + "sub\tr9, r9, #" + to_string(imm % 256) + endl;
                        }
                    code += space + "str\tr" + to_string(int_num) + ", [r9]" + endl;
                }
                int_num++;
            }
            // s0-s3
            else
            {
                if (float_num >= 5)
                    continue;
                if (imm < 256)
                    code += space + "vstr\ts" + to_string(float_num) + ", [fp, #" + to_string(para_offset) + "]" + endl;
                else
                {
                    code += space + "sub\tr9, fp, #" + to_string(imm / 256 * 256) + endl;
                    if (imm % 256 != 0)
                        code += space + "sub\tr9, r9, #" + to_string(imm % 256) + endl;
                    code += space + "vstr\ts" + to_string(float_num) + ", [r9]" + endl;
                }
                float_num++;
            }
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
        string func_exit = func->getName() + "_exit:";
        code += func_exit + endl;
        if (haveCall)
        {
            code += space + "sub\tsp, fp, #4" + endl;
            code += space + "pop\t{fp,lr}" + endl;
        }
        else
        {
            code += space + "add\tsp, fp, #0" + endl;
            code += space + "pop\t{fp}" + endl;
        }
        code += space + "bx\tlr" + endl;
        return code;
    }

    string CodeGen::function_gen(Function *func)
    {
        string code;
        clearFunctionRecord(func);
        // 第一遍扫描,设置每个变量的活跃区间
        int instr_num = 0;
        auto bbs = func->getBasicBlocks();
        for (auto iter = bbs.begin(); iter != bbs.end(); ++iter)
        {
            auto bb = iter->get();
            for (auto &instr : bb->getInstructions())
            {
                auto instrType = instr->getKind();
                if (instrType == Value::Kind::kCall)
                {
                    auto call_inst = dynamic_cast<CallInst *>(instr.get());
                    call_inst->setStart(instr_num);
                    auto args = call_inst->getArguments();
                    for (auto arg : args)
                    {
                        // if (isa<ConstantValue>(arg))
                        //     arg->setStart(instr_num);
                        arg->setEnd(instr_num);
                    }
                }
                else if (instrType == Value::Kind::kAlloca)
                {
                }
                else if (instrType == Value::Kind::kLoad)
                {
                    auto load_inst = dynamic_cast<LoadInst *>(instr.get());
                    load_inst->setStart(instr_num);
                    for (auto indice : load_inst->getIndices())
                    {
                        if (isa<ConstantValue>(indice))
                            continue;
                        indice->setEnd(instr_num);
                    }
                }
                else if (instrType == Value::Kind::kStore)
                {
                    auto store_inst = dynamic_cast<StoreInst *>(instr.get());
                    auto value = store_inst->getValue();
                    // if (isa<ConstantValue>(value))
                    // value->setStart(instr_num);
                    value->setEnd(instr_num);
                    for (auto indice : store_inst->getIndices())
                    {
                        if (isa<ConstantValue>(indice))
                            continue;
                        indice->setEnd(instr_num);
                    }
                }
                else if (instr->isBinary())
                {
                    auto binary_inst = dynamic_cast<BinaryInst *>(instr.get());
                    auto lhs = binary_inst->getLhs();
                    auto rhs = binary_inst->getRhs();
                    binary_inst->setStart(instr_num);
                    // if (!isa<ConstantValue>(lhs))
                    lhs->setEnd(instr_num);
                    // if (!isa<ConstantValue>(rhs))
                    rhs->setEnd(instr_num);
                }
                else if (instr->isUnary())
                {
                    auto unary_inst = dynamic_cast<UnaryInst *>(instr.get());
                    auto value = unary_inst->getOperand();
                    unary_inst->setStart(instr_num);
                    value->setEnd(instr_num);
                }
                else if(instrType == Value::Kind::kReturn)
                {
                    auto retInst = dynamic_cast<ReturnInst*>(instr.get());
                    auto retValue = retInst->getReturnValue();
                    retValue->setEnd(instr_num);
                }
                instr_num++;
            }
            // for (auto &instr : bb->getInstructions())
            //     code += "%"+instr.get()->getName()+":"+to_string(instr.get()->GetStart())+"-"+to_string(instr.get()->GetEnd())+endl;

        }
        // 第二遍扫描,使用线性扫描算法对r寄存器进行分配
        for (auto iter = bbs.begin(); iter != bbs.end(); ++iter)
        {
            auto bb = iter->get();
            for (auto &instr : bb->getInstructions())
            {
                if (instr->getType()->isFloat())
                {
                    if (!isa<LoadInst>(instr.get()))
                        continue;
                    auto ldinst = dynamic_cast<LoadInst *>(instr.get());
                    auto pointer = ldinst->getPointer();
                    int numdims = 0;
                    if (isa<GlobalValue>(pointer))
                        numdims = dynamic_cast<GlobalValue *>(pointer)->getNumDims();
                    else if (isa<Argument>(pointer))
                        numdims = dynamic_cast<Argument *>(pointer)->getNumDims();
                    else if (isa<AllocaInst>(pointer))
                        numdims = dynamic_cast<AllocaInst *>(pointer)->getNumDims();
                    if (ldinst->getNumIndices() == numdims)
                        continue;
                    else
                        ; // 若load出一个float数组,仍然按照int处理
                }
                auto instrType = instr->getKind();
                // 只考虑能产生新变量的指令,这样就相当于从小到大遍历所有变量
                if ((instrType == Value::Kind::kCall && !instr->getType()->isVoid()) || instrType == Value::Kind::kLoad || instr.get()->isBinary() || instr.get()->isUnary())
                {
                    // code += "%" + instr->getName() + "start:" + to_string(instr->GetStart()) + endl;
                    // for (auto interval : active)
                    //     code += "%" + interval->getName() + "end:" + to_string(interval->GetEnd()) + endl;
                    ExpireOldIntervals(instr.get());
                    if (active.size() == RegManager::R)
                        SpillAtIntervals(instr.get());
                    else
                    {
                        Register[instr.get()] = regm.GetFreeReg();
                        active.insert(instr.get());
                    }
                    // code += "%" + instr->getName() + regm.toString(Register[instr.get()]) + endl;
                }
            }
        }
        // 第三遍扫描,使用线性扫描算法对s寄存器进行分配
        active.clear();
        for (auto iter = bbs.begin(); iter != bbs.end(); ++iter)
        {
            auto bb = iter->get();
            for (auto &instr : bb->getInstructions())
            {
                if (instr->getType()->isInt())
                    continue;
                // 对于load一个float数组变量,已经为其分配了一个r寄存器,不能再分配s寄存器
                else if (Register.find(instr.get()) != Register.end())
                    continue;
                auto instrType = instr->getKind();
                // 只考虑能产生新变量的指令,这样就相当于从小到大遍历所有变量
                if ((instrType == Value::Kind::kCall && !instr->getType()->isVoid()) || instrType == Value::Kind::kLoad || instr.get()->isBinary() || instr.get()->isUnary())
                {
                    ExpireOldIntervals(instr.get());
                    if (active.size() == RegManager::S)
                        SpillAtIntervals(instr.get());
                    else
                    {
                        Register[instr.get()] = regm.GetFreesReg();
                        active.insert(instr.get());
                    }
                    // code += "%" + instr->getName() + regm.toString(Register[instr.get()]) + endl;
                }
            }
        }
        // 第四遍扫描,开辟局部变量的栈空间
        for (auto iter = bbs.begin(); iter != bbs.end(); ++iter)
        {
            auto bb = iter->get();
            for (auto &instr : bb->getInstructions())
            {
                auto instrType = instr->getKind();
                // 判断是否有函数调用,以及子函数参数个数最大值,子函数需要保护的参数个数最大值
                if (instrType == Value::Kind::kCall)
                {
                    haveCall = true;
                    auto alloca_inst = dynamic_cast<CallInst *>(instr.get());
                    int args_size = alloca_inst->getArguments().size();
                    // int protect_cnt = alloca_inst->ProtectCnt();
                    max_param = args_size > max_param ? args_size : max_param;
                    // max_protect = protect_cnt > max_protect ? protect_cnt : max_protect;
                }
                // 为局部变量开辟栈空间
                else if (instrType == Value::Kind::kAlloca)
                {
                    auto alloca_inst = dynamic_cast<AllocaInst *>(instr.get());
                    int NumDims = alloca_inst->getNumDims();
                    auto Dims = alloca_inst->getDims();
                    int num = 1;
                    for (auto iter = Dims.begin(); iter != Dims.end(); iter++)
                        num *= dynamic_cast<ConstantValue *>(*iter)->getInt();
                    if (NumDims == 0)
                        localVarStOffset.emplace(alloca_inst, top_offset);
                    else if (NumDims > 0)
                        localVarStOffset.emplace(alloca_inst, top_offset - 4 * num + 4);
                    top_offset -= 4 * num;
                }
            }
        }
        // 开辟形式参数&中间变量&寄存器保护&实际参数的栈空间
        {
            // 形式参数
            auto args = func->getEntryBlock()->getArguments();
            // int arg_num = 0;
            int int_num = 0;
            int float_num = 0;
            for (auto arg = args.begin(); arg != args.end(); ++arg)
            {
                // r0-r3
                if (arg->get()->getType()->as<PointerType>()->getBaseType()->isInt() || arg->get()->getNumDims() > 0)
                {
                    if (int_num < 4)
                    {
                        paramsStOffset.emplace(arg->get(), top_offset);
                        top_offset -= 4;
                    }
                    // other
                    else
                    {
                        paramsStOffset.emplace(arg->get(), above_offset);
                        above_offset += 4;
                    }
                    int_num++;
                }
                // s0-s3
                else
                {
                    if (float_num < 5)
                    {
                        paramsStOffset.emplace(arg->get(), top_offset);
                        top_offset -= 4;
                    }
                    // other
                    else
                    {
                        paramsStOffset.emplace(arg->get(), above_offset);
                        above_offset += 4;
                    }
                    float_num++;
                }
            }
            // 中间变量
            // temp_offset = top_offset;
            // top_offset -= 4;
            // 寄存器保护
            protect_reg_offset = top_offset;
            // top_offset -= 4 * max_protect;
            // 传递的实参
            if (max_param > 4)
                top_offset -= (max_param - 4) * 4;
        }
        // 第五遍扫描,生成汇编代码
        string bbCode;
        for (auto iter = bbs.begin(); iter != bbs.end(); ++iter)
        {
            auto bb = iter->get();
            bbCode += basicBlock_gen(bb);
        }
        string funcHead = functionHead_gen(func);
        string prologueCode = prologueCode_gen(func);
        string epilogueCode = epilogueCode_gen(func);
        string literalPoolsCode;
        code += funcHead + prologueCode + bbCode +
                epilogueCode + literalPoolsCode;

        return code;
    }

    string CodeGen::basicBlock_gen(BasicBlock *bb)
    {
        curBB = bb;
        string bbLabel = bb->getName();
        imms_name = "IMM_" + bbLabel;
        imms.clear();
        imm_offset = 0;
        // string bbLabel = getBBLabel(bb);
        string code;
        code += bbLabel + ":" + endl;
        for (auto &instr : bb->getInstructions())
        {
            auto instrType = instr->getKind();
            code += instruction_gen(instr.get());
        }
        code += literalPoolsCode_gen(curFunc);
        return code;
    }
    /**
     * RegId : binaryInst_gen returns RegId as its destination operand
     * code  : asmcode generated by binaryInst_gen
     */
    pair<RegId, string> CodeGen::binaryInst_gen(BinaryInst *bInst, RegId dstRegId)
    {
        string code;
        string lname, rname;
        dstRegId = Register[bInst];
        RegId lRegId, rRegId;
        int instrname = stoi(bInst->getName());
        auto lhs = bInst->getLhs();
        auto rhs = bInst->getRhs();
        int r_val = 0, l_val = 0;
        bool lconst = false, rconst = false;
        if (isa<ConstantValue>(lhs))
        {
            lname = "#" + to_string(dynamic_cast<ConstantValue *>(lhs)->getInt());
            l_val = dynamic_cast<ConstantValue *>(lhs)->getInt();
            lconst = true;
        }
        else
        {
            lname = "r" + lhs->getName();
            lRegId = Register[dynamic_cast<Instruction *>(lhs)];
        }
        if (isa<ConstantValue>(rhs))
        {
            rname = "#" + to_string(dynamic_cast<ConstantValue *>(rhs)->getInt());
            rconst = true;
            r_val = dynamic_cast<ConstantValue *>(rhs)->getInt();
        }
        else
        {
            rname = "r" + rhs->getName();
            rRegId = Register[dynamic_cast<Instruction *>(rhs)];
        }
        auto res = bInst->getName();
        if (bInst->getKind() == Instruction::kAdd)
        {
            if (lconst)
            {
                if (l_val <= 0xffff && l_val >= 0)
                {
                    // add lconst r9 is used when mov immidiate number
                    // code += space + "add\tr" + res + ", r" + res + ", " + rname + endl;
                    code += space + "mov\tr9, " + lname + endl;
                    if (rRegId != RegManager::NONE && dstRegId != RegManager::NONE)
                        code += emitInst_2srcR_1dstR("add", regm.toString(dstRegId), string("r9"), regm.toString(rRegId));
                    else if (dstRegId != RegManager::NONE)
                    {
                        code += emitInst_mem("ldr", "r10", "fp", rhs->GetLocation());
                        code += emitInst_2srcR_1dstR("add", regm.toString(dstRegId), "r9", "r10");
                    }
                    else
                    {
                        code += emitInst_mem("ldr", "r10", "fp", rhs->GetLocation());
                        code += emitInst_2srcR_1dstR("add", "r9", "r9", "r10");
                        code += emitInst_mem("str", "r9", "fp", bInst->GetLocation());
                        AVALUE[bInst].stack_offset = bInst->GetLocation();
                    }
                }
                else
                {
                    // add lconst r9 is used when mov immidiate number
                    code += space + "movw\tr9" + ", #" + to_string(unsigned(l_val & 0xffff)) + endl;
                    code += space + "movt\tr9" + ", #" + to_string(unsigned((l_val >> 16) & 0xffff)) + endl;
                    code += emitInst_2srcR_1dstR("add", regm.toString(dstRegId), string("r9"), regm.toString(rRegId));
                    // code += space + "add\tr" + res + ", r" + res + ", " + rname + endl;
                }
            }
            else if (rconst)
            {
                if (r_val <= 0xffff && r_val >= 0)
                {
                    code += space + "mov\tr9, " + rname + endl;
                    code += emitInst_2srcR_1dstR("add", regm.toString(dstRegId), regm.toString(lRegId), "r9");
                    // code += space + "add\tr" + res + ", r" + res + ", " + lname + endl;
                }
                else
                {
                    code += space + "movw\tr9" + ", #" + to_string(unsigned(r_val & 0xffff)) + endl;
                    code += space + "movt\tr9" + ", #" + to_string(unsigned((r_val >> 16) & 0xffff)) + endl;
                    code += emitInst_2srcR_1dstR("add", regm.toString(dstRegId), regm.toString(lRegId), "r9");
                }
            }
            else
            // "[fp - " + to_string(bInst->GetLocation()) + "]"
            // code += space + "add\tr" + res + ", " + lname + ", " + rname + endl;
            {
                if (rRegId != RegManager::NONE && dstRegId != RegManager::NONE && lRegId != RegManager::NONE)
                    code += emitInst_2srcR_1dstR("add", regm.toString(dstRegId), regm.toString(lRegId), regm.toString(rRegId));
                else if (dstRegId != RegManager::NONE && rRegId != RegManager::NONE)
                {
                    code += emitInst_mem("ldr", "r9", "fp", lhs->GetLocation());
                    code += emitInst_2srcR_1dstR("add", regm.toString(dstRegId), "r9", regm.toString(rRegId));
                }
            }
        }
        else if (bInst->getKind() == Instruction::kSub)
        {
            if (lconst && rconst)
            {
                int val = dynamic_cast<ConstantValue *>(lhs)->getInt() - dynamic_cast<ConstantValue *>(rhs)->getInt();
                if (val >= 0)
                    code += space + "mov\tr" + res + ", #" + to_string(val) + endl;
                else
                {
                    code += space + "mov\tr" + res + ", #" + to_string(-val) + endl;
                    code += space + "mvn\tr" + res + ", #1" + endl;
                }
            }
            else if (lconst)
            {
                if (l_val <= 0xffff & l_val >= 0)
                {
                    code += space + "mov\tr9, " + lname + endl;
                    code += emitInst_2srcR_1dstR("sub", regm.toString(dstRegId), string("r9"), regm.toString(rRegId));
                }
                else
                {
                    code += space + "movw\tr9" + ", #" + to_string(unsigned(l_val & 0xffff)) + endl;
                    code += space + "movt\tr9" + ", #" + to_string(unsigned((l_val >> 16) & 0xffff)) + endl;
                    code += emitInst_2srcR_1dstR("sub", regm.toString(dstRegId), string("r9"), regm.toString(rRegId));
                }
            }
            else if (rconst)
            {
                if (r_val <= 0xffff && r_val >= 0)
                {
                    code += space + "mov\tr9, " + rname + endl;
                    code += emitInst_2srcR_1dstR("sub", regm.toString(dstRegId), regm.toString(lRegId), "r9");
                }
                else
                {
                    code += space + "movw\tr9" + ", #" + to_string(unsigned(r_val & 0xffff)) + endl;
                    code += space + "movt\tr9" + ", #" + to_string(unsigned((r_val >> 16) & 0xffff)) + endl;
                    code += emitInst_2srcR_1dstR("sub", regm.toString(dstRegId), regm.toString(lRegId), "r9");
                }
            }
            else
                code += emitInst_2srcR_1dstR("sub", regm.toString(dstRegId), regm.toString(lRegId), regm.toString(rRegId));
        }
        else if (bInst->getKind() == Instruction::kMul)
        {
            if (dstRegId == RegManager::NONE)
            {
                dstRegId = RegManager::R10;
            }
            if (lconst)
            {
                if (l_val <= 0xffff & l_val >= 0)
                {
                    code += space + "mov\tr9, " + lname + endl;
                    code += emitInst_2srcR_1dstR("mul", regm.toString(dstRegId), string("r9"), regm.toString(rRegId));
                }
                else
                {
                    code += space + "movw\tr9" + ", #" + to_string(unsigned(l_val & 0xffff)) + endl;
                    code += space + "movt\tr9" + ", #" + to_string(unsigned((l_val >> 16) & 0xffff)) + endl;
                    code += emitInst_2srcR_1dstR("mul", regm.toString(dstRegId), string("r9"), regm.toString(rRegId));
                }
            }
            else if (rconst)
            {
                if (r_val <= 0xffff && r_val >= 0)
                {
                    code += space + "mov\tr9, " + rname + endl;
                    code += emitInst_2srcR_1dstR("mul", regm.toString(dstRegId), regm.toString(lRegId), "r9");
                }
                else
                {
                    code += space + "movw\tr9" + ", #" + to_string(unsigned(r_val & 0xffff)) + endl;
                    code += space + "movt\tr9" + ", #" + to_string(unsigned((r_val >> 16) & 0xffff)) + endl;
                    code += emitInst_2srcR_1dstR("mul", regm.toString(dstRegId), regm.toString(lRegId), "r9");
                }
            }
            else
                code += emitInst_2srcR_1dstR("mul", regm.toString(dstRegId), regm.toString(lRegId), regm.toString(rRegId));
            if (dstRegId == RegManager::R10)
            {
                dstRegId = RegManager::NONE;
                code += emitInst_mem("str", "r10", "fp", bInst->GetLocation());
                AVALUE[bInst].stack_offset = bInst->GetLocation();
            }
        }
        else if (bInst->getKind() == Instruction::kDiv)
        {
            /*
                因为除法硬件开销很大，所以我考虑对除法的特定情况进行优化
                1. 如果被除数已知但是除数非已知，这种情况目前没办法优化
                2. 如果除数已知但是被除数非已知 eg： a//CONSTANT 那么分两种情况
                2.1 如果被除数是二的幂次，那么直接移位即可：
                    ###
                        CONSTANT = 2 ** b
                        a//CONSTANT = a >> b
                    ###
                2.2 如果被除数不是二的幂次，那么可以这样优化：
                    首先，把除法转换成乘法；
                    ###
                        a//CONSTANT <=> a * (1/CONSTANT)
                    ###
                    然后，考虑用整数的乘法指令SMULL来做，那么可以将（1/CONSTANT) 左移 32 位，
                    考虑截断误差，加1即可，经检验在int的范围内，不存在的额外进位。
                    再带入上面的等式就得到；
                    ###
                        r = (1 // CONSTANT) << 32
                        a//CONSTANT <=> [a *(r+1)] >> 32;
                    ###
            */

            // r9 and r10 is used
            if (lconst)
            {
                if (l_val <= 0xffff & l_val >= 0)
                {
                    code += space + "mov\tr9, " + lname + endl;
                    code += emitInst_2srcR_1dstR("sdiv", regm.toString(dstRegId), string("r9"), regm.toString(rRegId));
                }
                else
                {
                    code += space + "movw\tr9" + ", #" + to_string(unsigned(l_val & 0xffff)) + endl;
                    code += space + "movt\tr9" + ", #" + to_string(unsigned((l_val >> 16) & 0xffff)) + endl;
                    code += emitInst_2srcR_1dstR("sdiv", regm.toString(dstRegId), string("r9"), regm.toString(rRegId));
                }
            }
            else if (rconst)
            {
                int rvalue = dynamic_cast<ConstantValue *>(rhs)->getInt();
                int num1 = 0;
                unsigned rval = rvalue > 0 ? rvalue : -rvalue;
                bool negflag = rvalue > 0 ? 0 : 1;
                unsigned C = rvalue > 0 ? rvalue : -rvalue;
                int shiftnum = 31;
                while (rval > 0)
                {
                    shiftnum++;
                    if ((rval & 1) == 1)
                        num1 += 1;
                    rval = rval >> 1;
                }
                if (num1 == 1)
                {
                    int b = 0;
                    rval = rvalue > 0 ? rvalue : -rvalue;
                    rval = rval >> 1;
                    while (rval)
                    {
                        b++;
                        rval = rval >> 1;
                    }
                    rval = rvalue > 0 ? rvalue : -rvalue;
                    rval -= 1;
                    if (rvalue < 0xffff && rvalue > 0)
                        code += emitInst_nosrcR_1DstR("movw", "r9", rval);
                    else
                    {
                        code += emitInst_nosrcR_1DstR("movw", "r9", rval & 0xffff);
                        code += emitInst_nosrcR_1DstR("movt", "r9", (rval >> 16) & 0xffff);
                    }
                    code += emitInst_2srcR_1dstR("add", "r9", regm.toString(lRegId), "r9");
                    code += emitInst_nosrcR_1DstR("cmp", regm.toString(lRegId), 0);
                    code += emitInst_2srcR("movlt", regm.toString(lRegId), "r9");
                    code += emitInst_2srcR_1dstR("asr", regm.toString(dstRegId), regm.toString(lRegId), "#" + to_string(b));
                    // code += space + "asr\tr" + res + ", " + lname + ", #" + to_string(b) + endl;
                    if (negflag)
                    {
                        code += emitInst_1srcR_1DstR("mvn", regm.toString(dstRegId), regm.toString(dstRegId));
                        code += emitInst_2srcR_1dstR("add", regm.toString(dstRegId), regm.toString(dstRegId), "#1");
                        // code += space + "mvn\tr" + res + ", r" + res + endl;
                        // code += space + "add\tr" + res + ", r" + res + ", #1" + endl;
                    }
                }
                else
                {
                    shiftnum -= 1;
                    long Power32 = long(1) << shiftnum;
                    int r = Power32 / C + 1;
                    code += space + "movw\tr9" + ", #" + to_string(unsigned(r & 0xffff)) + endl;
                    code += space + "movt\tr9" + ", #" + to_string(unsigned((r >> 16) & 0xFFFF)) + endl;
                    code += space + "smull\tr10" + ", r9" + ", r9" + ", " + regm.toString(lRegId) + endl;
                    if (shiftnum - 32)
                        code += space + "asr\tr9" + ", r9" + ", #" + to_string(shiftnum - 32) + endl;
                    code += space + "asr\tr10" + ", " + regm.toString(lRegId) + ", #31" + endl;
                    code += space + "sub\t" + regm.toString(dstRegId) + ", r9" + ", r10" + endl;
                    if (negflag)
                    {
                        code += emitInst_1srcR_1DstR("mvn", regm.toString(dstRegId), regm.toString(dstRegId));
                        code += emitInst_2srcR_1dstR("add", regm.toString(dstRegId), regm.toString(dstRegId), "#1");
                        // code += space + "mvn\tr" + to_string(instrname) + ", r" + to_string(instrname) + endl;
                        // code += space + "add\tr" + to_string(instrname) + ", r" + to_string(instrname) + ", #1" + endl;
                    }
                }
            }
            else
                code += emitInst_2srcR_1dstR("sdiv", regm.toString(dstRegId), regm.toString(lRegId), regm.toString(rRegId));
            // code += space + "sdiv\tr" + res + ", " + lname + ", " + rname + endl;
        }
        else if (bInst->getKind() == Instruction::kRem)
        // r9 and r10 is used
        {
            if (lconst)
            {
                if (l_val <= 0xffff && l_val >= 0)
                    code += emitInst_1srcR_1DstR("mov", "r9", lname);
                else
                {
                    code += emitInst_1srcR_1DstR("movw", "r9", to_string(l_val & 0xffff));
                    code += emitInst_1srcR_1DstR("movt", "r9", to_string((l_val >> 16) & 0xffff));
                }
                code += emitInst_2srcR_1dstR("sdiv", "r10", "r9", regm.toString(rRegId));
                code += emitInst_2srcR_1dstR("mul", regm.toString(dstRegId), "r10", regm.toString(rRegId));
                code += emitInst_2srcR_1dstR("sub", regm.toString(dstRegId), "r9", regm.toString(dstRegId));
                // code += space + "mov\tr" + to_string(instrname + 1) + ", " + lname + endl;
                // code += space + "sdiv\tr" + res + ", r" + to_string(instrname + 1) + ", " + rname + endl;
                // code += space + "mul\tr" + res + ", r" + to_string(instrname) + ", " + rname + endl;
                // code += space + "sub\tr" + res + ", r" + to_string(instrname + 1) + ", r" + res + endl;
            }
            else if (rconst)
            {
                int rvalue = dynamic_cast<ConstantValue *>(rhs)->getInt();
                rvalue = rvalue > 0 ? rvalue : -rvalue;
                int num1 = 0;
                int rval = rvalue;
                int shiftnum = 31;
                while (rval > 0)
                {
                    if ((rval & 1) == 1)
                        num1 += 1;
                    rval = rval >> 1;
                    shiftnum++;
                }
                if (num1 == 1)
                {
                    code += emitInst_2srcR_1dstR("rsbs", "r9", regm.toString(lRegId), "#0");
                    if (rvalue <= 128)
                    {
                        // code += space + "and\t" + regm.toString(dstRegId) + ", " + regm.toString(lRegId) + ", #" + to_string(rvalue - 1) + endl;
                        code += emitInst_1srcR_1DstR("and", "r9", "r9", rvalue - 1);
                        code += emitInst_1srcR_1DstR("and", regm.toString(lRegId), regm.toString(lRegId), rvalue - 1);
                        code += emitInst_2srcR_1dstR("rsbpl", regm.toString(dstRegId), "r9", "#0");
                    }
                    else if (rvalue == 256)
                    {
                        code += emitInst_1srcR_1DstR("uxtb", regm.toString(lRegId), regm.toString(lRegId));
                        code += emitInst_1srcR_1DstR("uxtb", "r9", "r9");
                        code += emitInst_2srcR_1dstR("rsbpl", regm.toString(dstRegId), "r9", "#0");
                    }
                    else
                    {
                        code += emitInst_2srcR_1dstR("ubfx", regm.toString(lRegId), regm.toString(lRegId), "#0", shiftnum - 32);
                        code += emitInst_2srcR_1dstR("ubfx", "r9", "r9", "#0", shiftnum - 32);
                        code += emitInst_2srcR_1dstR("rsbpl", regm.toString(dstRegId), "r9", "#0");
                    }
                }
                else
                {
                    int C = rvalue;
                    shiftnum -= 1;
                    long Power32 = long(1) << shiftnum;
                    int r = Power32 / C + 1;
                    if (r > 0 && r <= 0xffff)
                        code += emitInst_1srcR_1DstR("mov", "r9", "#" + to_string(r));
                    else
                    {
                        code += space + "movw\t" + "r9" + ", #" + to_string(int(r & 0xffff)) + endl;
                        code += space + "movt\t" + "r9" + ", #" + to_string(int((r >> 16) & 0xFFFF)) + endl;
                    }
                    code += space + "smull\tr9" + ", r10" + ", r9" + ", " + regm.toString(lRegId) + endl;
                    if (shiftnum - 32)
                        code += space + "asr\tr10" + ", r10" + ", #" + to_string(shiftnum - 32) + endl;
                    code += space + "asr\tr9" + ", " + regm.toString(lRegId) + ", #31" + endl;
                    code += space + "sub\tr9" + ", r10" + ", r9" + endl;
                    if (rvalue >= 0 && rvalue <= 0xffff)
                        code += emitInst_1srcR_1DstR("mov", "r10", "#" + to_string(rvalue));
                    else
                    {
                        code += space + "movw\t" + "r10" + ", #" + to_string(rvalue & 0xffff) + endl;
                        code += space + "movt\t" + "r10" + ", #" + to_string(((rvalue >> 16) & 0xffff)) + endl;
                    }
                    code += space + "mul\t" + "r10" + ", r9" + ", r10" + endl;
                    code += space + "sub\t" + regm.toString(dstRegId) + ", " + regm.toString(lRegId) + ", r10" + endl;
                }
            }
            else
            {
                code += emitInst_2srcR_1dstR("sdiv", "r9", regm.toString(lRegId), regm.toString(rRegId));
                code += emitInst_2srcR_1dstR("mul", regm.toString(rRegId), "r9", regm.toString(rRegId));
                code += emitInst_2srcR_1dstR("sub", regm.toString(dstRegId), regm.toString(lRegId), regm.toString(rRegId));
                // code += space + "sdiv\tr" + res + ", " + lname + ", " + rname + endl;
                // code += space + "mul\t" + rname + ", r" + res + ", " + rname + endl;
                // code += space + "sub\tr" + res + ", " + lname + ", " + rname + endl;
            }
        }
        else if (lconst && rconst)
        {
            if (bInst->getKind() == Instruction::kICmpEQ)
            {
                code += space + "mov\t" + regm.toString(dstRegId) + ", #" + to_string((l_val == r_val) ? 1 : 0) + endl;
            }
            else if (bInst->getKind() == Instruction::kICmpGE)
            {
                code += space + "mov\t" + regm.toString(dstRegId) + ", #" + to_string((l_val >= r_val) ? 1 : 0) + endl;
            }
            else if (bInst->getKind() == Instruction::kICmpGT)
            {
                code += space + "mov\t" + regm.toString(dstRegId) + ", #" + to_string((l_val > r_val) ? 1 : 0) + endl;
            }
            else if (bInst->getKind() == Instruction::kICmpLE)
            {
                code += space + "mov\t" + regm.toString(dstRegId) + ", #" + to_string((l_val <= r_val) ? 1 : 0) + endl;
            }
            else if (bInst->getKind() == Instruction::kICmpLT)
            {
                code += space + "mov\t" + regm.toString(dstRegId) + ", #" + to_string((l_val < r_val) ? 1 : 0) + endl;
            }
            else if (bInst->getKind() == Instruction::kICmpNE)
            {
                code += space + "mov\t" + regm.toString(dstRegId) + ", #" + to_string((l_val != r_val) ? 1 : 0) + endl;
            }
        }
        else if (bInst->getKind() == Instruction::kICmpEQ)
        {
            if (lconst)
            { // r9 is used to store left immidiate number
                if (l_val >= 0 && l_val < 0xffff)
                {
                    code += emitInst_1srcR_1DstR("mov", "r9", "#" + to_string(l_val));
                }
                else
                {
                    code += emitInst_1srcR_1DstR("movw", "r9", "#" + to_string(l_val & 0xffff));
                    code += emitInst_1srcR_1DstR("movt", "r9", "#" + to_string((l_val >> 16) & 0xffff));
                }
                // code += space + "mov\tr" + to_string(instrname) + ", #" + to_string(l_val) + endl;
                code += space + "cmp\tr9, " + regm.toString(rRegId) + endl;
            }
            else if (rconst)
            {
                if (r_val > 0 && r_val < 0xff)
                {
                    code += space + "cmp\t" + regm.toString(lRegId) + ", " + rname + endl;
                }
                else
                {
                    if (r_val > 0 && r_val < 0xffff)
                        code += emitInst_1srcR_1DstR("mov", "r9", "#" + to_string(r_val));
                    else
                    {
                        code += emitInst_1srcR_1DstR("movw", "r9", "#" + to_string(r_val & 0xffff));
                        code += emitInst_1srcR_1DstR("movt", "r9", "#" + to_string((r_val >> 16) & 0xffff));
                    }
                    code += space + "cmp\t" + regm.toString(lRegId) + ", r9" + endl;
                }
            }
            else
            {
                code += space + "cmp\t" + regm.toString(lRegId) + ", " + regm.toString(rRegId) + endl;
            }
        }
        else if (bInst->getKind() == Instruction::kICmpGE)
        {
            if (lconst)
            { // r9 is used to store left immidiate number
                if (l_val >= 0 && l_val < 0xffff)
                {
                    code += emitInst_1srcR_1DstR("mov", "r9", "#" + to_string(l_val));
                }
                else
                {
                    code += emitInst_1srcR_1DstR("movw", "r9", "#" + to_string(l_val & 0xffff));
                    code += emitInst_1srcR_1DstR("movt", "r9", "#" + to_string((l_val >> 16) & 0xffff));
                }
                // code += space + "mov\tr" + to_string(instrname) + ", #" + to_string(l_val) + endl;
                code += space + "cmp\tr9, " + regm.toString(rRegId) + endl;
            }
            else if (rconst)
            {
                if (r_val > 0 && r_val < 0xff)
                {
                    code += space + "cmp\t" + regm.toString(lRegId) + ", " + rname + endl;
                }
                else
                {
                    if (r_val > 0 && r_val < 0xffff)
                        code += emitInst_1srcR_1DstR("mov", "r9", "#" + to_string(r_val));
                    else
                    {
                        code += emitInst_1srcR_1DstR("movw", "r9", "#" + to_string(r_val & 0xffff));
                        code += emitInst_1srcR_1DstR("movt", "r9", "#" + to_string((r_val >> 16) & 0xffff));
                    }
                    code += space + "cmp\t" + regm.toString(lRegId) + ", r9" + endl;
                }
            }
            else
                code += space + "cmp\t" + regm.toString(lRegId) + ", " + regm.toString(rRegId) + endl;
        }
        else if (bInst->getKind() == Instruction::kICmpGT)
        {
            if (lconst)
            { // r9 is used to store left immidiate number
                if (l_val >= 0 && l_val < 0xffff)
                {
                    code += emitInst_1srcR_1DstR("mov", "r9", "#" + to_string(l_val));
                }
                else
                {
                    code += emitInst_1srcR_1DstR("movw", "r9", "#" + to_string(l_val & 0xffff));
                    code += emitInst_1srcR_1DstR("movt", "r9", "#" + to_string((l_val >> 16) & 0xffff));
                }
                // code += space + "mov\tr" + to_string(instrname) + ", #" + to_string(l_val) + endl;
                code += space + "cmp\tr9, " + regm.toString(rRegId) + endl;
            }
            else if (rconst)
            {
                if (r_val > 0 && r_val < 0xff)
                {
                    code += space + "cmp\t" + regm.toString(lRegId) + ", " + rname + endl;
                }
                else
                {
                    if (r_val > 0 && r_val < 0xffff)
                        code += emitInst_1srcR_1DstR("mov", "r9", "#" + to_string(r_val));
                    else
                    {
                        code += emitInst_1srcR_1DstR("movw", "r9", "#" + to_string(r_val & 0xffff));
                        code += emitInst_1srcR_1DstR("movt", "r9", "#" + to_string((r_val >> 16) & 0xffff));
                    }
                    code += space + "cmp\t" + regm.toString(lRegId) + ", r9" + endl;
                }
            }
            else
                code += space + "cmp\t" + regm.toString(lRegId) + ", " + regm.toString(rRegId) + endl;
        }
        else if (bInst->getKind() == Instruction::kICmpLE)
        {
            if (lconst)
            { // r9 is used to store left immidiate number
                if (l_val >= 0 && l_val < 0xffff)
                {
                    code += emitInst_1srcR_1DstR("mov", "r9", "#" + to_string(l_val));
                }
                else
                {
                    code += emitInst_1srcR_1DstR("movw", "r9", "#" + to_string(l_val & 0xffff));
                    code += emitInst_1srcR_1DstR("movt", "r9", "#" + to_string((l_val >> 16) & 0xffff));
                }
                // code += space + "mov\tr" + to_string(instrname) + ", #" + to_string(l_val) + endl;
                code += space + "cmp\tr9, " + regm.toString(rRegId) + endl;
            }
            else if (rconst)
            {
                if (r_val > 0 && r_val < 0xff)
                {
                    code += space + "cmp\t" + regm.toString(lRegId) + ", " + rname + endl;
                }
                else
                {
                    if (r_val > 0 && r_val < 0xffff)
                        code += emitInst_1srcR_1DstR("mov", "r9", "#" + to_string(r_val));
                    else
                    {
                        code += emitInst_1srcR_1DstR("movw", "r9", "#" + to_string(r_val & 0xffff));
                        code += emitInst_1srcR_1DstR("movt", "r9", "#" + to_string((r_val >> 16) & 0xffff));
                    }
                    code += space + "cmp\t" + regm.toString(lRegId) + ", r9" + endl;
                }
            }
            else
                code += space + "cmp\t" + regm.toString(lRegId) + ", " + regm.toString(rRegId) + endl;
        }
        else if (bInst->getKind() == Instruction::kICmpLT)
        {
            if (lconst)
            { // r9 is used to store left immidiate number
                if (l_val >= 0 && l_val < 0xffff)
                {
                    code += emitInst_1srcR_1DstR("mov", "r9", "#" + to_string(l_val));
                }
                else
                {
                    code += emitInst_1srcR_1DstR("movw", "r9", "#" + to_string(l_val & 0xffff));
                    code += emitInst_1srcR_1DstR("movt", "r9", "#" + to_string((l_val >> 16) & 0xffff));
                }
                // code += space + "mov\tr" + to_string(instrname) + ", #" + to_string(l_val) + endl;
                code += space + "cmp\tr9, " + regm.toString(rRegId) + endl;
            }
            else if (rconst)
            {
                if (r_val > 0 && r_val < 0xff)
                {
                    code += space + "cmp\t" + regm.toString(lRegId) + ", " + rname + endl;
                }
                else
                {
                    if (r_val > 0 && r_val < 0xffff)
                        code += emitInst_1srcR_1DstR("mov", "r9", "#" + to_string(r_val));
                    else
                    {
                        code += emitInst_1srcR_1DstR("movw", "r9", "#" + to_string(r_val & 0xffff));
                        code += emitInst_1srcR_1DstR("movt", "r9", "#" + to_string((r_val >> 16) & 0xffff));
                    }
                    code += space + "cmp\t" + regm.toString(lRegId) + ", r9" + endl;
                }
            }
            else
                code += space + "cmp\t" + regm.toString(lRegId) + ", " + regm.toString(rRegId) + endl;
        }
        else if (bInst->getKind() == Instruction::kICmpNE)
        {
            if (lconst)
            { // r9 is used to store left immidiate number
                if (l_val >= 0 && l_val < 0xffff)
                {
                    code += emitInst_1srcR_1DstR("mov", "r9", "#" + to_string(l_val));
                }
                else
                {
                    code += emitInst_1srcR_1DstR("movw", "r9", "#" + to_string(l_val & 0xffff));
                    code += emitInst_1srcR_1DstR("movt", "r9", "#" + to_string((l_val >> 16) & 0xffff));
                }
                // code += space + "mov\tr" + to_string(instrname) + ", #" + to_string(l_val) + endl;
                code += space + "cmp\tr9, " + regm.toString(rRegId) + endl;
            }
            else if (rconst)
            {
                if (r_val > 0 && r_val < 0xff)
                {
                    code += space + "cmp\t" + regm.toString(lRegId) + ", " + rname + endl;
                }
                else
                {
                    if (r_val > 0 && r_val < 0xffff)
                        code += emitInst_1srcR_1DstR("mov", "r9", "#" + to_string(r_val));
                    else
                    {
                        code += emitInst_1srcR_1DstR("movw", "r9", "#" + to_string(r_val & 0xffff));
                        code += emitInst_1srcR_1DstR("movt", "r9", "#" + to_string((r_val >> 16) & 0xffff));
                    }
                    code += space + "cmp\t" + regm.toString(lRegId) + ", r9" + endl;
                }
            }
            else
                code += space + "cmp\t" + regm.toString(lRegId) + ", " + regm.toString(rRegId) + endl;
        }
        // 更新RVALUE,AVALUE
        if (!lconst && lhs->GetEnd() <= bInst->GetStart())
        {
            RVALUE[Register[dynamic_cast<Instruction *>(lhs)]] = nullptr;
            auto iter = AVALUE.find(dynamic_cast<Instruction *>(lhs));
            AVALUE.erase(iter);
        }
        if (!rconst && rhs->GetEnd() <= bInst->GetStart())
        {
            RVALUE[Register[dynamic_cast<Instruction *>(rhs)]] = nullptr;
            auto iter = AVALUE.find(dynamic_cast<Instruction *>(rhs));
            if (iter != AVALUE.end())
                AVALUE.erase(iter);
        }
        AVALUE[bInst].reg_num = dstRegId;
        RVALUE[dstRegId] = bInst;
        return {dstRegId, code};
    }

    pair<RegId, string> CodeGen::binaryFloatInst_gen(BinaryInst *bInst, RegId dstRegId)
    {
        string code;
        /**
         *code in here
         */
        // 得到左右操作数和目的寄存器
        string lname, rname;
        auto lhs = bInst->getLhs();
        auto rhs = bInst->getRhs();
        dstRegId = Register[bInst];
        RegId lRegId, rRegId;
        bool lconst = false, rconst = false;
        int rvalue = 0;
        int lvalue = 0;
        int instrname = stoi(bInst->getName());
        if (isa<ConstantValue>(lhs))
        {
            // float val = dynamic_cast<ConstantValue *>(lhs)->getFloat();
            float val = dynamic_cast<ConstantValue *>(lhs)->getDouble();
            memcpy(&lvalue, &val, sizeof(val));
            // unsigned int num;
            // memcpy(&num, &val, sizeof(val));
            lname = "#" + to_string(val);
            lconst = true;
        }
        else
        {
            lname = "s" + to_string(15 - stoi(lhs->getName()));
            lRegId = Register[dynamic_cast<Instruction *>(lhs)];
        }

        if (isa<ConstantValue>(rhs))
        {
            // float val = dynamic_cast<ConstantValue *>(rhs)->getFloat();
            float val = dynamic_cast<ConstantValue *>(rhs)->getDouble();
            // unsigned int num;
            memcpy(&rvalue, &val, sizeof(val));
            char s[200];
            sprintf(s, "%3.1e", val);
            rname = "#" + string(s);
            rconst = true;
        }
        else
        {
            rRegId = Register[dynamic_cast<Instruction *>(rhs)];
            rname = "s" + to_string(15 - std::stoi(rhs->getName()));
        }
        // 各种指令分开处理
        if (bInst->getKind() == Instruction::kFAdd)
        {
            if (lconst)
            {
                // 1.当立即数为const float,按float处理
                if (dynamic_cast<ConstantValue *>(lhs)->IsLvalue())
                {
                    float val_l = dynamic_cast<ConstantValue *>(lhs)->getDouble();
                    int num = 0;
                    memcpy(&num, &val_l, sizeof(float));
                    code += space + "vldr.32\ts15" + ", " + imms_name + "+" + to_string(imm_offset) + endl;
                    imm_offset += 4;
                    imms.push_back(num);
                    code += emitInst_2srcR_1dstR("vadd.f32", regm.toString(dstRegId), "s15", regm.toString(rRegId));
                }
                // 2.当立即数为常数,按double处理
                else
                {
                    double val_l = dynamic_cast<ConstantValue *>(lhs)->getDouble();
                    long long num;
                    int num1, num2;
                    memcpy(&num, &val_l, sizeof(double));
                    num1 = (num & 0x00000000FFFFFFFF);
                    num2 = (num >> 32) & 0xFFFFFFFF;
                    code += space + "vcvt.f64.f32\td16" + ", " + regm.toString(rRegId) + endl;
                    code += space + "vldr.64\td17" + ", " + imms_name + "+" + to_string(imm_offset) + endl;
                    imm_offset += 8;
                    imms.push_back(num1);
                    imms.push_back(num2);
                    code += space + "vadd.f64\td17" + ", d17" + ", d16" + endl;
                    code += space + "vcvt.f32.f64\t" + regm.toString(dstRegId) + ", d17" + endl;
                }
            }
            else if (rconst)
            {
                // 1.当立即数为const float,按float处理
                if (dynamic_cast<ConstantValue *>(rhs)->IsLvalue())
                {
                    float val_r = dynamic_cast<ConstantValue *>(rhs)->getDouble();
                    int num = 0;
                    memcpy(&num, &val_r, sizeof(float));
                    code += space + "vldr.32\ts15" + ", " + imms_name + "+" + to_string(imm_offset) + endl;
                    imm_offset += 4;
                    imms.push_back(num);
                    code += emitInst_2srcR_1dstR("vadd.f32", regm.toString(dstRegId), regm.toString(lRegId), "s15");
                }
                // 2.当立即数为常数,按double处理
                else
                {
                    double val_r = dynamic_cast<ConstantValue *>(rhs)->getDouble();
                    long long num;
                    int num1, num2;
                    memcpy(&num, &val_r, sizeof(double));
                    num1 = (num & 0x00000000FFFFFFFF);
                    num2 = (num >> 32) & 0xFFFFFFFF;
                    code += space + "vcvt.f64.f32\td16" + ", " + regm.toString(lRegId) + endl;
                    code += space + "vldr.64\td17" + ", " + imms_name + "+" + to_string(imm_offset) + endl;
                    imm_offset += 8;
                    imms.push_back(num1);
                    imms.push_back(num2);
                    code += space + "vadd.f64\td17" + ", d16" + ", d17" + endl;
                    code += space + "vcvt.f32.f64\t" + regm.toString(dstRegId) + ", d17" + endl;
                }
            }
            else
                code += emitInst_2srcR_1dstR("vadd.f32", regm.toString(dstRegId), regm.toString(lRegId), regm.toString(rRegId));
            // code += space + "vadd.f32\ts" + res + ", " + lname + ", " + rname + endl;
        }
        else if (bInst->getKind() == Instruction::kFSub)
        {
            if (lconst)
            {
                // 1.当立即数为const float,按float处理
                if (dynamic_cast<ConstantValue *>(lhs)->IsLvalue())
                {
                    float val_l = dynamic_cast<ConstantValue *>(lhs)->getDouble();
                    int num = 0;
                    memcpy(&num, &val_l, sizeof(float));
                    code += space + "vldr.32\ts15" + ", " + imms_name + "+" + to_string(imm_offset) + endl;
                    imm_offset += 4;
                    imms.push_back(num);
                    code += emitInst_2srcR_1dstR("vsub.f32", regm.toString(dstRegId), "s15", regm.toString(rRegId));
                }
                // 2.当立即数为常数,按double处理
                else
                {
                    double val_l = dynamic_cast<ConstantValue *>(lhs)->getDouble();
                    long long num;
                    int num1, num2;
                    memcpy(&num, &val_l, sizeof(double));
                    num1 = (num & 0x00000000FFFFFFFF);
                    num2 = (num >> 32) & 0xFFFFFFFF;
                    code += space + "vcvt.f64.f32\td16" + ", " + regm.toString(rRegId) + endl;
                    code += space + "vldr.64\td17" + ", " + imms_name + "+" + to_string(imm_offset) + endl;
                    imm_offset += 8;
                    imms.push_back(num1);
                    imms.push_back(num2);
                    code += space + "vsub.f64\td17" + ", d17" + ", d16" + endl;
                    code += space + "vcvt.f32.f64\t" + regm.toString(dstRegId) + ", d17" + endl;
                }
            }
            else if (rconst)
            {
                // 1.当立即数为const float,按float处理
                if (dynamic_cast<ConstantValue *>(rhs)->IsLvalue())
                {
                    float val_r = dynamic_cast<ConstantValue *>(rhs)->getDouble();
                    int num = 0;
                    memcpy(&num, &val_r, sizeof(float));
                    code += space + "vldr.32\ts15" + ", " + imms_name + "+" + to_string(imm_offset) + endl;
                    imm_offset += 4;
                    imms.push_back(num);
                    code += emitInst_2srcR_1dstR("vsub.f32", regm.toString(dstRegId), regm.toString(lRegId), "s15");
                }
                // 2.当立即数为常数,按double处理
                else
                {
                    double val_r = dynamic_cast<ConstantValue *>(rhs)->getDouble();
                    long long num;
                    int num1, num2;
                    memcpy(&num, &val_r, sizeof(double));
                    num1 = (num & 0x00000000FFFFFFFF);
                    num2 = (num >> 32) & 0xFFFFFFFF;
                    code += space + "vcvt.f64.f32\td16" + ", " + regm.toString(lRegId) + endl;
                    code += space + "vldr.64\td17" + ", " + imms_name + "+" + to_string(imm_offset) + endl;
                    imm_offset += 8;
                    imms.push_back(num1);
                    imms.push_back(num2);
                    code += space + "vsub.f64\td17" + ", d16" + ", d17" + endl;
                    code += space + "vcvt.f32.f64\t" + regm.toString(dstRegId) + ", d17" + endl;
                }
            }
            else
                code += emitInst_2srcR_1dstR("vsub.f32", regm.toString(dstRegId), regm.toString(lRegId), regm.toString(rRegId));
        }
        else if (bInst->getKind() == Instruction::kFMul)
        {
            if (lconst)
            {
                // 1.当立即数为const float,按float处理
                if (dynamic_cast<ConstantValue *>(lhs)->IsLvalue())
                {
                    float val_l = dynamic_cast<ConstantValue *>(lhs)->getDouble();
                    int num = 0;
                    memcpy(&num, &val_l, sizeof(float));
                    code += space + "vldr.32\ts15" + ", " + imms_name + "+" + to_string(imm_offset) + endl;
                    imm_offset += 4;
                    imms.push_back(num);
                    code += emitInst_2srcR_1dstR("vmul.f32", regm.toString(dstRegId), "s15", regm.toString(rRegId));
                }
                // 2.当立即数为常数,按double处理
                else
                {
                    double val_l = dynamic_cast<ConstantValue *>(lhs)->getDouble();
                    long long num;
                    int num1, num2;
                    memcpy(&num, &val_l, sizeof(double));
                    num1 = (num & 0x00000000FFFFFFFF);
                    num2 = (num >> 32) & 0xFFFFFFFF;
                    code += space + "vcvt.f64.f32\td16" + ", " + regm.toString(rRegId) + endl;
                    code += space + "vldr.64\td17" + ", " + imms_name + "+" + to_string(imm_offset) + endl;
                    imm_offset += 8;
                    imms.push_back(num1);
                    imms.push_back(num2);
                    code += space + "vmul.f64\td17" + ", d17" + ", d16" + endl;
                    code += space + "vcvt.f32.f64\t" + regm.toString(dstRegId) + ", d17" + endl;
                }
            }
            else if (rconst)
            {
                // 1.当立即数为const float,按float处理
                if (dynamic_cast<ConstantValue *>(rhs)->IsLvalue())
                {
                    float val_r = dynamic_cast<ConstantValue *>(rhs)->getDouble();
                    int num = 0;
                    memcpy(&num, &val_r, sizeof(float));
                    code += space + "vldr.32\ts15" + ", " + imms_name + "+" + to_string(imm_offset) + endl;
                    imm_offset += 4;
                    imms.push_back(num);
                    code += emitInst_2srcR_1dstR("vmul.f32", regm.toString(dstRegId), regm.toString(lRegId), "s15");
                }
                // 2.当立即数为常数,按double处理
                else
                {
                    double val_r = dynamic_cast<ConstantValue *>(rhs)->getDouble();
                    long long num;
                    int num1, num2;
                    memcpy(&num, &val_r, sizeof(double));
                    num1 = (num & 0x00000000FFFFFFFF);
                    num2 = (num >> 32) & 0xFFFFFFFF;
                    code += space + "vcvt.f64.f32\td16" + ", " + regm.toString(lRegId) + endl;
                    code += space + "vldr.64\td17" + ", " + imms_name + "+" + to_string(imm_offset) + endl;
                    imm_offset += 8;
                    imms.push_back(num1);
                    imms.push_back(num2);
                    code += space + "vmul.f64\td17" + ", d16" + ", d17" + endl;
                    code += space + "vcvt.f32.f64\t" + regm.toString(dstRegId) + ", d17" + endl;
                }
            }
            else
                code += emitInst_2srcR_1dstR("vmul.f32", regm.toString(dstRegId), regm.toString(lRegId), regm.toString(rRegId));
        }
        else if (bInst->getKind() == Instruction::kFDiv)
        {
            if (lconst)
            {
                // 1.当立即数为const float,按float处理
                if (dynamic_cast<ConstantValue *>(lhs)->IsLvalue())
                {
                    float val_l = dynamic_cast<ConstantValue *>(lhs)->getDouble();
                    int num = 0;
                    memcpy(&num, &val_l, sizeof(float));
                    code += space + "vldr.32\ts15" + ", " + imms_name + "+" + to_string(imm_offset) + endl;
                    imm_offset += 4;
                    imms.push_back(num);
                    code += emitInst_2srcR_1dstR("vdiv.f32", regm.toString(dstRegId), "s15", regm.toString(rRegId));
                }
                // 2.当立即数为常数,按double处理
                else
                {
                    double val_l = dynamic_cast<ConstantValue *>(lhs)->getDouble();
                    long long num;
                    int num1, num2;
                    memcpy(&num, &val_l, sizeof(double));
                    num1 = (num & 0x00000000FFFFFFFF);
                    num2 = (num >> 32) & 0xFFFFFFFF;
                    code += space + "vcvt.f64.f32\td16" + ", " + regm.toString(rRegId) + endl;
                    code += space + "vldr.64\td17" + ", " + imms_name + "+" + to_string(imm_offset) + endl;
                    imm_offset += 8;
                    imms.push_back(num1);
                    imms.push_back(num2);
                    code += space + "vdiv.f64\td17" + ", d17" + ", d16" + endl;
                    code += space + "vcvt.f32.f64\t" + regm.toString(dstRegId) + ", d17" + endl;
                }
            }
            else if (rconst)
            {
                // 1.当立即数为const float,按float处理
                if (dynamic_cast<ConstantValue *>(rhs)->IsLvalue())
                {
                    float val_r = dynamic_cast<ConstantValue *>(rhs)->getDouble();
                    int num = 0;
                    memcpy(&num, &val_r, sizeof(float));
                    code += space + "vldr.32\ts15" + ", " + imms_name + "+" + to_string(imm_offset) + endl;
                    imm_offset += 4;
                    imms.push_back(num);
                    code += emitInst_2srcR_1dstR("vdiv.f32", regm.toString(dstRegId), regm.toString(lRegId), "s15");
                }
                // 2.当立即数为常数,按double处理
                else
                {
                    double val_r = dynamic_cast<ConstantValue *>(rhs)->getDouble();
                    long long num;
                    int num1, num2;
                    memcpy(&num, &val_r, sizeof(double));
                    num1 = (num & 0x00000000FFFFFFFF);
                    num2 = (num >> 32) & 0xFFFFFFFF;
                    code += space + "vcvt.f64.f32\td16" + ", " + regm.toString(lRegId) + endl;
                    code += space + "vldr.64\td17" + ", " + imms_name + "+" + to_string(imm_offset) + endl;
                    imm_offset += 8;
                    imms.push_back(num1);
                    imms.push_back(num2);
                    code += space + "vdiv.f64\td17" + ", d16" + ", d17" + endl;
                    code += space + "vcvt.f32.f64\t" + regm.toString(dstRegId) + ", d17" + endl;
                }
            }
            else
                code += emitInst_2srcR_1dstR("vdiv.f32", regm.toString(dstRegId), regm.toString(lRegId), regm.toString(rRegId));
        }
        else if (lconst && rconst)
            return {dstRegId, code};
        else if (bInst->getKind() == Instruction::kFCmpEQ)
        {
            if (lconst)
            {
                code += space + "movw\tr9" + ", #" + to_string(lvalue & 0xffff) + endl;
                code += space + "movt\tr9" + ", #" + to_string((lvalue >> 16) & 0xffff) + endl;
                code += emitInst_1srcR_1DstR("vmov", "s14", "r9");
                code += emitInst_2srcR("vcmpe.f32", "s14", regm.toString(rRegId));
            }
            else if (rconst)
            {
                // string immname = to_string(15 - instrname);
                code += space + "movw\tr9" + ", #" + to_string(rvalue & 0xffff) + endl;
                code += space + "movt\tr9" + ", #" + to_string((rvalue >> 16) & 0xffff) + endl;
                code += emitInst_1srcR_1DstR("vmov", "s14", "r9");
                code += emitInst_2srcR("vcmpe.f32", regm.toString(lRegId), "s14");
                // code += space + "vmov\ts" + immname + ", r" + to_string(instrname) + endl;
                // code += space + "vcmpe.f32\t" + lname + ", s" + immname + endl;
            }
            else
                code += emitInst_2srcR("vcmpe.f32", regm.toString(lRegId), regm.toString(rRegId));
            code += space + "vmrs\tAPSR_nzcv, FPSCR" + endl;
        }
        else if (bInst->getKind() == Instruction::kFCmpGE)
        {
            if (lconst)
            {
                code += space + "movw\tr9" + ", #" + to_string(lvalue & 0xffff) + endl;
                code += space + "movt\tr9" + ", #" + to_string((lvalue >> 16) & 0xffff) + endl;
                code += emitInst_1srcR_1DstR("vmov", "s14", "r9");
                code += emitInst_2srcR("vcmpe.f32", "s14", regm.toString(rRegId));
            }
            else if (rconst)
            {
                // string immname = to_string(15 - instrname);
                code += space + "movw\tr9" + ", #" + to_string(rvalue & 0xffff) + endl;
                code += space + "movt\tr9" + ", #" + to_string((rvalue >> 16) & 0xffff) + endl;
                code += emitInst_1srcR_1DstR("vmov", "s14", "r9");
                code += emitInst_2srcR("vcmpe.f32", regm.toString(lRegId), "s14");
                // code += space + "vmov\ts" + immname + ", r" + to_string(instrname) + endl;
                // code += space + "vcmpe.f32\t" + lname + ", s" + immname + endl;
            }
            else
                code += emitInst_2srcR("vcmpe.f32", regm.toString(lRegId), regm.toString(rRegId));
            code += space + "vmrs\tAPSR_nzcv, FPSCR" + endl;
        }
        else if (bInst->getKind() == Instruction::kFCmpGT)
        {
            if (lconst)
            {
                code += space + "movw\tr9" + ", #" + to_string(lvalue & 0xffff) + endl;
                code += space + "movt\tr9" + ", #" + to_string((lvalue >> 16) & 0xffff) + endl;
                code += emitInst_1srcR_1DstR("vmov", "s14", "r9");
                code += emitInst_2srcR("vcmpe.f32", "s14", regm.toString(rRegId));
            }
            else if (rconst)
            {
                code += space + "movw\tr9" + ", #" + to_string(rvalue & 0xffff) + endl;
                code += space + "movt\tr9" + ", #" + to_string((rvalue >> 16) & 0xffff) + endl;
                code += emitInst_1srcR_1DstR("vmov", "s14", "r9");
                code += emitInst_2srcR("vcmpe.f32", regm.toString(lRegId), "s14");
            }
            else
                code += emitInst_2srcR("vcmpe.f32", regm.toString(lRegId), regm.toString(rRegId));
            code += space + "vmrs\tAPSR_nzcv, FPSCR" + endl;
        }
        else if (bInst->getKind() == Instruction::kFCmpLE)
        {
            if (lconst)
            {
                code += space + "movw\tr9" + ", #" + to_string(lvalue & 0xffff) + endl;
                code += space + "movt\tr9" + ", #" + to_string((lvalue >> 16) & 0xffff) + endl;
                code += emitInst_1srcR_1DstR("vmov", "s14", "r9");
                code += emitInst_2srcR("vcmpe.f32", "s14", regm.toString(rRegId));
            }
            else if (rconst)
            {
                code += space + "movw\tr9" + ", #" + to_string(rvalue & 0xffff) + endl;
                code += space + "movt\tr9" + ", #" + to_string((rvalue >> 16) & 0xffff) + endl;
                code += emitInst_1srcR_1DstR("vmov", "s14", "r9");
                code += emitInst_2srcR("vcmpe.f32", regm.toString(lRegId), "s14");
            }
            else
                code += emitInst_2srcR("vcmpe.f32", regm.toString(lRegId), regm.toString(rRegId));
            code += space + "vmrs\tAPSR_nzcv, FPSCR" + endl;
        }
        else if (bInst->getKind() == Instruction::kFCmpLT)
        {
            if (lconst)
            {
                code += space + "movw\tr9" + ", #" + to_string(lvalue & 0xffff) + endl;
                code += space + "movt\tr9" + ", #" + to_string((lvalue >> 16) & 0xffff) + endl;
                code += emitInst_1srcR_1DstR("vmov", "s14", "r9");
                code += emitInst_2srcR("vcmpe.f32", "s14", regm.toString(rRegId));
            }
            else if (rconst)
            {
                code += space + "movw\tr9" + ", #" + to_string(rvalue & 0xffff) + endl;
                code += space + "movt\tr9" + ", #" + to_string((rvalue >> 16) & 0xffff) + endl;
                code += emitInst_1srcR_1DstR("vmov", "s14", "r9");
                code += emitInst_2srcR("vcmpe.f32", regm.toString(lRegId), "s14");
            }
            else
                code += emitInst_2srcR("vcmpe.f32", regm.toString(lRegId), regm.toString(rRegId));
            code += space + "vmrs\tAPSR_nzcv, FPSCR" + endl;
        }
        else if (bInst->getKind() == Instruction::kFCmpNE)
        {
            if (lconst)
            {
                code += space + "movw\tr9" + ", #" + to_string(lvalue & 0xffff) + endl;
                code += space + "movt\tr9" + ", #" + to_string((lvalue >> 16) & 0xffff) + endl;
                code += emitInst_1srcR_1DstR("vmov", "s14", "r9");
                code += emitInst_2srcR("vcmpe.f32", "s14", regm.toString(rRegId));
            }
            else if (rconst)
            {
                code += space + "movw\tr9" + ", #" + to_string(rvalue & 0xffff) + endl;
                code += space + "movt\tr9" + ", #" + to_string((rvalue >> 16) & 0xffff) + endl;
                code += emitInst_1srcR_1DstR("vmov", "s14", "r9");
                code += emitInst_2srcR("vcmpe.f32", regm.toString(lRegId), "s14");
            }
            else
                code += emitInst_2srcR("vcmpe.f32", regm.toString(lRegId), regm.toString(rRegId));
            code += space + "vmrs\tAPSR_nzcv, FPSCR" + endl;
        }
        // update RVALUE and AVALUE
        if (!lconst && lhs->GetEnd() <= bInst->GetStart())
        {
            RVALUE[Register[dynamic_cast<Instruction *>(lhs)]] = nullptr;
            auto iter = AVALUE.find(dynamic_cast<Instruction *>(lhs));
            AVALUE.erase(iter);
        }
        if (!rconst && rhs->GetEnd() <= bInst->GetStart())
        {
            RVALUE[Register[dynamic_cast<Instruction *>(rhs)]] = nullptr;
            auto iter = AVALUE.find(dynamic_cast<Instruction *>(rhs));
            if (iter != AVALUE.end())
                AVALUE.erase(iter);
        }
        AVALUE[bInst].reg_num = dstRegId;
        RVALUE[dstRegId] = bInst;
        return {dstRegId, code};
    }

    pair<RegId, string> CodeGen::unaryInst_gen(UnaryInst *uInst, RegId dstRegId)
    {
        string code;
        /**
         *code in here
         */
        auto val = uInst->getOperand();
        dstRegId = Register[uInst];
        string val_name;
        if (uInst->getKind() == Instruction::kNeg)
        {
            if (isa<ConstantValue>(val))
            {
                val_name = to_string(dynamic_cast<ConstantValue *>(val)->getInt());
                code += space + "mov\t" + regm.toString(dstRegId) + ", #" + val_name + endl;
                code += space + "mvn\t" + regm.toString(dstRegId) + ", #1" + endl;
            }
            else
            {
                RegId srcreg = Register[dynamic_cast<Instruction *>(val)];
                if (srcreg == RegManager::NONE)
                {
                    srcreg = RegManager::R9;
                    code += emitInst_mem("ldr", "r9", "fp", AVALUE[dynamic_cast<Instruction *>(val)].stack_offset);
                }
                if (dstRegId == RegManager::NONE)
                    dstRegId = RegManager::R9;
                code += space + "rsb\t" + regm.toString(dstRegId) + ", " + regm.toString(srcreg) + ", #0" + endl;
                if (dstRegId == RegManager::R9)
                {
                    dstRegId = RegManager::NONE;
                    AVALUE[uInst].stack_offset = uInst->GetLocation();
                    code += emitInst_mem("str", "r9", "fp", uInst->GetLocation());
                }
            }
        }
        else if (uInst->getKind() == Instruction::kFNeg)
        {
            RegId srcreg = Register[dynamic_cast<Instruction *>(val)];
            if (srcreg == RegManager::NONE)
            {
                srcreg = RegManager::S14;
                code += emitInst_mem("vldr.32", "s14", "fp", AVALUE[dynamic_cast<Instruction *>(val)].stack_offset);
            }
            if (dstRegId == RegManager::NONE)
                dstRegId = RegManager::S14;
            code += emitInst_1srcR_1DstR("vneg.f32", regm.toString(dstRegId), regm.toString(srcreg));
            if (dstRegId == RegManager::S14)
            {
                dstRegId = RegManager::NONE;
                AVALUE[uInst].stack_offset = uInst->GetLocation();
                code += emitInst_mem("vstr.32", "s14", "fp", uInst->GetLocation());
            }
        }
        else if (uInst->getKind() == Instruction::kFtoI)
        {
            RegId srcreg = Register[dynamic_cast<Instruction *>(val)];
            if (srcreg == RegManager::NONE)
            {
                srcreg = RegManager::S14;
                code += emitInst_mem("vldr.32", "s14", "fp", AVALUE[dynamic_cast<Instruction *>(val)].stack_offset);
            }
            if (dstRegId == RegManager::NONE)
                dstRegId = RegManager::R9;
            code += emitInst_1srcR_1DstR("vcvt.s32.f32", regm.toString(srcreg), regm.toString(srcreg));
            code += emitInst_1srcR_1DstR("vmov", regm.toString(dstRegId), regm.toString(srcreg));
            if (dstRegId == RegManager::R9)
            {
                dstRegId = RegManager::NONE;
                AVALUE[uInst].stack_offset = uInst->GetLocation();
                code += emitInst_mem("str", "r9", "fp", uInst->GetLocation());
            }
        }
        else if (uInst->getKind() == Instruction::kItoF)
        {
            RegId srcreg = Register[dynamic_cast<Instruction *>(val)];
            if (srcreg == RegManager::NONE)
            {
                srcreg = RegManager::R9;
                code += emitInst_mem("ldr", "r9", "fp", AVALUE[dynamic_cast<Instruction *>(val)].stack_offset);
            }
            if (dstRegId == RegManager::NONE)
                dstRegId = RegManager::S14;
            code += space + "vmov\t" + regm.toString(dstRegId) + ", " + regm.toString(srcreg) + endl;
            code += space + "vcvt.f32.s32\t" + regm.toString(dstRegId) + ", " + regm.toString(dstRegId) + endl;
            if (dstRegId == RegManager::S14)
            {
                dstRegId = RegManager::NONE;
                AVALUE[uInst].stack_offset = uInst->GetLocation();
                code += emitInst_mem("vstr.32", "s14", "fp", uInst->GetLocation());
            }
        }
        if (!isa<ConstantValue>(val) && val->GetEnd() <= uInst->GetStart())
        {
            RVALUE[Register[dynamic_cast<Instruction *>(val)]] = nullptr;
            auto iter = AVALUE.find(dynamic_cast<Instruction *>(val));
            AVALUE.erase(iter);
        }
        AVALUE[uInst].reg_num = dstRegId;
        RVALUE[dstRegId] = uInst;
        return {dstRegId, code};
    }
    pair<RegId, string>
    CodeGen::allocaInst_gen(AllocaInst *aInst, RegManager::RegId dstRegId)
    {
        string code;
        // 计算元素个数
        int NumDims = aInst->getNumDims();
        auto Dims = aInst->getDims();
        int num = 1;
        for (auto iter = Dims.begin(); iter != Dims.end(); iter++)
            num *= static_cast<const ConstantValue *>(*iter)->getInt();
        // 对超过1个元素的数组进行初始化
        if (NumDims && num >= 2)
        {
            int imm = -localVarStOffset[aInst];
            if (imm < 256)
                code += space + "sub\tr3, fp, #" + to_string(imm) + endl;
            else
            {
                code += space + "sub\tr3, fp, #" + to_string(imm / 256 * 256) + endl;
                if (imm % 256 != 0)
                    code += space + "sub\tr3, r3, #" + to_string(imm % 256) + endl;
            }
            if (num >= 18)
            {
                code += space + "mov\tr2, #" + to_string(4 * num) + endl;
                code += space + "mov\tr1, #0" + endl;
                code += space + "mov\tr0, r3" + endl;
                code += space + "bl\tmemset" + endl;
                haveCall = true;
            }
            // else if (num % 4 == 0)
            // {
            //     code += space + "vmov.i32\tq8, #0\t@ v16qi" + endl;
            //     code += space + "vst1.8\t{q8}, [r3:64]" + endl;
            //     num -= 4;
            //     while (num != 0)
            //     {
            //         num -= 4;
            //         code += space + "add\tr3, #16" + endl;
            //         code += space + "vst1.8\t{q8}, [r3:64]" + endl;
            //     }
            // }
            else if (num % 2 == 0)
            {
                code += space + "vmov.i32\td18, #0\t@ v8qi" + endl;
                for (int i = 0; i < num / 2; i++)
                {
                    code += space + "vstr\td18, [r3, #" + to_string(i * 8) + "]" + endl;
                }
            }
            else
            {
                int i;
                code += space + "vmov.i32\td18, #0\t@ v8qi" + endl;
                for (i = 0; i < num / 2; i++)
                {
                    code += space + "vstr\td18, [r3, #" + to_string(i * 8) + "]" + endl;
                }
                code += space + "vstr\td18, [r3, #" + to_string(i * 8 - 4) + "]" + endl;
            }
        }
        dstRegId = RegManager::NONE;
        return {dstRegId, code};
    }
    string CodeGen::storeInst_gen(StoreInst *stInst)
    {
        string code;
        auto value = stInst->getValue();
        auto pointer = stInst->getPointer();
        auto type = pointer->getType()->as<PointerType>()->getBaseType();
        int pos = 0;                              // variable's position in the stack
        int NumIndices = stInst->getNumIndices(); // numindices denotes dimensions of pointer

        // if pointer is a localvariable
        if (localVarStOffset.find(dynamic_cast<Instruction *>(pointer)) != localVarStOffset.end())
        {
            // 局部变量的维度,(起始)地址偏移量
            auto local_variable = dynamic_cast<AllocaInst *>(pointer);
            int NumDims = local_variable->getNumDims();
            pos = localVarStOffset[dynamic_cast<Instruction *>(pointer)];
            bool flag = false;          // 索引为常量=false/变量=true
            Value *first_var = nullptr; // 索引中出现的第一个变量
            int offset_array = 0;       // 求出元素的偏移
            int dim = NumDims - 1;
            vector<int> dim_len;
            vector<int> full_num;
            // 局部标量的偏移量(已求出)
            // 局部数组的偏移量
            if (NumDims > 0)
            {
                for (int i = 0; i < NumDims; i++)
                    dim_len.push_back(dynamic_cast<ConstantValue *>(local_variable->getDim(i))->getInt());
                reverse(dim_len.begin(), dim_len.end());
                full_num.push_back(1);
                for (int i = 0; i < NumIndices; i++)
                    full_num.push_back(dim_len[i] * full_num[i]);
                for (auto iter = stInst->getIndices().begin(); iter != stInst->getIndices().end(); iter++)
                {
                    int index = 0; // index 是数组下标
                    if (isa<ConstantValue>(*iter))
                        index = dynamic_cast<ConstantValue *>(*iter)->getInt();
                    else if (flag == false)
                    {
                        flag = true;
                        first_var = *iter;
                    }
                    offset_array += index * full_num[dim--];
                }
            }
            // 标量/数组下标均为常数
            if (flag == false)
            {
                pos += offset_array * 4;
                int imm = -pos;
                // 存入常数
                if (isa<ConstantValue>(value))
                {
                    if (value->isInt())
                    {
                        int constant_value = dynamic_cast<ConstantValue *>(value)->getInt();
                        code += emitInst_nosrcR_1DstR("movw", "r9", (constant_value & 0xffff));
                        code += emitInst_nosrcR_1DstR("movt", "r9", ((constant_value >> 16) & 0xffff));
                        if (imm < 256)
                            code += emitInst_mem("str", "r9", "fp", pos);
                        else if (imm < 65536)
                        {
                            code += emitInst_1srcR_1DstR("sub", "r10", "fp", imm / 256 * 256);
                            if (imm % 256 != 0)
                                code += emitInst_1srcR_1DstR("sub", "r10", "r10", imm % 256);
                            code += emitInst_mem("str", "r9", "r10");
                        }
                        else
                        {
                            code += emitInst_1srcR_1DstR("sub", "r10", "fp", imm / 4096 * 4096);
                            if (imm % 4096 != 0)
                            {
                                imm = imm % 4096;
                                code += emitInst_1srcR_1DstR("sub", "r10", "r10", imm / 256 * 256);
                                if (imm % 256 != 0)
                                    code += emitInst_1srcR_1DstR("sub", "r10", "r10", imm % 256);
                                code += emitInst_mem("str", "r9", "r10");
                            }
                        }
                        // int constant_value = dynamic_cast<ConstantValue *>(value)->getInt();
                        // code += space + "movw\tr3, #" + to_string(unsigned(constant_value & 0x0000FFFF)) + endl;
                        // code += space + "movt\tr3, #" + to_string(unsigned((constant_value >> 16) & 0x0000FFFF)) + endl;
                        // if (imm < 256)
                        //     code += space + "str\tr3, [fp, #" + to_string(pos) + "]" + endl;
                        // else
                        // {
                        //     code += space + "sub\tr4, fp, #" + to_string(imm / 256 * 256) + endl;
                        //     if (imm % 256 != 0)
                        //         code += space + "sub\tr4, r4, #" + to_string(imm % 256) + endl;
                        //     code += space + "str\tr3, [r4]" + endl;
                        // }
                    }
                    else if (value->isFloat())
                    {
                        float constant_value = dynamic_cast<ConstantValue *>(value)->getDouble();
                        unsigned dec;
                        std::memcpy(&dec, &constant_value, sizeof(dec));
                        code += emitInst_nosrcR_1DstR("movw", "r9", (dec & 0xffff));
                        code += emitInst_nosrcR_1DstR("movt", "r9", ((dec >> 16) & 0xffff));
                        if (imm < 256)
                            code += emitInst_mem("str", "r9", "fp", pos);
                        else
                        {
                            code += emitInst_1srcR_1DstR("sub", "r10", "fp", imm / 256 * 256);
                            if (imm % 256 != 0)
                                code += emitInst_1srcR_1DstR("sub", "r10", "r10", imm % 256);
                            code += space + "str\tr9, [r10]\t@float" + endl;
                        }
                        // code += space + "movw\tr3, #" + to_string(unsigned(dec & 0x0000FFFF)) + endl;
                        // code += space + "movt\tr3, #" + to_string(unsigned((dec >> 16) & 0xffff)) + endl;
                        // if (imm < 256)
                        //     code += space + "str\tr3, [fp, #" + to_string(pos) + "]\t@ float" + endl;
                        // else
                        // {
                        //     code += space + "sub\tr4, fp, #" + to_string(imm / 256 * 256) + endl;
                        //     if (imm % 256 != 0)
                        //         code += space + "sub\tr4, r4, #" + to_string(imm % 256) + endl;
                        //     code += space + "str\tr3, [r4]\t@ float" + endl;
                        // }
                    }
                }
                // 存入其他值
                else
                {
                    RegId srcRegId = Register[dynamic_cast<Instruction *>(value)];
                    if (value->getType()->isInt())
                    {
                        if (srcRegId == RegManager::NONE)
                        {
                            srcRegId = RegManager::R9;
                            code += emitInst_mem("ldr", "r9", "fp", AVALUE[dynamic_cast<Instruction *>(value)].stack_offset);
                        }
                        if (imm < 256)
                            // code += space + "str\tr" + value->getName() + ", [fp, #" + to_string(pos) + "]" + endl;
                            code += emitInst_mem("str", regm.toString(srcRegId), "fp", pos);
                        else
                        {
                            code += emitInst_1srcR_1DstR("sub", "r10", "fp", imm / 256 * 256);
                            if (imm % 256 != 0)
                                code += emitInst_1srcR_1DstR("sub", "r10", "r10", imm % 256);
                            code += emitInst_mem("str", regm.toString(srcRegId), "r10");
                            // int addr_reg = (1 + std::stoi(value->getName())) % 11;
                            // code += space + "sub\tr" + to_string(addr_reg) + ", fp, #" + to_string(imm / 256 * 256) + endl;
                            // if (imm % 256)
                            //     code += space + "sub\tr" + to_string(addr_reg) + ", r" + to_string(addr_reg) + ", #" + to_string(imm % 256) + endl;
                            // code += space + "str\tr" + value->getName() + ", [r" + to_string(addr_reg) + "]" + endl;
                        }
                    }
                    else if (value->getType()->isFloat())
                    {
                        if (srcRegId == RegManager::NONE)
                        {
                            srcRegId = RegManager::S14;
                            code += emitInst_mem("vldr.32", "s14", "fp", AVALUE[dynamic_cast<Instruction *>(value)].stack_offset);
                        }
                        if (imm < 256)
                            // code += space + "vstr.32\ts" + to_string(15 - std::stoi(value->getName())) + ", [fp, #" + to_string(pos) + "]" + endl;
                            code += emitInst_mem("vstr.32", regm.toString(srcRegId), "fp", pos);
                        else
                        {
                            code += emitInst_1srcR_1DstR("sub", "r10", "fp", imm / 256 * 256);
                            if (imm % 256 != 0)
                                code += emitInst_1srcR_1DstR("sub", "r10", "r10", imm % 256);
                            code += emitInst_mem("vstr.32", regm.toString(srcRegId), "r10");
                            // int addr_reg = (1 + std::stoi(value->getName())) % 11;
                            // code += space + "sub\tr" + to_string(addr_reg) + ", fp, #" + to_string(imm / 256 * 256) + endl;
                            // if (imm % 256)
                            //     code += space + "sub\tr" + to_string(addr_reg) + ", r" + to_string(addr_reg) + ", #" + to_string(imm % 256) + endl;
                            // code += space + "vstr.32\ts" + to_string(15 - std::stoi(value->getName())) + ", [r" + to_string(addr_reg) + "]" + endl;
                        }
                    }
                }
            }
            // 数组下标含有变量
            else
            {
                // 借用R8作为地址寄存器,将存在R8的变量保护起来
                Instruction *protect_value = nullptr;
                if (RVALUE.find(RegManager::R8) != RVALUE.end())
                    protect_value = RVALUE[RegManager::R8];
                if (protect_value)
                {
                    int imm = -protect_reg_offset;
                    if (imm < 256)
                        code += emitInst_mem("str", "r8", "fp", protect_reg_offset);
                    else
                    {
                        code += emitInst_1srcR_1DstR("sub", "r9", "fp", imm / 256 * 256);
                        if (imm % 256 != 0)
                            code += emitInst_1srcR_1DstR("sub", "r9", "r9", imm % 256);
                        code += emitInst_mem("str", "r8", "r9");
                    }
                    AVALUE[protect_value].reg_num = RegManager::NONE;
                    AVALUE[protect_value].stack_offset = protect_reg_offset;
                }
                dim = NumDims - 1;
                flag = false;
                for (auto iter = stInst->getIndices().begin(); iter != stInst->getIndices().end(); iter++)
                {
                    if (isa<ConstantValue>(*iter))
                        dim--;
                    else if (flag == false)
                    {
                        code += emitInst_nosrcR_1DstR("mov", "r8", offset_array);
                        auto value = dynamic_cast<Instruction *>(*iter);
                        code += emitInst_nosrcR_1DstR("movw", "r10", (full_num[dim] & 0x0000ffff));
                        code += emitInst_nosrcR_1DstR("movt", "r10", ((full_num[dim] >> 16) & 0xffff));
                        dim--;
                        if (AVALUE[value].reg_num == RegManager::NONE)
                        {
                            code += emitInst_mem("ldr", "r9", "fp", AVALUE[value].stack_offset);
                            code += space + "mla\tr8, r9, r10, r8" + endl;
                        }
                        else
                            code += space + "mla\tr8, " + regm.toString(AVALUE[value].reg_num) + ", r10, r8" + endl;
                        flag = true;
                        // code += space + "movw\tr10, #" + to_string(unsigned(full_num[dim] & 0x0000ffff)) + endl;
                        // code += space + "movt\tr10, #" + to_string(unsigned((full_num[dim--] >> 16) & 0xffff)) + endl;
                        // code += space + "mul\tr" + (*iter)->getName() + ", r" + (*iter)->getName() + ", r10" + endl;
                        // code += space + "add\tr" + (*iter)->getName() + ", r" + (*iter)->getName() + ", #" + to_string(offset_array) + endl;
                        // flag = true;
                    }
                    else
                    {
                        auto value = dynamic_cast<Instruction *>(*iter);
                        code += emitInst_nosrcR_1DstR("movw", "r10", (full_num[dim] & 0x0000FFFF));
                        code += emitInst_nosrcR_1DstR("movt", "r10", ((full_num[dim] >> 16) & 0xFFFF));
                        dim--;
                        if (AVALUE[value].reg_num == RegManager::NONE)
                        {
                            int imm = -AVALUE[value].stack_offset;
                            if (imm < 256)
                                code += emitInst_mem("ldr", "r9", "fp", AVALUE[value].stack_offset);
                            else
                            {
                                code += emitInst_1srcR_1DstR("sub", "r9", "fp", imm / 256 * 256);
                                if (imm % 256 != 0)
                                    code += emitInst_1srcR_1DstR("sub", "r9", "r9", imm % 256);
                                code += emitInst_mem("ldr", "r9", "r9");
                            }
                            code += space + "mla\tr8, r9, r10, r8" + endl;
                        }
                        else
                            code += space + "mla\tr8, " + regm.toString(AVALUE[value].reg_num) + ", r10, r8" + endl;
                        // code += space + "movw\tr10, #" + to_string(unsigned(full_num[dim] & 0x0000FFFF)) + endl;
                        // code += space + "movt\tr10, #" + to_string(unsigned((full_num[dim--] >> 16) & 0xffff)) + endl;
                        // code += space + "mla\tr" + first_var->getName() + ", r" + (*iter)->getName() + ", r10, r" + first_var->getName() + endl;
                    }
                }
                code += emitInst_1srcR_1DstR("lsl", "r8", "r8", 2);
                int imm = -pos;
                // code += to_string(imm) + endl;
                if (imm < 256)
                    code += emitInst_1srcR_1DstR("sub", "r8", "r8", imm);
                else
                {
                    code += emitInst_1srcR_1DstR("sub", "r8", "r8", imm / 256 * 256);
                    if (imm % 256 != 0)
                        code += emitInst_1srcR_1DstR("sub", "r8", "r8", imm % 256);
                }
                code += emitInst_2srcR_1dstR("add", "r8", "r8", "fp");
                // code += space + "lsl\tr" + first_var->getName() + ", r" + first_var->getName() + ", #2" + endl;
                // int imm = -pos;
                // if (imm < 256)
                //     code += space + "sub\tr" + first_var->getName() + ", r" + first_var->getName() + ", #" + to_string(-pos) + endl;
                // else
                // {
                //     code += space + "sub\tr" + first_var->getName() + ", r" + first_var->getName() + ", #" + to_string(imm / 256 * 256) + endl;
                //     if (imm % 256 != 0)
                //         code += space + "sub\tr" + first_var->getName() + ", r" + first_var->getName() + ", #" + to_string(imm % 256) + endl;
                // }
                // code += space + "add\tr" + first_var->getName() + ", r" + first_var->getName() + ", fp" + endl;

                // 存入常数
                if (isa<ConstantValue>(value))
                {

                    // int src_reg = std::stoi(first_var->getName()) + 1;
                    if (value->isInt())
                    {
                        int constant_value = dynamic_cast<ConstantValue *>(value)->getInt();
                        code += emitInst_nosrcR_1DstR("movw", "r9", (constant_value & 0x0000FFFF));
                        code += emitInst_nosrcR_1DstR("movt", "r9", ((constant_value >> 16) & 0xffff));
                        code += emitInst_mem("str", "r9", "r8");
                        // code += space + "movw\tr" + to_string(src_reg) + ", #" + to_string(unsigned(constant_value & 0x0000FFFF)) + endl;
                        // code += space + "movt\tr" + to_string(src_reg) + ", #" + to_string(unsigned((constant_value >> 16) & 0xffff)) + endl;
                        // code += space + "str\tr" + to_string(src_reg) + ", [r" + first_var->getName() + "] " + endl;
                    }
                    else if (value->isFloat())
                    {
                        float constant_value = dynamic_cast<ConstantValue *>(value)->getDouble();
                        unsigned dec;
                        std::memcpy(&dec, &constant_value, sizeof(dec));
                        code += emitInst_nosrcR_1DstR("movw", "r9", (dec & 0x0000FFFF));
                        code += emitInst_nosrcR_1DstR("movt", "r9", ((dec >> 16) & 0xffff));
                        code += space + "str\tr9, [r8]\t@ float" + endl;
                        // code += space + "movw\tr" + to_string(src_reg) + ", #" + to_string(dec & 0x0000FFFF) + endl;
                        // code += space + "movt\tr" + to_string(src_reg) + ", #" + to_string((dec >> 16) & 0xffff) + endl;
                        // code += space + "str\tr" + to_string(src_reg) + ", [r" + first_var->getName() + "]\t@ float" + endl;
                    }
                }
                // 存入其他值
                else
                {
                    auto src_value = dynamic_cast<Instruction *>(value);
                    RegId srcRegId = Register[src_value];
                    if (value->getType()->isInt())
                    {
                        if (AVALUE[src_value].reg_num == RegManager::NONE)
                        {
                            code += emitInst_mem("ldr", "r9", "fp", AVALUE[src_value].stack_offset);
                            code += emitInst_mem("str", "r9", "r8");
                        }
                        else
                            code += emitInst_mem("str", regm.toString(AVALUE[src_value].reg_num), "r8");
                        // code += space + "str\tr" + value->getName() + ", [r" + first_var->getName() + "]" + endl;
                    }
                    else if (value->getType()->isFloat())
                    {
                        if (AVALUE[src_value].reg_num == RegManager::NONE)
                        {
                            code += emitInst_mem("vldr.32", "s14", "fp", AVALUE[src_value].stack_offset);
                            code += emitInst_mem("vstr.32", "s14", "r8");
                        }
                        else
                            code += emitInst_mem("vstr.32", regm.toString(AVALUE[src_value].reg_num), "r8");
                    }
                    // code += space + "vstr.32\ts" + to_string(15 - std::stoi(value->getName())) + ", [r" + first_var->getName() + "]" + endl;
                }
                // 恢复R8对应的变量,仅当其在store指令后还活跃
                if (protect_value && protect_value->GetEnd() > stInst->GetStart())
                {
                    AVALUE[protect_value].reg_num = RegManager::R8;
                }
            }
        }
        // if pointer is a globalvalue
        else if (globalval.find(dynamic_cast<GlobalValue *>(pointer)) != globalval.end())
        {
            // 全局变量的维度,(起始)地址偏移量
            auto global_value = dynamic_cast<GlobalValue *>(pointer);
            int NumDims = global_value->getNumDims();
            pos = 0;
            bool flag = false;          // 索引为常量=false/变量=true
            Value *first_var = nullptr; // 索引中出现的第一个变量
            int offset_array = 0;       // 求出元素的偏移
            int dim = NumDims - 1;
            vector<int> dim_len;
            vector<int> full_num;
            // 获取全局数组(首)地址
            // code += space + "movw\tr10, #:lower16:" + pointer->getName() + endl;
            // code += space + "movt\tr10, #:upper16:" + pointer->getName() + endl;

            // 全局数组的偏移量
            if (NumDims > 0)
            {
                for (int i = 0; i < NumDims; i++)
                    dim_len.push_back(dynamic_cast<ConstantValue *>(global_value->getDim(i))->getInt());
                reverse(dim_len.begin(), dim_len.end());
                full_num.push_back(1);
                for (int i = 0; i < NumIndices; i++)
                    full_num.push_back(dim_len[i] * full_num[i]);
                for (auto iter = stInst->getIndices().begin(); iter != stInst->getIndices().end(); iter++)
                {
                    int index = 0; // index 是数组下标
                    if (isa<ConstantValue>(*iter))
                        index = dynamic_cast<ConstantValue *>(*iter)->getInt();
                    else if (flag == false)
                    {
                        flag = true;
                        first_var = *iter;
                    }
                    offset_array += index * full_num[dim--];
                }
                pos += offset_array * 4;
            }
            // 标量/数组下标均为常数
            if (flag == false)
            {
                code += emitInst_1srcR_1DstR("movw", "r9", "#:lower16:" + pointer->getName());
                code += emitInst_1srcR_1DstR("movt", "r9", "#:upper16:" + pointer->getName());
                int imm = pos;
                // 存入常数
                if (isa<ConstantValue>(value))
                {
                    if (value->isInt())
                    {
                        int constant_value = dynamic_cast<ConstantValue *>(value)->getInt();
                        code += emitInst_nosrcR_1DstR("movw", "r10", (constant_value & 0x0000FFFF));
                        code += emitInst_nosrcR_1DstR("movt", "r10", ((constant_value >> 16) & 0xffff));
                        // code += space + "movw\tr3, #" + to_string(constant_value & 0x0000FFFF) + endl;
                        // code += space + "movt\tr3, #" + to_string((constant_value >> 16) & 0xffff) + endl;
                        if (imm < 256)
                            // code += space + "str\tr3, [r10, #" + to_string(pos) + "]" + endl;
                            code += emitInst_mem("str", "r10", "r9", pos);
                        else if (imm < 65536)
                        {
                            code += emitInst_1srcR_1DstR("add", "r9", "r9", imm / 256 * 256);
                            if (imm % 256 != 0)
                                code += emitInst_1srcR_1DstR("add", "r9", "r9", imm % 256);
                            code += emitInst_mem("str", "r10", "r9");
                            // code += space + "add\tr10, r10, #" + to_string(imm / 256 * 256) + endl;
                            // if (imm % 256 != 0)
                            //     code += space + "add\tr10, r10, #" + to_string(imm % 256) + endl;
                            // code += space + "str\tr3, [r10]" + endl;
                        }
                        else
                        {
                            code += emitInst_1srcR_1DstR("add", "r9", "r9", imm / 4096 * 4096);
                            if (imm % 4096 != 0)
                            {
                                imm = imm % 4096;
                                code += emitInst_1srcR_1DstR("add", "r9", "r9", imm / 256 * 256);
                                if (imm % 256)
                                    code += emitInst_1srcR_1DstR("add", "r9", "r9", imm % 256);
                                code += emitInst_mem("str", "r10", "r9");
                            }
                            // code += space + "add\tr10, r10, #" + to_string(imm / 4096 * 4096) + endl;
                            // if (imm % 4096 != 0)
                            // {
                            //     imm = imm % 4096;
                            //     code += space + "add\tr10, r10, #" + to_string(imm / 256 * 256) + endl;
                            //     if (imm % 256)
                            //         code += space + "add\tr10, r10, #" + to_string(imm % 256) + endl;
                            // }
                            // code += space + "str\tr3, [r10]" + endl;
                        }
                    }
                    else if (value->isFloat())
                    {
                        float constant_value = dynamic_cast<ConstantValue *>(value)->getDouble();
                        unsigned dec;
                        std::memcpy(&dec, &constant_value, sizeof(dec));
                        code += emitInst_nosrcR_1DstR("movw", "r10", (dec & 0x0000FFFF));
                        code += emitInst_nosrcR_1DstR("movt", "r10", ((dec >> 16) & 0xffff));
                        if (imm < 256)
                            // code += emitInst_mem("str", "r10", "r9", pos);
                            code += space + "str\tr10, [r9, #" + to_string(pos) + "]\t@ float" + endl;
                        else
                        {
                            code += emitInst_1srcR_1DstR("add", "r9", "r9", imm / 256 * 256);
                            if (imm % 256 != 0)
                                code += emitInst_1srcR_1DstR("add", "r9", "r9", imm % 256);
                            code += space + "str\tr10, [r9]\t@ float" + endl;
                        }
                        // code += space + "movw\tr3, #" + to_string(dec & 0x0000FFFF) + endl;
                        // code += space + "movt\tr3, #" + to_string((dec >> 16) & 0xffff) + endl;
                        // if (imm < 256)
                        //     code += space + "str\tr3, [r10, #" + to_string(pos) + "]" + endl;
                        // else
                        // {
                        //     code += space + "add\tr10, r10, #" + to_string(imm / 256 * 256) + endl;
                        //     if (imm % 256 != 0)
                        //         code += space + "add\tr10, r10, #" + to_string(imm % 256) + endl;
                        //     code += space + "str\tr3, [r10]\t@ float" + endl;
                        // }
                    }
                }
                // 存入其他值
                else
                {
                    RegId srcRegId = Register[dynamic_cast<Instruction *>(value)];
                    if (value->getType()->isInt())
                    {
                        if (srcRegId == RegManager::NONE)
                        {
                            srcRegId = RegManager::R10;
                            code += emitInst_mem("ldr", "r10", "fp", AVALUE[dynamic_cast<Instruction *>(value)].stack_offset);
                        }
                        if (imm < 256)
                            // code += space + "str\tr" + value->getName() + ", [r10, #" + to_string(pos) + "]" + endl;
                            code += emitInst_mem("str", regm.toString(srcRegId), "r9", pos);
                        else
                        {
                            code += emitInst_1srcR_1DstR("add", "r9", "r9", imm / 256 * 256);
                            if (imm % 256 != 0)
                                code += emitInst_1srcR_1DstR("add", "r9", "r9", imm % 256);
                            code += emitInst_mem("str", regm.toString(srcRegId), "r9");
                            // code += space + "add\tr10, r10, #" + to_string(imm / 256 * 256) + endl;
                            // if (imm % 256 != 0)
                            //     code += space + "add\tr10, r10, #" + to_string(imm % 256) + endl;
                            // code += space + "str\tr" + value->getName() + ", [r10]" + endl;
                        }
                    }
                    else if (value->getType()->isFloat())
                    {
                        if (srcRegId == RegManager::NONE)
                        {
                            srcRegId = RegManager::S14;
                            code += emitInst_mem("vldr.32", "s14", "fp", AVALUE[dynamic_cast<Instruction *>(value)].stack_offset);
                        }
                        if (imm < 256)
                            // code += space + "vstr.32\ts" + to_string(15 - std::stoi(value->getName())) + ", [r10, #" + to_string(pos) + "]" + endl;
                            code += emitInst_mem("vstr.32", regm.toString(srcRegId), "r9", pos);
                        else
                        {
                            code += emitInst_1srcR_1DstR("add", "r9", "r9", imm / 256 * 256);
                            if (imm % 256 != 0)
                                code += emitInst_1srcR_1DstR("add", "r9", "r9", imm % 256);
                            code += emitInst_mem("vstr.32", regm.toString(srcRegId), "r9");
                            // code += space + "add\tr10, r10, #" + to_string(imm / 256 * 256) + endl;
                            // if (imm % 256 != 0)
                            //     code += space + "add\tr10, r10, #" + to_string(imm % 256) + endl;
                            // code += space + "vstr.32\ts" + to_string(15 - std::stoi(value->getName())) + ", [r10]" + endl;
                        }
                    }
                }
            }
            // 数组下标含有变量
            else
            {
                // 借用R8作为地址寄存器, 将存在R8的变量保护起来
                Instruction *protect_value = nullptr;
                if (RVALUE.find(RegManager::R8) != RVALUE.end())
                    protect_value = RVALUE[RegManager::R8];
                if (protect_value)
                {
                    int imm = -protect_reg_offset;
                    if (imm < 256)
                        code += emitInst_mem("str", "r8", "fp", protect_reg_offset);
                    else
                    {
                        code += emitInst_1srcR_1DstR("sub", "r9", "fp", imm / 256 * 256);
                        if (imm % 256 != 0)
                            code += emitInst_1srcR_1DstR("sub", "r9", "r9", imm % 256);
                        code += emitInst_mem("str", "r8", "r9");
                    }
                    AVALUE[protect_value].reg_num = RegManager::NONE;
                    AVALUE[protect_value].stack_offset = protect_reg_offset;
                }
                dim = NumDims - 1;
                flag = false;
                for (auto iter = stInst->getIndices().begin(); iter != stInst->getIndices().end(); iter++)
                {
                    if (isa<ConstantValue>(*iter))
                        dim--;
                    else if (flag == false)
                    {
                        code += emitInst_nosrcR_1DstR("mov", "r8", offset_array);
                        auto value = dynamic_cast<Instruction *>(*iter);
                        code += emitInst_nosrcR_1DstR("movw", "r10", (full_num[dim] & 0x0000ffff));
                        code += emitInst_nosrcR_1DstR("movt", "r10", ((full_num[dim] >> 16) & 0xffff));
                        dim--;
                        if (AVALUE[value].reg_num == RegManager::NONE)
                        {
                            code += emitInst_mem("ldr", "r9", "fp", AVALUE[value].stack_offset);
                            code += space + "mla\tr8, r9, r10, r8" + endl;
                        }
                        else
                            code += space + "mla\tr8, " + regm.toString(AVALUE[value].reg_num) + ", r10, r8" + endl;
                        flag = true;
                        // code += space + "movw\tr9, #" + to_string(unsigned(full_num[dim] & 0x0000FFFF)) + endl;
                        // code += space + "movt\tr9, #" + to_string(unsigned((full_num[dim--] >> 16) & 0xffff)) + endl;
                        // code += space + "mul\tr" + (*iter)->getName() + ", r" + (*iter)->getName() + ", r9" + endl;
                        // code += space + "add\tr" + (*iter)->getName() + ", r" + (*iter)->getName() + ", #" + to_string(offset_array) + endl;
                        // flag = true;
                    }
                    else
                    {
                        auto value = dynamic_cast<Instruction *>(*iter);
                        code += emitInst_nosrcR_1DstR("movw", "r10", (full_num[dim] & 0x0000FFFF));
                        code += emitInst_nosrcR_1DstR("movt", "r10", ((full_num[dim] >> 16) & 0xFFFF));
                        dim--;
                        if (AVALUE[value].reg_num == RegManager::NONE)
                        {
                            code += emitInst_mem("ldr", "r9", "fp", AVALUE[value].stack_offset);
                            code += space + "mla\tr8, r9, r10, r8" + endl;
                        }
                        else
                            code += space + "mla\tr8, " + regm.toString(AVALUE[value].reg_num) + ", r10, r8" + endl;
                        // code += space + "movw\tr9, #" + to_string(unsigned(full_num[dim] & 0x0000FFFF)) + endl;
                        // code += space + "movt\tr9, #" + to_string(unsigned((full_num[dim--] >> 16) & 0xffff)) + endl;
                        // code += space + "mla\tr" + first_var->getName() + ", r" + (*iter)->getName() + ", r9, r" + first_var->getName() + endl;
                    }
                }
                code += emitInst_1srcR_1DstR("lsl", "r8", "r8", 2);
                code += emitInst_1srcR_1DstR("movw", "r9", "#:lower16:" + pointer->getName());
                code += emitInst_1srcR_1DstR("movt", "r9", "#:upper16:" + pointer->getName());
                code += emitInst_2srcR_1dstR("add", "r8", "r9", "r8");
                // code += space + "lsl\tr" + first_var->getName() + ", r" + first_var->getName() + ", #2" + endl;
                // code += space + "add\tr" + first_var->getName() + ", r" + first_var->getName() + ", r10" + endl;
                // 存入常数
                if (isa<ConstantValue>(value))
                {
                    // int src_reg = std::stoi(first_var->getName()) + 1;
                    if (value->isInt())
                    {
                        int constant_value = dynamic_cast<ConstantValue *>(value)->getInt();
                        code += emitInst_nosrcR_1DstR("movw", "r9", (constant_value & 0x0000FFFF));
                        code += emitInst_nosrcR_1DstR("movt", "r9", ((constant_value >> 16) & 0xffff));
                        code += emitInst_mem("str", "r9", "r8");
                        // code += space + "movw\tr" + to_string(src_reg) + ", #" + to_string(constant_value & 0x0000FFFF) + endl;
                        // code += space + "movt\tr" + to_string(src_reg) + ", #" + to_string((constant_value >> 16) & 0xffff) + endl;
                        // code += space + "str\tr" + to_string(src_reg) + ", [r" + first_var->getName() + "]" + endl;
                    }
                    else if (value->isFloat())
                    {
                        float constant_value = dynamic_cast<ConstantValue *>(value)->getDouble();
                        unsigned dec;
                        std::memcpy(&dec, &constant_value, sizeof(dec));
                        code += emitInst_nosrcR_1DstR("movw", "r9", (dec & 0x0000FFFF));
                        code += emitInst_nosrcR_1DstR("movt", "r9", ((dec >> 16) & 0xffff));
                        code += space + "str\tr9, [r8]\t@ float" + endl;
                        // code += space + "movw\tr" + to_string(src_reg) + ", #" + to_string(dec & 0x0000FFFF) + endl;
                        // code += space + "movt\tr" + to_string(src_reg) + ", #" + to_string((dec >> 16) & 0xffff) + endl;
                        // code += space + "str\tr" + to_string(src_reg) + ", [r" + first_var->getName() + "]\t@ float" + endl;
                    }
                }
                // 存入其他值
                else
                {
                    RegId srcRegId = Register[dynamic_cast<Instruction *>(value)];
                    if (value->getType()->isInt())
                    {
                        if (srcRegId == RegManager::NONE)
                        {
                            srcRegId = RegManager::R9;
                            code += emitInst_mem("ldr", "r9", "fp", AVALUE[dynamic_cast<Instruction *>(value)].stack_offset);
                        }
                        code += emitInst_mem("str", regm.toString(srcRegId), "r8");
                        // code += space + "str\tr" + value->getName() + ", [r" + first_var->getName() + "]" + endl;
                    }
                    else if (value->getType()->isFloat())
                    {
                        if (srcRegId == RegManager::NONE)
                        {
                            srcRegId = RegManager::S14;
                            code += emitInst_mem("vldr.32", "s14", "fp", AVALUE[dynamic_cast<Instruction *>(value)].stack_offset);
                        }
                        code += emitInst_mem("vstr.32", regm.toString(srcRegId), "r8");
                        // code += space + "vstr.32\ts" + to_string(15 - std::stoi(value->getName())) + ", [r" + first_var->getName() + "]" + endl;
                    }
                }
                // 恢复R8对应的变量,仅当其在store指令后还活跃
                if (protect_value && protect_value->GetEnd() < stInst->GetStart())
                {
                    AVALUE[protect_value].reg_num = RegManager::R8;
                }
            }
        }
        // if pointer is an argument
        else
        {
            // 参数的维度,(起始)地址偏移量
            auto arg = dynamic_cast<Argument *>(pointer);
            int NumDims = arg->getNumDims();
            pos = paramsStOffset[arg];
            // 参数标量
            if (NumDims == 0)
            {
                // 存入常数
                if (isa<ConstantValue>(value))
                {
                    if (value->isInt())
                    {
                        int constant_value = dynamic_cast<ConstantValue *>(value)->getInt();
                        code += emitInst_nosrcR_1DstR("movw", "r9", (constant_value & 0x0000FFFF));
                        code += emitInst_nosrcR_1DstR("movt", "r9", ((constant_value >> 16) & 0xffff));
                        code += emitInst_mem("str", "r9", "fp", pos);
                        // code += space + "movw\tr3, #" + to_string(constant_value & 0x0000FFFF) + endl;
                        // code += space + "movt\tr3, #" + to_string((constant_value >> 16) & 0xffff) + endl;
                        // code += space + "str\tr3, [fp, #" + to_string(pos) + "]" + endl;
                    }
                    else if (value->isFloat())
                    {
                        float constant_value = dynamic_cast<ConstantValue *>(value)->getDouble();
                        unsigned dec;
                        std::memcpy(&dec, &constant_value, sizeof(dec));
                        code += emitInst_nosrcR_1DstR("movw", "r9", (dec & 0x0000FFFF));
                        code += emitInst_nosrcR_1DstR("movt", "r9", ((dec >> 16) & 0xffff));
                        code += space + "str\tr9, [fp, #" + to_string(pos) + "]\t@ float" + endl;
                        // code += space + "movw\tr3, #" + to_string(dec & 0x0000FFFF) + endl;
                        // code += space + "movt\tr3, #" + to_string((dec >> 16) & 0xffff) + endl;
                        // code += space + "str\tr3, [fp, #" + to_string(pos) + "]\t@ float" + endl;
                    }
                }
                // 存入其他值
                else
                {
                    RegId srcRegId = Register[dynamic_cast<Instruction *>(value)];
                    if (value->getType()->isInt())
                    {
                        if (srcRegId == RegManager::NONE)
                        {
                            srcRegId = RegManager::R9;
                            code += emitInst_mem("ldr", "r9", "fp", AVALUE[dynamic_cast<Instruction *>(value)].stack_offset);
                        }
                        code += emitInst_mem("str", regm.toString(srcRegId), "fp", pos);
                        // code += space + "str\tr" + value->getName() + ", [fp, #" + to_string(pos) + "]" + endl;
                    }
                    else if (value->getType()->isFloat())
                    {
                        if (srcRegId == RegManager::NONE)
                        {
                            srcRegId = RegManager::S14;
                            code += emitInst_mem("vldr.32", "s14", "fp", AVALUE[dynamic_cast<Instruction *>(value)].stack_offset);
                        }
                        code += emitInst_mem("vstr.32", regm.toString(srcRegId), "fp", pos);
                        // code += space + "vstr.32\ts" + to_string(15 - std::stoi(value->getName())) + ", [fp, #" + to_string(pos) + "]" + endl;
                    }
                }
            }
            // 参数数组
            else if (NumDims > 0)
            {
                // 求出元素的偏移/数组的首地址偏移
                int offset_array = 0;
                int dim = NumDims - 1;
                bool flag = false;          // 索引为常量=false/变量=true
                Value *first_var = nullptr; // 索引中出现的第一个变量
                vector<int> dim_len;
                vector<int> full_num;
                for (int i = 0; i < NumDims; i++)
                    dim_len.push_back(arg->getDim(i));
                reverse(dim_len.begin(), dim_len.end());
                full_num.push_back(1);
                for (int i = 0; i < NumIndices; i++)
                    full_num.push_back(dim_len[i] * full_num[i]);
                for (auto iter = stInst->getIndices().begin(); iter != stInst->getIndices().end(); iter++)
                {
                    int index = 0; // index 是数组下标
                    if (isa<ConstantValue>(*iter))
                        index = dynamic_cast<ConstantValue *>(*iter)->getInt();
                    else if (flag == false)
                    {
                        flag = true;
                        first_var = *iter;
                    }
                    offset_array += index * full_num[dim--];
                }
                // 数组下标均为常数
                if (flag == false)
                {
                    offset_array *= 4;
                    code += emitInst_mem("ldr", "r9", "fp", pos);
                    // 存入常数
                    if (isa<ConstantValue>(value))
                    {
                        // code += space + "ldr\tr0, [fp, #" + to_string(pos) + "]" + endl;
                        if (value->isInt())
                        {
                            int constant_value = dynamic_cast<ConstantValue *>(value)->getInt();
                            code += emitInst_nosrcR_1DstR("movw", "r10", (constant_value & 0x0000FFFF));
                            code += emitInst_nosrcR_1DstR("movt", "r10", ((constant_value >> 16) & 0xffff));
                            code += emitInst_mem("str", "r10", "r9", offset_array);
                            // code += space + "movw\tr3, #" + to_string(constant_value & 0x0000FFFF) + endl;
                            // code += space + "movt\tr3, #" + to_string((constant_value >> 16) & 0xffff) + endl;
                            // code += space + "str\tr3, [r0, #" + to_string(offset_array) + "]" + endl;
                        }
                        else if (value->isFloat())
                        {
                            float constant_value = dynamic_cast<ConstantValue *>(value)->getDouble();
                            unsigned dec;
                            std::memcpy(&dec, &constant_value, sizeof(dec));
                            code += emitInst_nosrcR_1DstR("movw", "r10", (dec & 0x0000FFFF));
                            code += emitInst_nosrcR_1DstR("movt", "r10", ((dec >> 16) & 0xffff));
                            code += space + "str\tr10, [r9, #" + to_string(offset_array) + "]\t@ float" + endl;
                            // code += space + "movw\tr3, #" + to_string(dec & 0x0000FFFF) + endl;
                            // code += space + "movt\tr3, #" + to_string((dec >> 16) & 0xffff) + endl;
                            // code += space + "str\tr3, [r0, #" + to_string(offset_array) + "]\t@ float" + endl;
                        }
                    }
                    // 存入其他值
                    else
                    {
                        // int src_reg = (1 + std::stoi(value->getName())) % 11;
                        // code += space + "ldr\tr" + to_string(src_reg) + ", [fp, #" + to_string(pos) + "]" + endl;
                        RegId srcRegId = Register[dynamic_cast<Instruction *>(value)];

                        if (value->getType()->isInt())
                        {
                            if (srcRegId == RegManager::NONE)
                            {
                                srcRegId = RegManager::R10;
                                code += emitInst_mem("ldr", "r10", "fp", AVALUE[dynamic_cast<Instruction *>(value)].stack_offset);
                            }
                            code += emitInst_mem("str", regm.toString(srcRegId), "r9", offset_array);
                            // code += space + "str\tr" + value->getName() + ", [r" + to_string(src_reg) + ", #" + to_string(offset_array) + "]" + endl;
                        }
                        else if (value->getType()->isFloat())
                        {
                            if (srcRegId == RegManager::NONE)
                            {
                                srcRegId = RegManager::S14;
                                code += emitInst_mem("vldr.32", "s14", "fp", AVALUE[dynamic_cast<Instruction *>(value)].stack_offset);
                            }
                            code += emitInst_mem("vstr.32", regm.toString(srcRegId), "r9", offset_array);
                            // code += space + "vstr.32\ts" + to_string(15 - std::stoi(value->getName())) + ", [r" + to_string(src_reg) + ", #" + to_string(offset_array) + "]" + endl;
                        }
                    }
                }
                // 数组下标含有变量
                else
                {
                    // code += "参数数组,下标含有变量\n";
                    // for (int i = 0; i < 3; i++)
                    //     code += to_string(full_num[i]) + "\n";
                    // 借用R8作为地址寄存器,将存在R8的变量保护起来
                    Instruction *protect_value = nullptr;
                    if (RVALUE.find(RegManager::R8) != RVALUE.end())
                        protect_value = RVALUE[RegManager::R8];
                    if (protect_value)
                    {
                        int imm = -protect_reg_offset;
                        if (imm < 256)
                            code += emitInst_mem("str", "r8", "fp", protect_reg_offset);
                        else
                        {
                            code += emitInst_1srcR_1DstR("sub", "r9", "fp", imm / 256 * 256);
                            if (imm % 256 != 0)
                                code += emitInst_1srcR_1DstR("sub", "r9", "r9", imm % 256);
                            code += emitInst_mem("str", "r8", "r9");
                        }
                        AVALUE[protect_value].reg_num = RegManager::NONE;
                        AVALUE[protect_value].stack_offset = protect_reg_offset;
                    }
                    dim = NumDims - 1;
                    flag = false;
                    for (auto iter = stInst->getIndices().begin(); iter != stInst->getIndices().end(); iter++)
                    {
                        if (isa<ConstantValue>(*iter))
                            dim--;
                        else if (flag == false)
                        {
                            code += emitInst_nosrcR_1DstR("mov", "r8", offset_array);
                            auto value = dynamic_cast<Instruction *>(*iter);
                            code += emitInst_nosrcR_1DstR("movw", "r10", (full_num[dim] & 0x0000ffff));
                            code += emitInst_nosrcR_1DstR("movt", "r10", ((full_num[dim] >> 16) & 0xffff));
                            dim--;
                            if (AVALUE[value].reg_num == RegManager::NONE)
                            {
                                code += emitInst_mem("ldr", "r9", "fp", AVALUE[value].stack_offset);
                                code += space + "mla\tr8, r9, r10, r8" + endl;
                            }
                            else
                                code += space + "mla\tr8, " + regm.toString(AVALUE[value].reg_num) + ", r10, r8" + endl;
                            flag = true;
                            // code += space + "movw\tr10, #" + to_string(full_num[dim] & 0x0000FFFF) + endl;
                            // code += space + "movt\tr10, #" + to_string((full_num[dim--] >> 16) & 0xffff) + endl;
                            // code += space + "mul\tr" + (*iter)->getName() + ", r" + (*iter)->getName() + ", r10" + endl;
                            // code += space + "add\tr" + (*iter)->getName() + ", r" + (*iter)->getName() + ", #" + to_string(offset_array) + endl;
                            // flag = true;
                        }
                        else
                        {
                            auto value = dynamic_cast<Instruction *>(*iter);
                            code += emitInst_nosrcR_1DstR("movw", "r10", (full_num[dim] & 0x0000FFFF));
                            code += emitInst_nosrcR_1DstR("movt", "r10", ((full_num[dim] >> 16) & 0xFFFF));
                            dim--;
                            if (AVALUE[value].reg_num == RegManager::NONE)
                            {
                                code += emitInst_mem("ldr", "r9", "fp", AVALUE[value].stack_offset);
                                code += space + "mla\tr8, r9, r10, r8" + endl;
                            }
                            else
                                code += space + "mla\tr8, " + regm.toString(AVALUE[value].reg_num) + ", r10, r8" + endl;
                            // code += space + "movw\tr10, #" + to_string(full_num[dim] && 0x0000FFFF) + endl;
                            // code += space + "movt\tr10, #" + to_string((full_num[dim--] >> 16) & 0xffff) + endl;
                            // code += space + "mla\tr" + first_var->getName() + ", r" + (*iter)->getName() + ", r10, r" + first_var->getName() + endl;
                        }
                    }
                    // code += space + "lsl\tr" + first_var->getName() + ", r" + first_var->getName() + ", #2" + endl;
                    code += emitInst_1srcR_1DstR("lsl", "r8", "r8", 2);
                    // int src_reg = (1 + std::stoi(first_var->getName())) % 11;
                    code += emitInst_mem("ldr", "r9", "fp", pos);
                    code += emitInst_2srcR_1dstR("add", "r8", "r9", "r8");
                    // 存入常数
                    if (isa<ConstantValue>(value))
                    {
                        // code += space + "ldr\tr" + to_string(src_reg) + ", [fp, #" + to_string(paramsStOffset[arg]) + "]" + endl;
                        // code += space + "add\tr" + first_var->getName() + ", r" + first_var->getName() + ", r" + to_string(src_reg) + endl;
                        if (value->isInt())
                        {
                            int constant_value = dynamic_cast<ConstantValue *>(value)->getInt();
                            code += emitInst_nosrcR_1DstR("movw", "r9", (constant_value & 0x0000FFFF));
                            code += emitInst_nosrcR_1DstR("movt", "r9", ((constant_value >> 16) & 0xffff));
                            code += emitInst_mem("str", "r9", "r8");
                            // code += space + "movw\tr" + to_string(src_reg) + ", #" + to_string(constant_value & 0x0000FFFF) + endl;
                            // code += space + "movt\tr" + to_string(src_reg) + ", #" + to_string((constant_value >> 16) & 0xffff) + endl;
                            // code += space + "str\tr" + to_string(src_reg) + ", [r" + first_var->getName() + "]" + endl;
                        }
                        else if (value->isFloat())
                        {
                            float constant_value = dynamic_cast<ConstantValue *>(value)->getDouble();
                            unsigned dec;
                            std::memcpy(&dec, &constant_value, sizeof(dec));
                            code += emitInst_nosrcR_1DstR("movw", "r9", (dec & 0x0000FFFF));
                            code += emitInst_nosrcR_1DstR("movt", "r9", ((dec >> 16) & 0xffff));
                            code += space + "str\tr9, [r8]\t@ float" + endl;
                            // code += space + "movw\tr" + to_string(src_reg) + ", #" + to_string(dec & 0x0000FFFF) + endl;
                            // code += space + "movt\tr" + to_string(src_reg) + ", #" + to_string((dec >> 16) & 0xffff) + endl;
                            // code += space + "str\tr" + to_string(src_reg) + ", [r" + first_var->getName() + "]\t@ float" + endl;
                        }
                    }
                    // 存入其他值
                    else
                    {
                        // code += space + "ldr\tr" + to_string(src_reg) + ", [fp, #" + to_string(paramsStOffset[arg]) + "]" + endl;
                        // code += space + "add\tr" + first_var->getName() + ", r" + first_var->getName() + ", r" + to_string(src_reg) + endl;
                        RegId srcRegId = Register[dynamic_cast<Instruction *>(value)];
                        if (value->getType()->isInt())
                        {
                            if (srcRegId == RegManager::NONE)
                            {
                                srcRegId = RegManager::R9;
                                code += emitInst_mem("ldr", "r9", "fp", AVALUE[dynamic_cast<Instruction *>(value)].stack_offset);
                            }
                            code += emitInst_mem("str", regm.toString(srcRegId), "r8");
                            // code += space + "str\tr" + value->getName() + ", [r" + first_var->getName() + "]" + endl;
                        }
                        else if (value->getType()->isFloat())
                        {
                            if (srcRegId == RegManager::NONE)
                            {
                                srcRegId = RegManager::S14;
                                code += emitInst_mem("vldr.32", "s14", "fp", AVALUE[dynamic_cast<Instruction *>(value)].stack_offset);
                            }
                            code += emitInst_mem("vstr.32", regm.toString(srcRegId), "r8");
                            // code += space + "vstr.32\ts" + to_string(15 - std::stoi(value->getName())) + ", [r" + first_var->getName() + "]" + endl;
                        }
                    }
                    // 恢复R8对应的变量,仅当其在store指令后还活跃
                    if (protect_value && protect_value->GetEnd() < stInst->GetStart())
                    {
                        AVALUE[protect_value].reg_num = RegManager::R8;
                    }
                }
            }
        }
        // 更新RVALUE,AVALUE
        if (!isa<ConstantValue>(value) && value->GetEnd() <= stInst->GetStart())
        {
            RVALUE[Register[dynamic_cast<Instruction *>(value)]] = nullptr;
            auto iter = AVALUE.find(dynamic_cast<Instruction *>(value));
            AVALUE.erase(iter);
        }
        for (auto indice : stInst->getIndices())
        {
            if (!isa<ConstantValue>(indice) && indice->GetEnd() <= stInst->GetStart())
            {
                auto value = dynamic_cast<Instruction *>(indice);
                RVALUE[Register[value]] = nullptr;
                auto iter = AVALUE.find(value);
                if (iter != AVALUE.end())
                    AVALUE.erase(iter);
            }
        }
        return code;
    }
    pair<RegId, string> CodeGen::loadInst_gen(LoadInst *ldInst, RegId dstRegId)
    {
        string code;
        // dst register
        dstRegId = Register[ldInst];
        // variable to be loaded
        auto pointer = ldInst->getPointer();
        auto type = pointer->getType()->as<PointerType>()->getBaseType();
        if (dstRegId == RegManager::NONE)
        {
            if (type->isInt())
                dstRegId = RegManager::R9;
            else if (type->isFloat())
                dstRegId = RegManager::S14;
        }
        int pos;                                  // variable's position in the stack
        int NumIndices = ldInst->getNumIndices(); // numindices denotes dimensions of pointer

        // if pointer is a localvariable
        if (localVarStOffset.find(dynamic_cast<Instruction *>(pointer)) != localVarStOffset.end())
        {
            // code += "%" + ldInst->getName() + endl;
            // 局部变量的维度,(起始)地址偏移量
            auto local_variable = dynamic_cast<AllocaInst *>(pointer);
            int NumDims = local_variable->getNumDims();
            pos = localVarStOffset[dynamic_cast<Instruction *>(pointer)];
            // 局部标量
            if (NumDims == 0)
            {
                int imm = -pos;
                if (type->isInt())
                {
                    if (imm < 256)
                        code += emitInst_mem("ldr", regm.toString(dstRegId), "fp", pos);
                    else
                    {
                        code += emitInst_1srcR_1DstR("sub", regm.toString(dstRegId), "fp", imm / 256 * 256);
                        if (imm % 256 != 0)
                            code += emitInst_1srcR_1DstR("sub", regm.toString(dstRegId), regm.toString(dstRegId), imm % 256);
                        code += emitInst_mem("ldr", regm.toString(dstRegId), regm.toString(dstRegId));
                    }
                }
                else if (type->isFloat())
                {
                    if (imm < 256)
                        // code += space + "vldr.32\ts" + to_string(15 - reg_num) + ", [fp, #" + to_string(pos) + "]" + endl;
                        code += emitInst_mem("vldr.32", regm.toString(dstRegId), "fp", pos);
                    else
                    {
                        code += emitInst_1srcR_1DstR("sub", "r9", "fp", imm / 256 * 256);
                        if (imm % 256 != 0)
                            code += emitInst_1srcR_1DstR("sub", "r9", "r9", imm % 256);
                        code += emitInst_mem("vldr.32", regm.toString(dstRegId), "r9");
                        // code += space + "sub\tr" + to_string(reg_num) + ", fp ,#" + to_string(imm / 256 * 256) + endl;
                        // if (imm % 256 != 0)
                        //     code += space + "sub\tr" + to_string(reg_num) + ", r" + to_string(reg_num) + ", #" + to_string(imm % 256) + endl;
                        // code += space + "vldr.32\ts" + to_string(15 - reg_num) + ", [r" + to_string(reg_num) + "]" + endl;
                    }
                }
            }
            // 局部数组
            else if (NumDims > 0)
            {
                // 求出元素的偏移/数组的首地址偏移
                int offset_array = 0;
                int dim = NumDims - 1;
                bool flag = false;          // 索引为常量=false/变量=true
                Value *first_var = nullptr; // 索引中出现的第一个变量
                vector<int> dim_len;
                vector<int> full_num;
                for (int i = 0; i < NumDims; i++)
                    dim_len.push_back(dynamic_cast<ConstantValue *>(local_variable->getDim(i))->getInt());
                reverse(dim_len.begin(), dim_len.end());
                full_num.push_back(1);
                for (int i = 0; i < NumIndices; i++)
                    full_num.push_back(dim_len[i] * full_num[i]);
                for (auto iter = ldInst->getIndices().begin(); iter != ldInst->getIndices().end(); iter++)
                {
                    int index = 0; // index 是数组下标
                    if (isa<ConstantValue>(*iter))
                        index = dynamic_cast<ConstantValue *>(*iter)->getInt();
                    else if (flag == false)
                    {
                        flag = true;
                        first_var = *iter;
                    }
                    offset_array += index * full_num[dim--];
                }
                if (NumIndices < NumDims && dstRegId == RegManager::S14)
                    dstRegId = RegManager::R9;
                // 数组下标均为常数
                if (flag == false)
                {
                    pos += offset_array * 4;
                    int imm = -pos;
                    // load出数组的一个元素
                    if (NumIndices == NumDims)
                    {
                        if (type->isInt())
                        {
                            if (imm < 256)
                                // code += space + "ldr\tr" + to_string(reg_num) + ", [fp, #" + to_string(pos) + "]" + endl;
                                code += emitInst_mem("ldr", regm.toString(dstRegId), "fp", pos);
                            else
                            {
                                code += emitInst_1srcR_1DstR("sub", regm.toString(dstRegId), "fp", imm / 256 * 256);
                                if (imm % 256 != 0)
                                    code += emitInst_1srcR_1DstR("sub", regm.toString(dstRegId), regm.toString(dstRegId), imm % 256);
                                code += emitInst_mem("ldr", regm.toString(dstRegId), regm.toString(dstRegId));
                                // code += space + "sub\tr" + to_string(reg_num) + ", fp ,#" + to_string(imm / 256 * 256) + endl;
                                // if (imm % 256 != 0)
                                //     code += space + "sub\tr" + to_string(reg_num) + ", r" + to_string(reg_num) + ", #" + to_string(imm % 256) + endl;
                                // code += space + "ldr\tr" + to_string(reg_num) + ", [r" + to_string(reg_num) + "]" + endl;
                            }
                        }
                        else if (type->isFloat())
                        {
                            if (imm < 256)
                                code += emitInst_mem("vldr.32", regm.toString(dstRegId), "fp", pos);
                            else
                            {
                                code += emitInst_1srcR_1DstR("sub", "r9", "fp", imm / 256 * 256);
                                if (imm % 256 != 0)
                                    code += emitInst_1srcR_1DstR("sub", "r9", "r9", imm % 256);
                                code += emitInst_mem("vldr.32", regm.toString(dstRegId), "r9");
                            }
                            // if (imm < 256)
                            //     code += space + "vldr.32\ts" + to_string(15 - reg_num) + ", [fp, #" + to_string(pos) + "]" + endl;
                            // else
                            // {
                            //     code += space + "sub\tr" + to_string(reg_num) + ", fp ,#" + to_string(imm / 256 * 256) + endl;
                            //     if (imm % 256 != 0)
                            //         code += space + "sub\tr" + to_string(reg_num) + ", r" + to_string(reg_num) + ", #" + to_string(imm % 256) + endl;
                            //     code += space + "vldr.32\ts" + to_string(15 - reg_num) + ", [r" + to_string(reg_num) + "]" + endl;
                            // }
                        }
                    }
                    // load出一个(子)数组,求出其首地址
                    else if (NumIndices < NumDims)
                    {
                        int imm = -pos;
                        if (imm < 256)
                            code += emitInst_1srcR_1DstR("sub", regm.toString(dstRegId), "fp", imm);
                        else
                        {
                            code += emitInst_1srcR_1DstR("sub", regm.toString(dstRegId), "fp", imm / 256 * 256);
                            if (imm % 256 != 0)
                                code += emitInst_1srcR_1DstR("sub", regm.toString(dstRegId), regm.toString(dstRegId), imm % 256);
                        }
                        // if (imm < 256)
                        //     code += space + "sub\tr" + to_string(reg_num) + ", fp, #" + to_string(-pos) + endl;
                        // else
                        // {
                        //     code += space + "sub\tr" + to_string(reg_num) + ", fp, #" + to_string(imm / 256 * 256) + endl;
                        //     if (imm % 256 != 0)
                        //         code += space + "sub\tr" + to_string(reg_num) + ", r" + to_string(reg_num) + ", #" + to_string(imm % 256) + endl;
                        // }
                    }
                }
                // 数组下标含有变量
                else
                {
                    // 借用R8作为地址寄存器,将存在R8的变量保护起来
                    Instruction *protect_value = nullptr;
                    if (RVALUE.find(RegManager::R8) != RVALUE.end())
                        protect_value = RVALUE[RegManager::R8];
                    if (protect_value)
                    {
                        int imm = -protect_reg_offset;
                        if (imm < 256)
                            code += emitInst_mem("str", "r8", "fp", protect_reg_offset);
                        else
                        {
                            code += emitInst_1srcR_1DstR("sub", "r9", "fp", imm / 256 * 256);
                            if (imm % 256 != 0)
                                code += emitInst_1srcR_1DstR("sub", "r9", "r9", imm % 256);
                            code += emitInst_mem("str", "r8", "r9");
                        }
                        AVALUE[protect_value].reg_num = RegManager::NONE;
                        AVALUE[protect_value].stack_offset = protect_reg_offset;
                    }
                    dim = NumDims - 1;
                    flag = false;
                    for (auto iter = ldInst->getIndices().begin(); iter != ldInst->getIndices().end(); iter++)
                    {
                        if (isa<ConstantValue>(*iter))
                            dim--;
                        else if (flag == false)
                        {
                            code += emitInst_nosrcR_1DstR("mov", "r8", offset_array);
                            auto value = dynamic_cast<Instruction *>(*iter);
                            code += emitInst_nosrcR_1DstR("movw", "r10", (full_num[dim] & 0x0000FFFF));
                            code += emitInst_nosrcR_1DstR("movt", "r10", ((full_num[dim] >> 16) & 0xFFFF));
                            dim--;
                            if (AVALUE[value].reg_num == RegManager::NONE)
                            {
                                code += emitInst_mem("ldr", "r9", "fp", AVALUE[value].stack_offset);
                                code += space + "mla\tr8, r9, r10, r8" + endl;
                            }
                            else
                                code += space + "mla\tr8, " + regm.toString(AVALUE[value].reg_num) + ", r10, r8" + endl;
                            flag = true;
                            // code += space + "movw\tr10, #" + to_string(full_num[dim] & 0x0000FFFF) + endl;
                            // code += space + "movt\tr10, #" + to_string((full_num[dim--] >> 16) & 0xffff) + endl;
                            // code += space + "mul\tr" + (*iter)->getName() + ", r" + (*iter)->getName() + ", r10" + endl;
                            // int imm = offset_array;
                            // if (imm < 256)
                            //     code += space + "add\tr" + (*iter)->getName() + ", r" + (*iter)->getName() + ", #" + to_string(offset_array) + endl;
                            // else
                            // {
                            //     code += space + "add\tr" + (*iter)->getName() + ", r" + (*iter)->getName() + ", #" + to_string(imm / 256 * 256) + endl;
                            //     if (imm % 256 != 0)
                            //         code += space + "add\tr" + (*iter)->getName() + ", r" + (*iter)->getName() + ", #" + to_string(imm % 256) + endl;
                            // }
                            // flag = true;
                        }
                        else
                        {
                            auto value = dynamic_cast<Instruction *>(*iter);
                            code += emitInst_nosrcR_1DstR("movw", "r10", (full_num[dim] & 0x0000FFFF));
                            code += emitInst_nosrcR_1DstR("movt", "r10", ((full_num[dim] >> 16) & 0xFFFF));
                            dim--;
                            if (AVALUE[value].reg_num == RegManager::NONE)
                            {
                                code += emitInst_mem("ldr", "r9", "fp", AVALUE[value].stack_offset);
                                code += space + "mla\tr8, r9, r10, r8" + endl;
                            }
                            else
                                code += space + "mla\tr8, " + regm.toString(AVALUE[value].reg_num) + ", r10, r8" + endl;
                            //     code += space + "movw\tr10, #" + to_string(full_num[dim] & 0x0000FFFF) + endl;
                            //     code += space + "movt\tr10, #" + to_string((full_num[dim--] >> 16) & 0xffff) + endl;
                            //     code += space + "mla\tr" + first_var->getName() + ", r" + (*iter)->getName() + ", r10, r" + first_var->getName() + endl;
                        }
                    }
                    code += emitInst_1srcR_1DstR("lsl", "r8", "r8", 2);
                    int imm = -pos;
                    // code += to_string(imm) + endl;
                    if (imm < 256)
                        code += emitInst_1srcR_1DstR("sub", "r8", "r8", imm);
                    else
                    {
                        code += emitInst_1srcR_1DstR("sub", "r8", "r8", imm / 256 * 256);
                        if (imm % 256 != 0)
                            code += emitInst_1srcR_1DstR("sub", "r8", "r8", imm % 256);
                    }
                    code += emitInst_2srcR_1dstR("add", "r8", "r8", "fp");
                    // code += space + "lsl\tr" + first_var->getName() + ", r" + first_var->getName() + ", #2" + endl;
                    // int imm = -pos;
                    // if (imm < 256)
                    //     code += space + "sub\tr" + first_var->getName() + ", r" + first_var->getName() + ", #" + to_string(-pos) + endl;
                    // else
                    // {
                    //     code += space + "sub\tr" + first_var->getName() + ", r" + first_var->getName() + ", #" + to_string(imm / 256 * 256) + endl;
                    //     if (imm % 256 != 0)
                    //         code += space + "sub\tr" + first_var->getName() + ", r" + first_var->getName() + ", #" + to_string(imm % 256) + endl;
                    // }
                    // code += space + "add\tr" + first_var->getName() + ", r" + first_var->getName() + ", fp" + endl;

                    // load出数组的一个元素
                    if (NumIndices == NumDims)
                    {
                        if (type->isInt())
                            code += emitInst_mem("ldr", regm.toString(dstRegId), "r8");
                        // code += space + "ldr\tr" + to_string(reg_num) + ", [r" + first_var->getName() + "]" + endl;
                        else if (type->isFloat())
                            code += emitInst_mem("vldr.32", regm.toString(dstRegId), "r8");
                        // code += space + "vldr.32\ts" + to_string(15 - reg_num) + ", [r" + first_var->getName() + "]" + endl;
                    }
                    // load出一个(子)数组,求出其首地址
                    else if (NumIndices < NumDims)
                    {
                        code += emitInst_1srcR_1DstR("mov", regm.toString(dstRegId), "r8");
                        // code += space + "mov\tr" + to_string(reg_num) + ", r" + first_var->getName() + endl;
                    }
                    // 恢复R8对应的变量,仅当其在load指令后还活跃
                    if (protect_value && protect_value->GetEnd() > ldInst->GetStart())
                    {
                        AVALUE[protect_value].reg_num = RegManager::R8;
                    }
                }
            }
        }
        // if pointer is a globalvalue
        else if (globalval.find(dynamic_cast<GlobalValue *>(pointer)) != globalval.end())
        {
            // 全局变量的维度,(起始)地址偏移量
            auto global_value = dynamic_cast<GlobalValue *>(pointer);
            int NumDims = global_value->getNumDims();
            pos = 0;
            // 获取全局数组(首)地址
            // code += space + "movw\tr" + to_string(reg_num) + ", #:lower16:" + pointer->getName() + endl;
            // code += space + "movt\tr" + to_string(reg_num) + ", #:upper16:" + pointer->getName() + endl;

            // 全局标量
            if (NumDims == 0)
            {
                code += emitInst_1srcR_1DstR("movw", "r9", "#:lower16:" + pointer->getName());
                code += emitInst_1srcR_1DstR("movt", "r9", "#:upper16:" + pointer->getName());
                if (type->isInt())
                    // code += space + "ldr\tr" + to_string(reg_num) + ", [r" + to_string(reg_num) + "]" + endl;
                    code += emitInst_mem("ldr", regm.toString(dstRegId), "r9");
                else if (type->isFloat())
                    // code += space + "vldr.32\ts" + to_string(15 - reg_num) + ", [r" + to_string(reg_num) + "]" + endl;
                    code += emitInst_mem("vldr.32", regm.toString(dstRegId), "r9");
            }
            // 全局数组
            else if (NumDims > 0)
            {
                // 求出元素的偏移/数组的首地址偏移
                int offset_array = 0;
                int dim = NumDims - 1;
                bool flag = false;          // 索引为常量=false/变量=true
                Value *first_var = nullptr; // 索引中出现的第一个变量
                vector<int> dim_len;
                vector<int> full_num;
                for (int i = 0; i < NumDims; i++)
                    dim_len.push_back(dynamic_cast<ConstantValue *>(global_value->getDim(i))->getInt());
                reverse(dim_len.begin(), dim_len.end());
                full_num.push_back(1);
                for (int i = 0; i < NumIndices; i++)
                    full_num.push_back(dim_len[i] * full_num[i]);
                for (auto iter = ldInst->getIndices().begin(); iter != ldInst->getIndices().end(); iter++)
                {
                    int index = 0; // index 是数组下标
                    if (isa<ConstantValue>(*iter))
                        index = dynamic_cast<ConstantValue *>(*iter)->getInt();
                    else if (flag == false)
                    {
                        flag = true;
                        first_var = *iter;
                    }
                    offset_array += index * full_num[dim--];
                }
                if (NumIndices < NumDims && dstRegId == RegManager::S14)
                    dstRegId = RegManager::R9;
                // 数组下标均为常数
                if (flag == false)
                {
                    pos += offset_array * 4;
                    int imm = pos;
                    code += emitInst_1srcR_1DstR("movw", "r9", "#:lower16:" + pointer->getName());
                    code += emitInst_1srcR_1DstR("movt", "r9", "#:upper16:" + pointer->getName());
                    // load出数组的一个元素
                    if (NumIndices == NumDims)
                    {
                        if (type->isInt())
                        {
                            if (imm < 256)
                                // code += space + "ldr\tr" + to_string(reg_num) + ", [r" + to_string(reg_num) + ", #" + to_string(pos) + "]" + endl;
                                code += emitInst_mem("ldr", regm.toString(dstRegId), "r9", pos);
                            else if (imm < 65536)
                            {
                                code += emitInst_1srcR_1DstR("add", "r9", "r9", imm / 256 * 256);
                                if (imm % 256 != 0)
                                    code += emitInst_1srcR_1DstR("add", "r9", "r9", imm % 256);
                                code += emitInst_mem("ldr", regm.toString(dstRegId), "r9");
                                // code += "add\tr" + to_string(reg_num) + ", r" + to_string(reg_num) + ", #" + to_string(imm / 256 * 256) + endl;
                                // if (imm % 256 != 0)
                                //     code += "add\tr" + to_string(reg_num) + ", r" + to_string(reg_num) + ", #" + to_string(imm % 256) + endl;
                                // code += "ldr\tr" + to_string(reg_num) + ", [r" + to_string(reg_num) + "]" + endl;
                            }
                            else
                            {
                                code += emitInst_1srcR_1DstR("add", "r9", "r9", imm / 4096 * 4096);
                                if (imm % 4096 != 0)
                                {
                                    imm = imm % 4096;
                                    code += emitInst_1srcR_1DstR("add", "r9", "r9", imm / 256 * 256);
                                    if (imm % 256 != 0)
                                        code += emitInst_1srcR_1DstR("add", "r9", "r9", imm % 256);
                                    code += emitInst_mem("ldr", regm.toString(dstRegId), "r9");
                                }
                                // code += "add\tr" + to_string(reg_num) + ", r" + to_string(reg_num) + ", #" + to_string(imm / 4096 * 4096) + endl;
                                // if (imm % 4096 != 0)
                                // {
                                //     imm = imm % 4096;
                                //     code += "add\tr" + to_string(reg_num) + ", r" + to_string(reg_num) + ", #" + to_string(imm / 256 * 256) + endl;
                                //     if (imm % 256 != 0)
                                //         code += "add\tr" + to_string(reg_num) + ", r" + to_string(reg_num) + ", #" + to_string(imm % 256) + endl;
                                // }
                                // code += "ldr\tr" + to_string(reg_num) + ", [r" + to_string(reg_num) + "]" + endl;
                            }
                        }
                        else if (type->isFloat())
                        {
                            if (imm < 256)
                                // code += space + "vldr.32\ts" + to_string(15 - reg_num) + ", [r" + to_string(reg_num) + ", #" + to_string(pos) + "]" + endl;
                                code += emitInst_mem("vldr.32", regm.toString(dstRegId), "r9", pos);
                            else
                            {
                                code += emitInst_1srcR_1DstR("add", "r9", "r9", imm / 256 * 256);
                                if (imm % 256 != 0)
                                    code += emitInst_1srcR_1DstR("add", "r9", "r9", imm % 256);
                                code += emitInst_mem("vldr.32", regm.toString(dstRegId), "r9");
                                // code += "add\tr" + to_string(reg_num) + ", r" + to_string(reg_num) + ", #" + to_string(imm / 256 * 256) + endl;
                                // if (imm % 256 != 0)
                                //     code += "add\tr" + to_string(reg_num) + ", r" + to_string(reg_num) + ", #" + to_string(imm % 256) + endl;
                                // code += "vldr.32\ts" + to_string(15 - reg_num) + ", [r" + to_string(reg_num) + "]" + endl;
                            }
                        }
                    }
                    // load出一个(子)数组,求出其首地址
                    else if (NumIndices < NumDims)
                    {
                        int imm = pos;
                        if (imm < 256)
                            // code += space + "add\tr" + to_string(reg_num) + ", r" + to_string(reg_num) + ", #" + to_string(pos) + endl;
                            code += space + "add\t" + regm.toString(dstRegId) + ", r9, #" + to_string(pos) + endl;
                        else
                        {
                            code += emitInst_1srcR_1DstR("add", "r9", "r9", imm / 256 * 256);
                            if (imm % 256 != 0)
                                code += emitInst_1srcR_1DstR("add", regm.toString(dstRegId), "r9", imm % 256);
                            // code += space + "add\tr" + to_string(reg_num) + ", r" + to_string(reg_num) + ", #" + to_string(imm / 256 * 256) + endl;
                            // if (imm % 256 != 0)
                            //     code += space + "add\tr" + to_string(reg_num) + ", r" + to_string(reg_num) + ", #" + to_string(imm % 256) + endl;
                        }
                    }
                }
                // 数组下标含有变量
                else
                {
                    // 借用R8作为地址寄存器,将存在R8的变量保护起来
                    Instruction *protect_value = nullptr;
                    if (RVALUE.find(RegManager::R8) != RVALUE.end())
                        protect_value = RVALUE[RegManager::R8];
                    if (protect_value)
                    {
                        int imm = -protect_reg_offset;
                        if (imm < 256)
                            code += emitInst_mem("str", "r8", "fp", protect_reg_offset);
                        else
                        {
                            code += emitInst_1srcR_1DstR("sub", "r9", "fp", imm / 256 * 256);
                            if (imm % 256 != 0)
                                code += emitInst_1srcR_1DstR("sub", "r9", "r9", imm % 256);
                            code += emitInst_mem("str", "r8", "r9");
                        }
                        AVALUE[protect_value].reg_num = RegManager::NONE;
                        AVALUE[protect_value].stack_offset = protect_reg_offset;
                    }
                    dim = NumDims - 1;
                    flag = false;
                    for (auto iter = ldInst->getIndices().begin(); iter != ldInst->getIndices().end(); iter++)
                    {
                        if (isa<ConstantValue>(*iter))
                            dim--;
                        else if (flag == false)
                        {
                            code += emitInst_nosrcR_1DstR("mov", "r8", offset_array);
                            auto value = dynamic_cast<Instruction *>(*iter);
                            code += emitInst_nosrcR_1DstR("movw", "r10", (full_num[dim] & 0x0000FFFF));
                            code += emitInst_nosrcR_1DstR("movt", "r10", ((full_num[dim] >> 16) & 0xFFFF));
                            dim--;
                            if (AVALUE[value].reg_num == RegManager::NONE)
                            {
                                code += emitInst_mem("ldr", "r9", "fp", AVALUE[value].stack_offset);
                                code += space + "mla\tr8, r9, r10, r8" + endl;
                            }
                            else
                                code += space + "mla\tr8, " + regm.toString(AVALUE[value].reg_num) + ", r10, r8" + endl;
                            flag = true;
                            // code += space + "movw\tr9, #" + to_string(full_num[dim] & 0x0000FFFF) + endl;
                            // code += space + "movt\tr9, #" + to_string((full_num[dim--] >> 16) & 0xffff) + endl;
                            // code += space + "mul\tr" + (*iter)->getName() + ", r" + (*iter)->getName() + ", r9" + endl;
                            // code += space + "add\tr" + (*iter)->getName() + ", r" + (*iter)->getName() + ", #" + to_string(offset_array) + endl;
                            // flag = true;
                        }
                        else
                        {
                            auto value = dynamic_cast<Instruction *>(*iter);
                            code += emitInst_nosrcR_1DstR("movw", "r10", (full_num[dim] & 0x0000FFFF));
                            code += emitInst_nosrcR_1DstR("movt", "r10", ((full_num[dim] >> 16) & 0xFFFF));
                            dim--;
                            if (AVALUE[value].reg_num == RegManager::NONE)
                            {
                                code += emitInst_mem("ldr", "r9", "fp", AVALUE[value].stack_offset);
                                code += space + "mla\tr8, r9, r10, r8" + endl;
                            }
                            else
                                code += space + "mla\tr8, " + regm.toString(AVALUE[value].reg_num) + ", r10, r8" + endl;
                            // code += space + "movw\tr9, #" + to_string(full_num[dim] & 0x0000FFFF) + endl;
                            // code += space + "movt\tr9, #" + to_string((full_num[dim--] >> 16) & 0xffff) + endl;
                            // code += space + "mla\tr" + first_var->getName() + ", r" + (*iter)->getName() + ", r9, r" + first_var->getName() + endl;
                        }
                    }
                    code += emitInst_1srcR_1DstR("lsl", "r8", "r8", 2);
                    code += emitInst_1srcR_1DstR("movw", "r9", "#:lower16:" + pointer->getName());
                    code += emitInst_1srcR_1DstR("movt", "r9", "#:upper16:" + pointer->getName());
                    code += emitInst_2srcR_1dstR("add", "r8", "r8", "r9");
                    // code += space + "lsl\tr" + first_var->getName() + ", r" + first_var->getName() + ", #2" + endl;
                    // code += space + "add\tr" + first_var->getName() + ", r" + first_var->getName() + ", r" + to_string(reg_num) + endl;
                    // load出数组的一个元素
                    if (NumIndices == NumDims)
                    {
                        if (type->isInt())
                            // code += space + "ldr\tr" + to_string(reg_num) + ", [r" + first_var->getName() + "]" + endl;
                            code += emitInst_mem("ldr", regm.toString(dstRegId), "r8");
                        else if (type->isFloat())
                            // code += space + "vldr.32\ts" + to_string(15 - reg_num) + ", [r" + first_var->getName() + "]" + endl;
                            code += emitInst_mem("vldr.32", regm.toString(dstRegId), "r8");
                    }
                    // load出一个(子)数组,求出其首地址
                    else if (NumIndices < NumDims)
                    {
                        // code += space + "mov\tr" + to_string(reg_num) + ", r" + first_var->getName() + endl;
                        code += emitInst_1srcR_1DstR("mov", regm.toString(dstRegId), "r8");
                    }
                    // 恢复R8对应的变量,仅当其在load指令后还活跃
                    if (protect_value && protect_value->GetEnd() > ldInst->GetStart())
                    {
                        AVALUE[protect_value].reg_num = RegManager::R8;
                    }
                }
            }
        }
        // if pointer is an argument
        else
        {
            // 参数的维度,(起始)地址偏移量
            auto arg = dynamic_cast<Argument *>(pointer);
            int NumDims = arg->getNumDims();
            pos = paramsStOffset[arg];
            // 参数标量
            if (NumDims == 0)
            {
                int imm = -pos;
                if (type->isInt())
                {
                    if (imm < 256)
                        // code += space + "ldr\tr" + to_string(reg_num) + ", [fp, #" + to_string(pos) + "]" + endl;
                        code += emitInst_mem("ldr", regm.toString(dstRegId), "fp", pos);
                    else if(imm < 65536)
                    {
                        code += emitInst_1srcR_1DstR("sub", regm.toString(dstRegId), "fp", imm / 256 * 256);
                        if (imm % 256 != 0)
                            code += emitInst_1srcR_1DstR("sub", regm.toString(dstRegId), regm.toString(dstRegId), imm % 256);
                        code += emitInst_mem("ldr", regm.toString(dstRegId), regm.toString(dstRegId));
                        // code += space + "sub\tr" + to_string(reg_num) + ", fp, #" + to_string(imm / 256 * 256) + endl;
                        // if (imm % 256 != 0)
                        //     code += space + "sub\tr" + to_string(reg_num) + ", r" + to_string(reg_num) + ", #" + to_string(imm % 256) + endl;
                        // code += space + "ldr\tr" + to_string(reg_num) + ", [r" + to_string(reg_num) + "]" + endl;
                    }
                    else{
                        code += emitInst_1srcR_1DstR("sub", regm.toString(dstRegId), "fp", imm / 4096 * 4096);
                        if (imm % 4096 != 0){
                            imm = imm % 4096;
                            code += emitInst_1srcR_1DstR("sub", regm.toString(dstRegId), regm.toString(dstRegId), imm / 256 * 256);
                            if(imm % 256 != 0)
                            code += emitInst_1srcR_1DstR("sub", regm.toString(dstRegId), regm.toString(dstRegId), imm % 256);
                            }
                        code += emitInst_mem("ldr", regm.toString(dstRegId), regm.toString(dstRegId));
                    }
                }
                else if (type->isFloat())
                {
                    if (imm < 256)
                        // code += space + "vldr.32\ts" + to_string(15 - reg_num) + ", [fp, #" + to_string(pos) + "]" + endl;
                        code += emitInst_mem("vldr.32", regm.toString(dstRegId), "fp", pos);
                    else
                    {
                        code += emitInst_1srcR_1DstR("sub", "r9", "fp", imm / 256 * 256);
                        if (imm % 256 != 0)
                            code += emitInst_1srcR_1DstR("sub", "r9", "r9", imm % 256);
                        code += emitInst_mem("vldr.32", regm.toString(dstRegId), "r9");
                    }
                }
            }
            // 参数数组
            else if (NumDims > 0)
            {
                // 求出元素的偏移/数组的首地址偏移
                int offset_array = 0;
                int dim = NumDims - 1;
                bool flag = false;          // 索引为常量=false/变量=true
                Value *first_var = nullptr; // 索引中出现的第一个变量
                vector<int> dim_len;
                vector<int> full_num;
                for (int i = 0; i < NumDims; i++)
                    dim_len.push_back(arg->getDim(i));
                reverse(dim_len.begin(), dim_len.end());
                full_num.push_back(1);
                for (int i = 0; i < NumIndices; i++)
                    full_num.push_back(dim_len[i] * full_num[i]);
                for (auto iter = ldInst->getIndices().begin(); iter != ldInst->getIndices().end(); iter++)
                {
                    int index = 0; // index 是数组下标
                    if (isa<ConstantValue>(*iter))
                        index = dynamic_cast<ConstantValue *>(*iter)->getInt();
                    else if (flag == false)
                    {
                        flag = true;
                        first_var = *iter;
                    }
                    offset_array += index * full_num[dim--];
                }
                if (NumIndices < NumDims && dstRegId == RegManager::S14)
                    dstRegId = RegManager::R9;
                // 数组下标均为常数
                if (flag == false)
                {
                    offset_array *= 4;
                    // load出数组的一个元素
                    if (NumIndices == NumDims)
                    {
                        // code += space + "ldr\tr" + to_string(reg_num) + ", [fp, #" + to_string(pos) + "]" + endl;
                        code += emitInst_mem("ldr", "r9", "fp", pos);
                        int imm = offset_array;
                        if (type->isInt())
                        // code += space + "ldr\tr" + to_string(reg_num) + ", [r" + to_string(reg_num) + ", #" + to_string(offset_array) + "]" + endl;
                        {
                            if (imm < 256)
                                code += emitInst_mem("ldr", regm.toString(dstRegId), "r9", offset_array);
                            else if (imm < 65536)
                            {
                                code += emitInst_1srcR_1DstR("add", "r9", "r9", imm / 256 * 256);
                                if (imm % 256 != 0)
                                    code += emitInst_1srcR_1DstR("add", "r9", "r9", imm % 256);
                                code += emitInst_mem("ldr", regm.toString(dstRegId), "r9");
                            }
                            else
                            {
                                code += emitInst_1srcR_1DstR("add", "r9", "r9", imm / 4096 * 4096);
                                if (imm % 4096 != 0)
                                {
                                    imm = imm % 4096;
                                    code += emitInst_1srcR_1DstR("add", "r9", "r9", imm / 256 * 256);
                                    if (imm % 256 != 0)
                                        code += emitInst_1srcR_1DstR("add", "r9", "r9", imm % 256);
                                }
                                code += emitInst_mem("ldr", regm.toString(dstRegId), "r9");
                            }
                        }
                        else if (type->isFloat())
                            // code += space + "vldr.32\ts" + to_string(15 - reg_num) + ", [r" + to_string(reg_num) + ", #" + to_string(offset_array) + "]" + endl;
                            code += emitInst_mem("vldr.32", regm.toString(dstRegId), "r9", offset_array);
                    }
                    // load出一个(子)数组,求出其首地址
                    else if (NumIndices < NumDims)
                    {
                        int imm = -pos;
                        if (imm < 256)
                            // code += space + "ldr\tr" + to_string(reg_num) + ", [fp, #" + to_string(pos) + "]" + endl;
                            code += emitInst_mem("ldr", regm.toString(dstRegId), "fp", pos);
                        else
                        {
                            code += emitInst_1srcR_1DstR("sub", regm.toString(dstRegId), "fp", imm / 256 * 256);
                            if (imm % 256 != 0)
                                code += emitInst_1srcR_1DstR("sub", regm.toString(dstRegId), regm.toString(dstRegId), imm % 256);
                            code += emitInst_mem("ldr", regm.toString(dstRegId), regm.toString(dstRegId));
                            // code += space + "sub\tr" + to_string(reg_num) + ", fp, #" + to_string(imm / 256 * 256) + endl;
                            // if (imm % 256 != 0)
                            //     code += space + "sub\tr" + to_string(reg_num) + ", r" + to_string(reg_num) + ", #" + to_string(imm % 256) + endl;
                            // code += space + "ldr\tr" + to_string(reg_num) + ", [r" + to_string(reg_num) + "]" + endl;
                        }
                        imm = offset_array;
                        if (imm == 0)
                            ;
                        else if (imm < 256)
                            // code += space + "add\tr" + to_string(reg_num) + ", r" + to_string(reg_num) + ", #" + to_string(imm) + endl;
                            code += emitInst_1srcR_1DstR("add", regm.toString(dstRegId), regm.toString(dstRegId), imm);
                        else
                        {
                            code += emitInst_1srcR_1DstR("add", regm.toString(dstRegId), regm.toString(dstRegId), imm / 256 * 256);
                            if (imm % 256)
                                code += emitInst_1srcR_1DstR("add", regm.toString(dstRegId), regm.toString(dstRegId), imm % 256);
                            // code += space + "add\tr" + to_string(reg_num) + ", r" + to_string(reg_num) + ", #" + to_string(imm / 256 * 256) + endl;
                            // if (imm % 256 != 0)
                            //     code += space + "add\tr" + to_string(reg_num) + ", r" + to_string(reg_num) + ", #" + to_string(imm % 256) + endl;
                        }
                    }
                }
                // 数组下标含有变量
                else
                {
                    // 借用R8作为地址寄存器, 将存在R8的变量保护起来
                    Instruction *protect_value = nullptr;
                    if (RVALUE.find(RegManager::R8) != RVALUE.end())
                        protect_value = RVALUE[RegManager::R8];
                    if (protect_value)
                    {
                        int imm = -protect_reg_offset;
                        if (imm < 256)
                            code += emitInst_mem("str", "r8", "fp", protect_reg_offset);
                        else
                        {
                            code += emitInst_1srcR_1DstR("sub", "r9", "fp", imm / 256 * 256);
                            if (imm % 256 != 0)
                                code += emitInst_1srcR_1DstR("sub", "r9", "r9", imm % 256);
                            code += emitInst_mem("str", "r8", "r9");
                        }
                        AVALUE[protect_value].reg_num = RegManager::NONE;
                        AVALUE[protect_value].stack_offset = protect_reg_offset;
                    }
                    dim = NumDims - 1;
                    flag = false;
                    for (auto iter = ldInst->getIndices().begin(); iter != ldInst->getIndices().end(); iter++)
                    {
                        if (isa<ConstantValue>(*iter))
                            dim--;
                        else if (flag == false)
                        {
                            code += emitInst_nosrcR_1DstR("mov", "r8", offset_array);
                            auto value = dynamic_cast<Instruction *>(*iter);
                            code += emitInst_nosrcR_1DstR("movw", "r10", (full_num[dim] & 0x0000FFFF));
                            code += emitInst_nosrcR_1DstR("movt", "r10", ((full_num[dim] >> 16) & 0xFFFF));
                            dim--;
                            if (AVALUE[value].reg_num == RegManager::NONE)
                            {
                                code += emitInst_mem("ldr", "r9", "fp", AVALUE[value].stack_offset);
                                code += space + "mla\tr8, r9, r10, r8" + endl;
                            }
                            else
                                code += space + "mla\tr8, " + regm.toString(AVALUE[value].reg_num) + ", r10, r8" + endl;
                            flag = true;
                            // code += space + "movw\tr10, #" + to_string(full_num[dim] & 0x0000FFFF) + endl;
                            // code += space + "movt\tr10, #" + to_string((full_num[dim--] >> 16) & 0xffff) + endl;
                            // code += space + "mul\tr" + (*iter)->getName() + ", r" + (*iter)->getName() + ", r10" + endl;
                            // code += space + "add\tr" + (*iter)->getName() + ", r" + (*iter)->getName() + ", #" + to_string(offset_array) + endl;
                            // flag = true;
                        }
                        else
                        {
                            auto value = dynamic_cast<Instruction *>(*iter);
                            code += emitInst_nosrcR_1DstR("movw", "r10", (full_num[dim] & 0x0000FFFF));
                            code += emitInst_nosrcR_1DstR("movt", "r10", ((full_num[dim] >> 16) & 0xFFFF));
                            dim--;
                            if (AVALUE[value].reg_num == RegManager::NONE)
                            {
                                code += emitInst_mem("ldr", "r9", "fp", AVALUE[value].stack_offset);
                                code += space + "mla\tr8, r9, r10, r8" + endl;
                            }
                            else
                                code += space + "mla\tr8, " + regm.toString(AVALUE[value].reg_num) + ", r10, r8" + endl;
                            // code += space + "mov\tr10, #" + to_string(full_num[dim--]) + endl;
                            // code += space + "mla\tr" + first_var->getName() + ", r" + (*iter)->getName() + ", r10, r" + first_var->getName() + endl;
                        }
                    }
                    // code += space + "lsl\tr" + first_var->getName() + ", r" + first_var->getName() + ", #2" + endl;
                    code += emitInst_1srcR_1DstR("lsl", "r8", "r8", 2);
                    // 参数数组的首地址
                    int imm = -pos;
                    if (imm < 256)
                        // code += space + "ldr\tr" + to_string(reg_num) + ", [fp, #" + to_string(pos) + "]" + endl;
                        code += emitInst_mem("ldr", "r9", "fp", pos);
                    else
                    {
                        code += emitInst_1srcR_1DstR("sub", "r9", "fp", imm / 256 * 256);
                        if (imm % 256)
                            code += emitInst_1srcR_1DstR("sub", "r9", "r9", imm % 256);
                        code += emitInst_mem("ldr", "r9", "r9");
                        // code += space + "sub\tr" + to_string(reg_num) + ", fp, #" + to_string(imm / 256 * 256) + endl;
                        // if (imm % 256 != 0)
                        //     code += space + "sub\tr" + to_string(reg_num) + ", r" + to_string(reg_num) + ", #" + to_string(imm % 256) + endl;
                        // code += space + "ldr\tr" + to_string(reg_num) + ", [r" + to_string(reg_num) + "]" + endl;
                    }
                    // 获得地址
                    // code += space + "add\tr" + first_var->getName() + ", r" + to_string(reg_num) + ", r" + first_var->getName() + endl;
                    code += emitInst_2srcR_1dstR("add", "r8", "r9", "r8");
                    // load出数组的一个元素
                    if (NumIndices == NumDims)
                    {
                        if (type->isInt())
                            // code += space + "ldr\tr" + to_string(reg_num) + ", [r" + first_var->getName() + "]" + endl;
                            code += emitInst_mem("ldr", regm.toString(dstRegId), "r8");
                        else if (type->isFloat())
                            // code += space + "vldr.32\ts" + to_string(15 - reg_num) + ", [r" + first_var->getName() + "]" + endl;
                            code += emitInst_mem("vldr.32", regm.toString(dstRegId), "r8");
                    }
                    // load出一个(子)数组,求出其首地址
                    else if (NumIndices < NumDims)
                    {
                        code += emitInst_1srcR_1DstR("mov", regm.toString(dstRegId), "r8");
                    }
                    // 恢复R8对应的变量,仅当其在load指令后还活跃
                    if (protect_value && protect_value->GetEnd() > ldInst->GetStart())
                    {
                        AVALUE[protect_value].reg_num = RegManager::R8;
                    }
                }
            }
        }
        // 更新RVALUE,AVALUE
        for (auto indice : ldInst->getIndices())
        {
            if (!isa<ConstantValue>(indice) && indice->GetEnd() <= ldInst->GetStart())
            {
                auto value = dynamic_cast<Instruction *>(indice);
                RVALUE[Register[value]] = nullptr;
                auto iter = AVALUE.find(value);
                if (iter != AVALUE.end())
                    AVALUE.erase(iter);
            }
        }
        if (dstRegId == RegManager::R9 || dstRegId == RegManager::S14)
        {
            if (dstRegId == RegManager::R9)
                code += emitInst_mem("str", "r9", "fp", ldInst->GetLocation());
            else if (dstRegId == RegManager::S14)
                code += emitInst_mem("vstr.32", "s14", "fp", ldInst->GetLocation());
            dstRegId = RegManager::NONE;
            AVALUE[ldInst].stack_offset = ldInst->GetLocation();
        }
        AVALUE[ldInst].reg_num = dstRegId;
        RVALUE[dstRegId] = ldInst;
        return {dstRegId, code};
    }

    string CodeGen::returnInst_gen(ReturnInst *retInst)
    {
        string code;
        string func_exit = retInst->getFunction()->getName() + "_exit";
        auto retval = retInst->getReturnValue();
        if (retval == nullptr)
            ;
        else if (isa<ConstantValue>(retval))
        {
            if (retval->getType()->isInt())
            {
                code += space + "movw\tr0, #" + to_string(dynamic_cast<ConstantValue *>(retval)->getInt() & 0x0000FFFF) + endl;
                code += space + "movt\tr0, #" + to_string((dynamic_cast<ConstantValue *>(retval)->getInt() >> 16) & 0xffff) + endl;
            }
            else if (retval->getType()->isFloat())
            {
                unsigned num1;
                float val1;
                val1 = dynamic_cast<ConstantValue *>(retval)->getDouble();
                memcpy(&num1, &val1, sizeof(val1));
                code += space + "movw\tr9, #" + to_string(num1 & 0x0000FFFF) + endl;
                code += space + "movt\tr9, #" + to_string((num1 >> 16) & 0xffff) + endl;
                code += space + "vmov\ts0, r9" + endl;
                // code += space + "vmov.f32\ts0, s15" + endl;
            }
        }
        // else if (isa<CallInst>(retval))
        //     ;
        else if (retval)
        {
            auto value = dynamic_cast<Instruction *>(retval);
            if (value->getType()->isInt())
            {
                if (Register[value] == RegManager::NONE)
                    code += emitInst_mem("ldr", "r0", "fp", AVALUE[value].stack_offset);
                else
                    code += emitInst_1srcR_1DstR("mov", "r0", regm.toString(AVALUE[value].reg_num));
            }
            else if (value->getType()->isFloat())
            {
                if (Register[value] == RegManager::NONE)
                    code += emitInst_mem("vldr.32", "s0", "fp", AVALUE[value].stack_offset);
                else
                    code += emitInst_1srcR_1DstR("vmov", "s0", regm.toString(AVALUE[value].reg_num));
            }
            // if (retval->getKind() == Instruction::kFtoI)
            //     code += space + "vmov\tr0,s" + to_string(15 - stoi(retval->getName())) + endl;
            // else if (retval->getType()->isInt())
            //     code += space + "mov\tr0, r" + retval->getName() + endl;
            // else if (retval->getType()->isFloat())
            //     code += space + "vmov.f32\ts0, s" + to_string(15 - stoi(retval->getName())) + endl;
        }
        code += space + "b\t" + func_exit + endl;
        return code;
    }
    string CodeGen::uncondBrInst_gen(UncondBrInst *ubInst)
    {
        string code;
        auto goal = ubInst->getBlock();
        code += space + "b\t" + goal->getName() + endl;
        return code;
    }
    string CodeGen::condBrInst_gen(CondBrInst *cbInst)
    {
        string code;
        auto cond = cbInst->getCondition();
        auto then_block = cbInst->getThenBlock();
        auto else_block = cbInst->getElseBlock();
        if (isa<BinaryInst>(cond))
        {
            BinaryInst *bInst = dynamic_cast<BinaryInst *>(cond);
            auto lhs = bInst->getLhs();
            auto rhs = bInst->getRhs();
            if (isa<ConstantValue>(lhs) && isa<ConstantValue>(rhs))
            {
                float lvalue = 0, rvalue = 0;
                if (lhs->getType()->isInt())
                    lvalue = dynamic_cast<ConstantValue *>(lhs)->getInt();
                else if (lhs->getType()->isFloat())
                    lvalue = dynamic_cast<ConstantValue *>(lhs)->getDouble();
                if (rhs->getType()->isInt())
                    rvalue = dynamic_cast<ConstantValue *>(rhs)->getInt();
                else if (rhs->getType()->isFloat())
                    rvalue = dynamic_cast<ConstantValue *>(rhs)->getDouble();
                if (bInst->getKind() == Instruction::kICmpEQ ||
                    bInst->getKind() == Instruction::kFCmpEQ)
                    if (lvalue == rvalue)
                        code += space + "b\t" + then_block->getName() + endl;
                    else
                        code += space + "b\t" + else_block->getName() + endl;
                else if (bInst->getKind() == Instruction::kICmpGE ||
                         bInst->getKind() == Instruction::kFCmpGE)
                    if (lvalue >= rvalue)
                        code += space + "b\t" + then_block->getName() + endl;
                    else
                        code += space + "b\t" + else_block->getName() + endl;
                else if (bInst->getKind() == Instruction::kICmpGT ||
                         bInst->getKind() == Instruction::kFCmpGT)
                    if (lvalue > rvalue)
                        code += space + "b\t" + then_block->getName() + endl;
                    else
                        code += space + "b\t" + else_block->getName() + endl;
                else if (bInst->getKind() == Instruction::kICmpLE ||
                         bInst->getKind() == Instruction::kFCmpLE)
                    if (lvalue <= rvalue)
                        code += space + "b\t" + then_block->getName() + endl;
                    else
                        code += space + "b\t" + else_block->getName() + endl;
                else if (bInst->getKind() == Instruction::kICmpLT ||
                         bInst->getKind() == Instruction::kFCmpLT)
                    if (lvalue < rvalue)
                        code += space + "b\t" + then_block->getName() + endl;
                    else
                        code += space + "b\t" + else_block->getName() + endl;
                else if (bInst->getKind() == Instruction::kICmpNE ||
                         bInst->getKind() == Instruction::kFCmpNE)
                    if (lvalue != rvalue)
                        code += space + "b\t" + then_block->getName() + endl;
                    else
                        code += space + "b\t" + else_block->getName() + endl;
                else
                {
                    code += space + "cmp\tr" + bInst->getName() + ", #0" + endl;
                    code += space + "bne\t" + then_block->getName() + endl;
                    code += space + "b\t" + else_block->getName() + endl;
                }
            }
            else if (bInst->getKind() == Instruction::kICmpEQ || bInst->getKind() == Instruction::kFCmpEQ)
            {
                // code += binaryInst_gen(bInst, RegManager::RANY).second;
                code += space + "beq\t" + then_block->getName() + endl;
                code += space + "b\t" + else_block->getName() + endl;
            }
            else if (bInst->getKind() == Instruction::kICmpGE || bInst->getKind() == Instruction::kFCmpGE)
            {
                // code += binaryInst_gen(bInst, RegManager::RANY).second;
                code += space + "bge\t" + then_block->getName() + endl;
                code += space + "b\t" + else_block->getName() + endl;
            }
            else if (bInst->getKind() == Instruction::kICmpGT || bInst->getKind() == Instruction::kFCmpGT)
            {
                // code += binaryInst_gen(bInst, RegManager::RANY).second;
                code += space + "bgt\t" + then_block->getName() + endl;
                code += space + "b\t" + else_block->getName() + endl;
            }
            else if (bInst->getKind() == Instruction::kICmpLE || bInst->getKind() == Instruction::kFCmpLE)
            {
                // code += binaryInst_gen(bInst, RegManager::RANY).second;
                code += space + "ble\t" + then_block->getName() + endl;
                code += space + "b\t" + else_block->getName() + endl;
            }
            else if (bInst->getKind() == Instruction::kICmpLT || bInst->getKind() == Instruction::kFCmpLT)
            {
                // code += binaryInst_gen(bInst, RegManager::RANY).second;
                code += space + "blt\t" + then_block->getName() + endl;
                code += space + "b\t" + else_block->getName() + endl;
            }
            else if (bInst->getKind() == Instruction::kICmpNE || bInst->getKind() == Instruction::kFCmpNE)
            {
                // code += binaryInst_gen(bInst, RegManager::RANY).second;
                code += space + "bne\t" + then_block->getName() + endl;
                code += space + "b\t" + else_block->getName() + endl;
            }
            else
            {
                RegId cond_reg = Register[bInst];
                if (cond_reg == RegManager::NONE)
                {
                    code += emitInst_mem("ldr", "r9", "fp", AVALUE[bInst].stack_offset);
                    cond_reg = RegManager::R9;
                }
                // code += space + "cmp\tr" + bInst->getName() + ", #0" + endl;
                code += emitInst_nosrcR_1DstR("cmp", regm.toString(cond_reg), 0);
                code += space + "bne\t" + then_block->getName() + endl;
                code += space + "b\t" + else_block->getName() + endl;
            }
            if (bInst->GetEnd() <= cbInst->GetStart())
            {
                auto iter = AVALUE.find(bInst);
                AVALUE.erase(iter);
            }
        }
        else if (isa<ConstantValue>(cond))
        {
            if (cond->isInt() && dynamic_cast<ConstantValue *>(cond)->getInt())
                code += space + "b\t" + then_block->getName() + endl;
            else if (cond->isFloat() && dynamic_cast<ConstantValue *>(cond)->getDouble())
                code += space + "b\t" + then_block->getName() + endl;
            else
                code += space + "b\t" + else_block->getName() + endl;
        }
        else
        {
            RegId cond_reg = Register[dynamic_cast<Instruction *>(cond)];
            if (cond_reg == RegManager::NONE)
            {
                code += emitInst_mem("ldr", "r9", "fp", AVALUE[dynamic_cast<Instruction *>(cond)].stack_offset);
                cond_reg = RegManager::R9;
            }
            // code += space + "cmp\tr" + bInst->getName() + ", #0" + endl;
            code += emitInst_nosrcR_1DstR("cmp", regm.toString(cond_reg), 0);
            code += space + "bne\t" + then_block->getName() + endl;
            code += space + "b\t" + else_block->getName() + endl;
            if (cond->GetEnd() <= cbInst->GetStart())
            {
                auto iter = AVALUE.find(dynamic_cast<Instruction *>(cond));
                AVALUE.erase(iter);
            }
        }
        return code;
    }
    pair<RegId, string>
    CodeGen::callInst_gen(CallInst *callInst, RegId dstRegId)
    {
        string code;
        auto callee_fuc = callInst->getCallee(); // 被调用函数

        auto args = callInst->getArguments(); // 所有参数
        if (!callInst->getType()->isVoid())
            dstRegId = Register[callInst]; // 函数返回值(若有)的寄存器
        unsigned int_num = 0;              // 记录当前是第几个int类型的参数
        unsigned float_num = 0;            // 记录当前是第几个float类型的参数
        int para_offset = 0;               // 记录通过栈传递的参数的栈偏移
        int protect_offset = 0;            // 记录当前保护变量相对第一个保护变量的栈偏移
        int protect_cnt = 0;               // 记录函数需要保护的变量个数
        set<Instruction *> protect_value;  // 记录所有需要释放的被保护变量

        // 1.保护R0-R3,S0-S3并传递参数
        for (auto arg : args)
        {
            bool LoadArray = false;
            // float数组仍然用R寄存器传参
            if (isa<LoadInst>(arg))
            {
                auto ld_inst = dynamic_cast<LoadInst *>(arg);
                auto pointer = ld_inst->getPointer();
                int numdims = 0;
                if (isa<GlobalValue>(pointer))
                    numdims = dynamic_cast<GlobalValue *>(pointer)->getNumDims();
                else if (isa<AllocaInst>(pointer))
                    numdims = dynamic_cast<AllocaInst *>(pointer)->getNumDims();
                else if (isa<Argument>(pointer))
                    numdims = dynamic_cast<Argument *>(pointer)->getNumDims();
                if (ld_inst->getNumIndices() < numdims)
                    LoadArray = true;
                else
                    LoadArray = false;
            }
            if (arg->getType()->isInt() || LoadArray)
            {
                if (int_num >= 4)
                {
                    // 若参数为常数
                    if (isa<ConstantValue>(arg))
                    {
                        int imm = dynamic_cast<ConstantValue *>(arg)->getInt();
                        code += emitInst_nosrcR_1DstR("mov", "r9", imm);
                        code += emitInst_mem("str", "r9", "sp", para_offset);
                    }
                    // 若参数为变量
                    else
                    {
                        auto value = dynamic_cast<Instruction *>(arg);
                        // 若变量溢出
                        // 若变量未溢出,但被保护在栈上
                        if (AVALUE[value].reg_num == RegManager::NONE)
                        {
                            code += emitInst_mem("ldr", "r9", "fp", AVALUE[value].stack_offset);
                            code += emitInst_mem("str", "r9", "sp", para_offset);
                        }
                        // 若变量在寄存器中
                        else
                        {
                            code += emitInst_mem("str", regm.toString(AVALUE[value].reg_num), "sp", para_offset);
                        }
                    }
                    para_offset += 4;
                    continue;
                }
                // 保护原来存在R0-R3的变量
                RegId reg_num = regm.ConvertToR(int_num);
                if (RVALUE.find(reg_num) != RVALUE.end() && RVALUE[reg_num])
                {
                    protect_cnt++;
                    auto protect_value = RVALUE[reg_num];
                    int imm = -(protect_reg_offset + protect_offset);
                    if (imm < 256)
                        code += emitInst_mem("str", regm.toString(reg_num), "fp", protect_reg_offset + protect_offset);
                    else
                    {
                        code += emitInst_1srcR_1DstR("sub", "r9", "fp", imm / 256 * 256);
                        if (imm % 256 != 0)
                            code += emitInst_1srcR_1DstR("sub", "r9", "r9", imm % 256);
                        code += emitInst_mem("str", regm.toString(reg_num), "r9");
                    }
                    AVALUE[protect_value].reg_num = RegManager::NONE;
                    AVALUE[protect_value].stack_offset = protect_reg_offset + protect_offset;
                    protect_offset -= 4;
                }
                // 将参数传到R0-R3
                // 若参数为常数
                if (isa<ConstantValue>(arg))
                {
                    int imm = dynamic_cast<ConstantValue *>(arg)->getInt();
                    if (imm < 256)
                        code += emitInst_nosrcR_1DstR("mov", regm.toString(reg_num), imm);
                    else
                    {
                        code += emitInst_nosrcR_1DstR("movw", regm.toString(reg_num), (imm & 0xFFFF));
                        code += emitInst_nosrcR_1DstR("movt", regm.toString(reg_num), ((imm >> 16) & 0xFFFF));
                    }
                    // src_name = "#" + to_string(dynamic_cast<ConstantValue *>(arg)->getInt());
                }
                // 若参数为变量
                else
                {
                    auto value = dynamic_cast<Instruction *>(arg);
                    // 若变量溢出&若变量未溢出但在保护区
                    if (AVALUE[value].reg_num == RegManager::NONE)
                    {
                        int imm = -AVALUE[value].stack_offset;
                        if (imm < 256)
                            code += emitInst_mem("ldr", regm.toString(reg_num), "fp", AVALUE[value].stack_offset);
                        else
                        {
                            code += emitInst_1srcR_1DstR("sub", "r9", "fp", imm / 256 * 256);
                            if (imm % 256 != 0)
                                code += emitInst_1srcR_1DstR("sub", "r9", "r9", imm % 256);
                            code += emitInst_mem("ldr", regm.toString(reg_num), "r9");
                        }
                    }
                    // 若变量在寄存器中
                    else
                    {
                        code += emitInst_1srcR_1DstR("mov", regm.toString(reg_num), regm.toString(AVALUE[value].reg_num));
                    }
                }
                int_num++;
            }
            else if (arg->getType()->isFloat())
            {
                if (float_num >= 5)
                {
                    // 若参数为常数
                    if (isa<ConstantValue>(arg))
                    {
                        float imm = dynamic_cast<ConstantValue *>(arg)->getDouble();
                        unsigned dec;
                        memcpy(&dec, &imm, sizeof(float));
                        code += emitInst_nosrcR_1DstR("movw", "r9", (dec & 0xffff));
                        code += emitInst_nosrcR_1DstR("movt", "r9", ((dec >> 16) & 0xffff));
                        code += emitInst_mem("str", "r9", "sp", para_offset);
                        // code += space + "movw\tr4, #" + to_string(mid & 0xffff) + endl;
                        // code += space + "movt\tr4, #" + to_string((mid >> 16) & 0xffff) + endl;
                        // code += space + "vmov\ts4, " + "r4" + endl;
                        // code += space + "vstr\ts4, [sp, #" + to_string(para_offset) + "]" + endl;
                    }
                    // 若参数为变量
                    else
                    {
                        auto value = dynamic_cast<Instruction *>(arg);
                        // 若变量溢出
                        // 若变量未溢出,但被保护在栈上
                        if (AVALUE[value].reg_num == RegManager::NONE)
                        {
                            code += emitInst_mem("vldr.32", "s14", "fp", AVALUE[value].stack_offset);
                            code += emitInst_mem("vstr.32", "s14", "sp", para_offset);
                        }
                        // 若变量在寄存器中
                        else
                        {
                            code += emitInst_mem("vstr.32", regm.toString(AVALUE[value].reg_num), "sp", para_offset);
                        }
                    }
                    para_offset += 4;
                    continue;
                }
                // 保护原来存在S0-S3的变量
                RegId reg_num = regm.ConvertToS(float_num);
                if (RVALUE.find(reg_num) != RVALUE.end() && RVALUE[reg_num])
                {
                    protect_cnt++;
                    auto protect_value = RVALUE[reg_num];
                    int imm = -(protect_reg_offset + protect_offset);
                    if (imm < 256)
                        code += emitInst_mem("vstr.32", regm.toString(reg_num), "fp", protect_reg_offset + protect_offset);
                    else
                    {
                        code += emitInst_1srcR_1DstR("sub", "r9", "fp", imm / 256 * 256);
                        if (imm % 256 != 0)
                            code += emitInst_1srcR_1DstR("sub", "r9", "r9", imm % 256);
                        code += emitInst_mem("vstr.32", regm.toString(reg_num), "r9");
                    }
                    AVALUE[protect_value].reg_num = RegManager::NONE;
                    AVALUE[protect_value].stack_offset = protect_reg_offset + protect_offset;
                    protect_offset -= 4;
                }
                // 将参数传到S0-S3
                // 若参数为常数
                if (isa<ConstantValue>(arg))
                {
                    float imm = dynamic_cast<ConstantValue *>(arg)->getDouble();
                    unsigned int dec;
                    memcpy(&dec, &imm, sizeof(float));
                    code += emitInst_nosrcR_1DstR("movw", "r9", (dec & 0xffff));
                    code += emitInst_nosrcR_1DstR("movt", "r9", ((dec >> 16) & 0xffff));
                    code += emitInst_1srcR_1DstR("vmov", regm.toString(reg_num), "r9");
                    // code += space + "movw\tr4, #" + to_string(dec & 0xffff) + endl;
                    // code += space + "movt\tr4, #" + to_string((dec >> 16) & 0xffff) + endl;
                    // code += space + "vmov\ts" + to_string(arg_num - 1) + ", r4" + endl;
                }
                // 若参数为变量
                else
                {
                    auto value = dynamic_cast<Instruction *>(arg);
                    // 若变量未溢出,但被保护在栈上
                    if (AVALUE[value].reg_num == RegManager::NONE)
                    {
                        int imm = -AVALUE[value].stack_offset;
                        if (imm < 256)
                            code += emitInst_mem("vldr.32", regm.toString(reg_num), "fp", AVALUE[value].stack_offset);
                        else
                        {
                            code += emitInst_1srcR_1DstR("sub", "r9", "fp", imm / 256 * 256);
                            if (imm % 256 != 0)
                                code += emitInst_1srcR_1DstR("sub", "r9", "r9", imm % 256);
                            code += emitInst_mem("vldr.32", regm.toString(reg_num), "r9");
                        }
                    }
                    // 若变量在寄存器中
                    else
                    {
                        code += emitInst_1srcR_1DstR("vmov.f32", regm.toString(reg_num), regm.toString(AVALUE[value].reg_num));
                    }
                    // int protect_offset = dynamic_cast<Instruction *>(arg)->ProtectOffset();
                    // if (protect_offset >= 0)
                    //     code += space + "vldr.32\ts" + to_string(15 - std::stoi(arg->getName())) + ", [fp, #" + to_string(protect_reg_offset - protect_offset) + "]" + endl;
                    // code += space + "vmov.f32\ts" + to_string(arg_num - 1) + ", " + "s" + to_string(15 - std::stoi(arg->getName())) + endl;
                }
                float_num++;
            }
        }
        // 2.保护其他活跃变量
        auto bbs = curFunc->getBasicBlocks();
        bool PreValue = true; // 判断活跃区间的起点是否在call指令之前
        for (auto iter = bbs.begin(); iter != bbs.end(); ++iter)
        {
            auto bb = iter->get();
            for (auto &instr : bb->getInstructions())
            {
                if (instr->GetStart() >= callInst->GetStart())
                {
                    PreValue = false;
                    break;
                }
                auto instrType = instr->getKind();
                // 只考虑能产生新变量的指令,这样就相当于从小到大遍历所有变量
                if ((instrType == Value::Kind::kCall && !instr->getType()->isVoid()) || instrType == Value::Kind::kLoad || instr->isBinary() || instr->isUnary())
                {
                    // code += "%" + instr->getName() + "end:" + to_string(instr->GetEnd()) + endl;
                    // code += "%" + callInst->getName() + "start:" + to_string(callInst->GetStart()) + endl;
                    // 若区间在call指令后不再活跃,不用保护
                    if (instr->GetEnd() <= callInst->GetStart())
                        continue;
                    // 若区间在栈上,不用保护
                    if (Register[instr.get()] == RegManager::NONE)
                        continue;
                    protect_value.insert(instr.get());
                    // 若区间已被保护,不用再生成store指令
                    if (AVALUE[instr.get()].reg_num == RegManager::NONE)
                        continue;
                    // 若区间在call指令后仍然活跃,需要保护
                    int imm = -(protect_reg_offset + protect_offset);
                    if (instr->getType()->isInt())
                    {
                        if (imm < 256)
                            code += emitInst_mem("str", regm.toString(AVALUE[instr.get()].reg_num), "fp", protect_reg_offset + protect_offset);
                        else
                        {
                            code += emitInst_1srcR_1DstR("sub", "r9", "fp", imm / 256 * 256);
                            if (imm % 256 != 0)
                                code += emitInst_1srcR_1DstR("sub", "r9", "r9", imm % 256);
                            code += emitInst_mem("str", regm.toString(AVALUE[instr.get()].reg_num), "r9");
                        }
                    }
                    else
                    {
                        if (imm < 256)
                            code += emitInst_mem("vstr.32", regm.toString(AVALUE[instr.get()].reg_num), "fp", protect_reg_offset + protect_offset);
                        else
                        {
                            code += emitInst_1srcR_1DstR("sub", "r9", "fp", imm / 256 * 256);
                            if (imm % 256 != 0)
                                code += emitInst_1srcR_1DstR("sub", "r9", "r9", imm % 256);
                            code += emitInst_mem("vstr.32", regm.toString(AVALUE[instr.get()].reg_num), "r9");
                        }
                    }
                    AVALUE[instr.get()].reg_num = RegManager::NONE;
                    AVALUE[instr.get()].stack_offset = protect_reg_offset + protect_offset;
                    protect_offset -= 4;
                    protect_cnt++;
                }
            }
            if (!PreValue)
                break;
        }
        max_protect = max_protect > protect_cnt ? max_protect : protect_cnt;
        // 3.执行函数调用
        if (callee_fuc->getName() == "starttime")
            code += space + "bl\t" + "_sysy_starttime" + endl;
        else if (callee_fuc->getName() == "stoptime")
            code += space + "bl\t" + "_sysy_stoptime" + endl;
        else
            code += space + "bl\t" + callee_fuc->getName() + endl;
        RVALUE.clear();
        // 4.传递返回值(若有)
        if (callInst->getType()->isInt())
        {
            if (dstRegId == RegManager::NONE)
                code += emitInst_mem("str", "r0", "fp", callInst->GetLocation());
            else
                code += emitInst_1srcR_1DstR("mov", regm.toString(dstRegId), "r0");
        }
        else if (callInst->getType()->isFloat())
        {
            if (dstRegId == RegManager::NONE)
                code += emitInst_mem("vstr.32", "s0", "fp", callInst->GetLocation());
            else
                code += emitInst_1srcR_1DstR("vmov", regm.toString(dstRegId), "s0");
        }
        // if (callInst->getType()->isInt())
        // {
        //     int protect_offset = callInst->ProtectOffset();
        //     int pass_offset = callInst->PassOffset();
        //     // 如果函数返回值需要被保护
        //     if (protect_offset >= 0)
        //         code += space + "str\tr0, [fp, #" + to_string(protect_reg_offset - protect_offset) + "]" + endl;
        //     // 如果函数返回值需要立即传参(即为第4个之后的参数)
        //     else if (pass_offset >= 0)
        //         code += space + "str\tr0, [sp, #" + to_string(pass_offset) + "]" + endl;
        //     // 否则将返回值mov到相应寄存器
        //     else
        //         code += space + "mov\tr" + callInst->getName() + ", r0" + endl;
        // }
        // else if (callInst->getType()->isFloat())
        // {
        //     int protect_offset = callInst->ProtectOffset();
        //     int pass_offset = callInst->PassOffset();
        //     // 如果函数返回值需要被保护
        //     if (protect_offset >= 0)
        //         code += space + "vstr\ts0, [fp, #" + to_string(protect_reg_offset - protect_offset) + "]" + endl;
        //     // 如果函数返回值需要立即传参(即为第4个之后的参数)
        //     else if (pass_offset >= 0)
        //         code += space + "vstr\ts0, [sp, #" + to_string(pass_offset) + "]" + endl;
        //     // 否则将返回值mov到相应寄存器
        //     else
        //         code += space + "vmov\ts" + to_string(15 - stoi(callInst->getName())) + ", s0" + endl;
        // }

        // 5.将保护区释放回变量的寄存器
        for (auto value : protect_value)
        {
            // code += regm.toString(Register[value]) + " " + to_string(AVALUE[value].stack_offset) + endl;
            int imm = -AVALUE[value].stack_offset;
            if (value->getType()->isInt())
            {
                if (imm < 256)
                    code += emitInst_mem("ldr", regm.toString(Register[value]), "fp", AVALUE[value].stack_offset);
                else
                {
                    code += emitInst_1srcR_1DstR("sub", "r9", "fp", imm / 256 * 256);
                    if (imm % 256 != 0)
                        code += emitInst_1srcR_1DstR("sub", "r9", "r9", imm % 256);
                    code += emitInst_mem("ldr", regm.toString(Register[value]), "r9");
                }
            }
            else if (value->getType()->isFloat())
            {
                if (imm < 256)
                    code += emitInst_mem("vldr.32", regm.toString(Register[value]), "fp", AVALUE[value].stack_offset);
                else
                {
                    code += emitInst_1srcR_1DstR("sub", "r9", "fp", imm / 256 * 256);
                    if (imm % 256 != 0)
                        code += emitInst_1srcR_1DstR("sub", "r9", "r9", imm % 256);
                    code += emitInst_mem("vldr.32", regm.toString(Register[value]), "r9");
                }
            }
            AVALUE[value].reg_num = Register[value];
            RVALUE[Register[value]] = value;
        }
        // 6.更新RVALUE,AVALUE
        for (auto arg : args)
        {
            if (isa<ConstantValue>(arg))
                continue;
            auto value = dynamic_cast<Instruction *>(arg);
            // 若变量不再活跃,将其AVALUE删除
            if (value->GetEnd() == callInst->GetStart())
            {
                RVALUE[Register[value]] = nullptr;
                auto iter = AVALUE.find(value);
                if (iter != AVALUE.end())
                    AVALUE.erase(iter);
            }
        }
        if (!callInst->getType()->isVoid())
        {
            AVALUE[callInst].reg_num = dstRegId;
            if (dstRegId == RegManager::NONE)
                AVALUE[callInst].stack_offset = callInst->GetLocation();
            RVALUE[dstRegId] = callInst;
        }
        return {dstRegId, code};
    }
    string CodeGen::instruction_gen(Instruction *instr)
    {
        string code;
        string unkName = instr->getName();
        RegManager::RegId dstRegId = RegManager::NONE;
        auto instrType = instr->getKind();
        pair<RegId, string> tmp;
        switch (instrType)
        {
        // binary float inst
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
            BinaryInst *bInst = dynamic_cast<BinaryInst *>(instr);
            // registers are used only for instruction operation, consider use which register (any one that is free for use)
            tmp = binaryFloatInst_gen(bInst, RegManager::ANY);
            code += tmp.second;
            dstRegId = tmp.first;
            break;
        }
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
        {
            BinaryInst *bInst = dynamic_cast<BinaryInst *>(instr);
            // registers are used only for instruction operation, consider use which register (any one that is free for use)
            tmp = binaryInst_gen(bInst, RegManager::ANY);
            code += tmp.second;
            dstRegId = tmp.first;
            break;
        }
        case Instruction::kLoad:
        {
            LoadInst *ldInst = dynamic_cast<LoadInst *>(instr);
            tmp = loadInst_gen(ldInst, RegManager::ANY);
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
            tmp = allocaInst_gen(aInst, RegManager::ANY);
            // code += M_emitComment("alloca inst");
            code += tmp.second;
            dstRegId = tmp.first;
            break;
        }
        case Instruction::kReturn:
        {
            ReturnInst *retInst = dynamic_cast<ReturnInst *>(instr);
            // code += M_emitComment("return inst");
            code += returnInst_gen(retInst);
            return code;
            break;
        }
        case Instruction::kCall:
        {
            CallInst *cInst = dynamic_cast<CallInst *>(instr);
            auto tmp = callInst_gen(cInst, RegManager::ANY);
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
        case Instruction::kFNeg:
        case Instruction::kFtoI:
        case Instruction::kNeg:
        case Instruction::kNot:
        case Instruction::kItoF:
        {
            UnaryInst *uInst = dynamic_cast<UnaryInst *>(instr);
            tmp = unaryInst_gen(uInst, RegManager::ANY);
            // code += "unary instr\n";
            code += tmp.second;
            dstRegId = tmp.first;
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
        globalval.insert(glbvl);

        asmCode += space + ".global\t" + name + endl;
        auto type = static_cast<const PointerType *>(glbvl->getType())->getBaseType();
        // has init
        if (glbvl->init())
        {
            asmCode += space + ".data" + endl;
            //******************Revised by lyq BEGIN***************************************
            if (glbvl->isconst())
                asmCode += space + ".section" + space + ".rodata" + endl;
            //******************Revised by lyq END*****************************************
            asmCode += space + ".align\t2" + endl;
            asmCode += space + ".type\t" + name + ", %" + "object" + endl;
            asmCode += space + ".size\t" + name + ", " + to_string(glbvl->getsize() * 4) + endl;
            asmCode += name + ":\n";
            // globalvalue is a scalar
            if (glbvl->getNumDims() == 0)
            {
                auto value = dyncast<ConstantValue>(glbvl->getOperand(0));
                string val;
                if (type->isInt())
                    val = to_string(value->getInt());
                else if (type->isFloat())
                {
                    float constant_value = value->getDouble();
                    int dec;
                    std::memcpy(&dec, &constant_value, sizeof(dec));
                    val = to_string(dec);
                }
                asmCode += space + ".word\t" + val + endl;
            }
            // globalvalue is an array
            else
            {
                int continue_zero = 0;
                if (type->isInt())
                {
                    int *array = glbvl->getIntarray();
                    for (int i = 0; i < glbvl->getsize(); i++)
                    {
                        if (array[i] != 0)
                        {
                            if (continue_zero)
                                asmCode += space + ".space\t" + to_string(4 * continue_zero) + endl;
                            continue_zero = 0;
                            asmCode += space + ".word\t" + to_string(array[i]) + endl;
                        }
                        else
                            continue_zero++;
                    }
                    if (continue_zero)
                        asmCode += space + ".space\t" + to_string(4 * continue_zero) + endl;
                }
                else if (type->isFloat())
                {
                    double *array = glbvl->getDoublearray();
                    for (int i = 0; i < glbvl->getsize(); i++)
                    {
                        if (array[i] != 0)
                        {
                            if (continue_zero)
                                asmCode += space + ".space\t" + to_string(4 * continue_zero) + endl;
                            continue_zero = 0;
                            float num = array[i];
                            int dec = 0;
                            memcpy(&dec, &num, sizeof(num));
                            asmCode += space + ".word\t" + to_string(dec) + endl;
                        }
                        else
                            continue_zero++;
                    }
                    if (continue_zero)
                        asmCode += space + ".space\t" + to_string(4 * continue_zero) + endl;
                }
            }
        }
        // no init
        else
        {
            asmCode += space + ".bss" + endl;
            //******************Revised by lyq BEGIN***************************************
            if (glbvl->isconst())
                asmCode += space + ".section" + space + "rodata" + endl;
            //******************Revised by lyq END*****************************************
            asmCode += space + ".align\t2" + endl;
            asmCode += space + ".type\t" + name + ", %object" + endl;
            asmCode += space + ".size\t" + name + ", " + to_string(glbvl->getsize() * 4) + endl;
            asmCode += name + ":\n";
            asmCode += space + ".space\t" + to_string(4 * glbvl->getsize()) + endl;
        }

        return asmCode;
    }

    string CodeGen::literalPoolsCode_gen(Function *func)
    {
        string code;
        string immcode;
        if (imms.size())
            immcode += imms_name + ":" + endl;
        for (auto imm : imms)
        {
            // double Dimm = imm;
            // long long num;
            // int num1, num2;
            // memcpy(&num, &Dimm, sizeof(Dimm));
            // num1 = (num & 0x00000000FFFFFFFF);
            // num2 = (num >> 32) & 0xFFFFFFFF;
            // immcode += space + ".word\t" + to_string(num1) + endl;
            // immcode += space + ".word\t" + to_string(num2) + endl;
            immcode += space + ".word\t" + to_string(imm) + endl;
        }
        code += immcode;
        return code;
    }

} // namespace backend
