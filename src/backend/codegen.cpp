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
        string immcode;
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
        if (imms.size())
            immcode += ".IMM:" + endl;
        for (auto imm : imms)
        {
            double Dimm = imm;
            long long num;
            int num1, num2;
            memcpy(&num, &Dimm, sizeof(Dimm));
            num1 = (num & 0x00000000FFFFFFFF);
            num2 = (num >> 32) & 0xFFFFFFFF;
            immcode += space + ".word\t" + to_string(num1) + endl;
            immcode += space + ".word\t" + to_string(num2) + endl;
        }
        code += (dataCode + textCode + immcode + endl);
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
        if (libfunc.find(func->getName()) != libfunc.end())
            return code;
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
        string bbLabel = bb->getName();
        // string bbLabel = getBBLabel(bb);
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
        auto rhs = bInst->getRhs();
        bool lconst = false, rconst = false;
        if (isa<ConstantValue>(lhs))
        {
            lname = "#" + to_string(dynamic_cast<ConstantValue *>(lhs)->getInt());
            lconst = true;
        }
        else if (isa<CallInst>(lhs))
        {
            lname = "r" + to_string(stoi(lhs->getName()) + 1);
        }
        else
            lname = "r" + lhs->getName();
        if (isa<ConstantValue>(rhs))
        {
            rname = "#" + to_string(dynamic_cast<ConstantValue *>(rhs)->getInt());
            rconst = true;
        }
        else if (isa<CallInst>(rhs))
            rname = "r" + to_string(stoi(rhs->getName()) + 1);
        else
            rname = "r" + rhs->getName();
        auto res = bInst->getName();
        if (bInst->getKind() == Instruction::kAdd)
        {
            if (lconst && rconst)
            {
                int val = dynamic_cast<ConstantValue *>(lhs)->getInt() + dynamic_cast<ConstantValue *>(rhs)->getInt();
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
                code += space + "mov\tr" + res + ", " + lname + endl;
                code += space + "add\tr" + res + ", r" + res + ", " + rname + endl;
            }
            else
                code += space + "add\tr" + res + ", " + lname + ", " + rname + endl;
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
                code += space + "mov\tr" + res + ", " + lname + endl;
                code += space + "sub\tr" + res + ", r" + res + ", " + rname + endl;
            }
            else
                code += space + "sub\tr" + res + ", " + lname + ", " + rname + endl;
        }
        else if (bInst->getKind() == Instruction::kMul)
        {
            if (lconst && rconst)
            {
                int val = dynamic_cast<ConstantValue *>(lhs)->getInt() * dynamic_cast<ConstantValue *>(rhs)->getInt();
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
                code += space + "mov\tr" + res + ", " + lname + endl;
                code += space + "mul\tr" + res + ", r" + res + ", " + rname + endl;
            }
            else if (rconst)
            {
                code += space + "mov\tr" + res + ", " + rname + endl;
                code += space + "mul\tr" + res + ", " + lname + ", r" + res + endl;
            }
            else
                code += space + "mul\tr" + res + ", " + lname + ", " + rname + endl;
        }
        else if (bInst->getKind() == Instruction::kDiv)
            code += space + "div\tr" + res + ", " + lname + ", " + rname + endl;
        else if (lconst && rconst)
            return {dstRegId, code};
        else if (bInst->getKind() == Instruction::kICmpEQ)
            code += space + "cmp\t" + lname + ", " + rname + endl;
        else if (bInst->getKind() == Instruction::kICmpGE)
            code += space + "cmp\t" + lname + ", " + rname + endl;
        else if (bInst->getKind() == Instruction::kICmpGT)
            code += space + "cmp\t" + lname + ", " + rname + endl;
        else if (bInst->getKind() == Instruction::kICmpLE)
            code += space + "cmp\t" + lname + ", " + rname + endl;
        else if (bInst->getKind() == Instruction::kICmpLT)
            code += space + "cmp\t" + lname + ", " + rname + endl;
        else if (bInst->getKind() == Instruction::kICmpNE)
            code += space + "cmp\t" + lname + ", " + rname + endl;
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
        bool lconst = false, rconst = false;
        if (isa<ConstantValue>(lhs))
        {
            // float val = dynamic_cast<ConstantValue *>(lhs)->getFloat();
            float val = dynamic_cast<ConstantValue *>(lhs)->getDouble();
            unsigned int num;
            memcpy(&num, &val, sizeof(val));
            lname = "#" + to_string(num);
            lconst = true;
        }
        else if (isa<CallInst>(lhs))
        { // 如果左右操作数都是函数调用返回值，那么先把第一个左操作数的函数返回值放到s1里面，第二个操作数的值仍在s0里
            lname = 's' + to_string(16 - stoi(lhs->getName()));
        }
        else
            lname = "s" + to_string(15 - std::stoi(lhs->getName()));
        if (isa<ConstantValue>(rhs))
        {
            // float val = dynamic_cast<ConstantValue *>(rhs)->getFloat();
            float val = dynamic_cast<ConstantValue *>(rhs)->getDouble();
            unsigned int num;
            memcpy(&num, &val, sizeof(val));
            rname = "#" + to_string(num);
            rconst = true;
        }
        else if (isa<CallInst>(rhs))
            rname = 's' + to_string(16 - stoi(rhs->getName()));
        else
            rname = "s" + to_string(15 - std::stoi(rhs->getName()));
        auto res = to_string(15 - std::stoi(bInst->getName()));
        // 各种指令分开处理
        if (bInst->getKind() == Instruction::kFAdd)
        {
            if (lconst && rconst)
            {
                // float val = dynamic_cast<ConstantValue *>(lhs)->getFloat() + dynamic_cast<ConstantValue *>(rhs)->getFloat();
                double val = dynamic_cast<ConstantValue *>(lhs)->getDouble() + dynamic_cast<ConstantValue *>(rhs)->getDouble();
                unsigned num;
                memcpy(&num, &val, sizeof(val));
                code += space + "vmov.f32\ts" + res + "#" + to_string(num) + endl;
            }
            else if (lconst)
            {
                double val_l = dynamic_cast<ConstantValue *>(lhs)->getDouble();
                // lname 是一个立即数，rname是一个32位寄存器号，dname是把rname放到64位寄存器的那个寄存器号，immname是存立即数的64位寄存器
                std::string dname = to_string(16 + std::stoi(rhs->getName()));
                std::string immname = to_string(16 + std::stoi(bInst->getName()));
                code += space + "vcvt.f64.f32\td" + dname + ", " + rname + endl;
                // code += space + "vmov.f64\td" + immname + ", " + lname + endl;
                code += space + "vldr.64\td" + immname + ", .IMM+" + to_string(imm_offset) + endl;
                imm_offset += 8;
                imms.push_back(val_l);
                code += space + "vadd.f64\td" + immname + ", d" + immname + ", d" + dname + endl;
                code += space + "vcvt.f32.f64\ts" + to_string(15 - std::stoi(bInst->getName())) + ", d" + immname + endl;
            }
            else if (rconst)
            {
                double val_r = dynamic_cast<ConstantValue *>(rhs)->getDouble();
                // lname 是一个立即数，rname是一个32位寄存器号，dname是把rname放到64位寄存器的那个寄存器号，immname是存立即数的64位寄存器
                std::string dname = to_string(16 + std::stoi(lhs->getName()));
                std::string immname = to_string(16 + std::stoi(bInst->getName()));
                code += space + "vcvt.f64.f32\td" + dname + ", " + lname + endl;
                code += space + "vldr.64\td" + immname + ", .IMM+" + to_string(imm_offset) + endl;
                imm_offset += 8;
                imms.push_back(val_r);
                code += space + "vadd.f64\td" + immname + ", d" + immname + ", d" + dname + endl;
                code += space + "vcvt.f32.f64\ts" + to_string(15 - std::stoi(bInst->getName())) + ", d" + immname + endl;
            }
            else
                code += space + "vadd.f32\ts" + res + ", " + lname + ", " + rname + endl;
        }
        else if (bInst->getKind() == Instruction::kFSub)
        {
            if (lconst && rconst)
            {
                double val = dynamic_cast<ConstantValue *>(lhs)->getDouble() - dynamic_cast<ConstantValue *>(rhs)->getDouble();
                unsigned num;
                memcpy(&num, &val, sizeof(val));
                code += space + "vmov.f32\ts" + res + "#" + to_string(num) + endl;
            }
            else if (lconst)
            {
                // float val_l = dynamic_cast<ConstantValue *>(lhs)->getFloat();
                double val_l = dynamic_cast<ConstantValue *>(lhs)->getDouble();
                // lname 是一个立即数，rname是一个32位寄存器号，dname是把rname放到64位寄存器的那个寄存器号，immname是存立即数的64位寄存器
                std::string dname = to_string(16 + std::stoi(rhs->getName()));
                std::string immname = to_string(16 + std::stoi(bInst->getName()));
                code += space + "vcvt.f64.f32\td" + dname + ", " + rname + endl;
                // code += space + "vmov.f64\td" + immname + ", " + lname + endl;
                code += space + "vldr.64\td" + immname + ", .IMM+" + to_string(imm_offset) + endl;
                imm_offset += 8;

                imms.push_back(val_l);
                code += space + "vsub.f64\td" + immname + ", d" + immname + ", d" + dname + endl;
                code += space + "vcvt.f32.f64\ts" + to_string(15 - std::stoi(bInst->getName())) + ", d" + immname + endl;
            }
            else if (rconst)
            {
                // float val_r = dynamic_cast<ConstantValue *>(rhs)->getFloat();
                double val_r = dynamic_cast<ConstantValue *>(rhs)->getDouble();
                // lname 是一个立即数，rname是一个32位寄存器号，dname是把rname放到64位寄存器的那个寄存器号，immname是存立即数的64位寄存器
                std::string dname = to_string(16 + std::stoi(lhs->getName()));
                std::string immname = to_string(16 + std::stoi(bInst->getName()));
                code += space + "vcvt.f64.f32\td" + dname + ", " + lname + endl;
                code += space + "vldr.64\td" + immname + ", .IMM+" + to_string(imm_offset) + endl;
                imm_offset += 8;
                imms.push_back(val_r);
                code += space + "vsub.f64\td" + dname + ", d" + dname + ", d" + immname + endl;
                code += space + "vcvt.f32.f64\ts" + to_string(15 - std::stoi(bInst->getName())) + ", d" + dname + endl;
            }
            else
                code += space + "vsub.f32\ts" + res + ", " + lname + ", " + rname + endl;
        }
        else if (bInst->getKind() == Instruction::kFMul)
        {
            if (lconst && rconst)
            {
                // float val = dynamic_cast<ConstantValue *>(lhs)->getFloat() * dynamic_cast<ConstantValue *>(rhs)->getFloat();
                double val = dynamic_cast<ConstantValue *>(lhs)->getDouble() * dynamic_cast<ConstantValue *>(rhs)->getDouble();
                unsigned num;
                memcpy(&num, &val, sizeof(val));
                code += space + "vmov.f32\ts" + res + "#" + to_string(num) + endl;
            }
            else if (lconst)
            {
                double val_l = dynamic_cast<ConstantValue *>(lhs)->getDouble();
                // lname 是一个立即数，rname是一个32位寄存器号，dname是把rname放到64位寄存器的那个寄存器号，immname是存立即数的64位寄存器
                std::string dname = to_string(16 + std::stoi(rhs->getName()));
                std::string immname = to_string(16 + std::stoi(bInst->getName()));
                code += space + "vcvt.f64.f32\td" + dname + ", " + rname + endl;
                // code += space + "vmov.f64\td" + immname + ", " + lname + endl;
                code += space + "vldr.64\td" + immname + ", .IMM+" + to_string(imm_offset) + endl;
                imm_offset += 8;
                imms.push_back(val_l);
                code += space + "vmul.f64\td" + immname + ", d" + immname + ", d" + dname + endl;
                code += space + "vcvt.f32.f64\ts" + to_string(15 - std::stoi(bInst->getName())) + ", d" + immname + endl;
            }
            else if (rconst)
            {
                double val_r = dynamic_cast<ConstantValue *>(rhs)->getDouble();
                // lname 是一个立即数，rname是一个32位寄存器号，dname是把rname放到64位寄存器的那个寄存器号，immname是存立即数的64位寄存器
                std::string dname = to_string(16 + std::stoi(lhs->getName()));
                std::string immname = to_string(16 + std::stoi(bInst->getName()));
                code += space + "vcvt.f64.f32\td" + dname + ", " + lname + endl;
                code += space + "vldr.64\td" + immname + ", .IMM+" + to_string(imm_offset) + endl;
                imm_offset += 8;
                imms.push_back(val_r);
                code += space + "vmul.f64\td" + immname + ", d" + immname + ", d" + dname + endl;
                code += space + "vcvt.f32.f64\ts" + to_string(15 - std::stoi(bInst->getName())) + ", d" + immname + endl;
            }
            else
                code += space + "vmul.f32\ts" + res + ", " + lname + ", " + rname + endl;
        }
        else if (bInst->getKind() == Instruction::kFDiv)
        {
            if (lconst && rconst)
            {
                double val = dynamic_cast<ConstantValue *>(lhs)->getDouble() - dynamic_cast<ConstantValue *>(rhs)->getDouble();
                unsigned num;
                memcpy(&num, &val, sizeof(val));
                code += space + "vmov.f32\ts" + res + "#" + to_string(num) + endl;
            }
            else if (lconst)
            {
                double val_l = dynamic_cast<ConstantValue *>(lhs)->getDouble();
                // lname 是一个立即数，rname是一个32位寄存器号，dname是把rname放到64位寄存器的那个寄存器号，immname是存立即数的64位寄存器
                std::string dname = to_string(16 + std::stoi(rhs->getName()));
                std::string immname = to_string(16 + std::stoi(bInst->getName()));
                code += space + "vcvt.f64.f32\td" + dname + ", " + rname + endl;
                // code += space + "vmov.f64\td" + immname + ", " + lname + endl;
                code += space + "vldr.64\td" + immname + ", .IMM+" + to_string(imm_offset) + endl;
                imm_offset += 8;
                imms.push_back(val_l);
                code += space + "vdiv.f64\td" + immname + ", d" + immname + ", d" + dname + endl;
                code += space + "vcvt.f32.f64\ts" + to_string(15 - std::stoi(bInst->getName())) + ", d" + immname + endl;
            }
            else if (rconst)
            {
                double val_r = dynamic_cast<ConstantValue *>(rhs)->getDouble();
                // lname 是一个立即数，rname是一个32位寄存器号，dname是把rname放到64位寄存器的那个寄存器号，immname是存立即数的64位寄存器
                std::string dname = to_string(16 + std::stoi(lhs->getName()));
                std::string immname = to_string(16 + std::stoi(bInst->getName()));
                code += space + "vcvt.f64.f32\td" + dname + ", " + lname + endl;
                code += space + "vldr.64\td" + immname + ", .IMM+" + to_string(imm_offset) + endl;
                imm_offset += 8;
                imms.push_back(val_r);
                code += space + "vdiv.f64\td" + immname + ", d" + dname + ", d" + immname + endl;
                code += space + "vcvt.f32.f64\ts" + to_string(15 - std::stoi(bInst->getName())) + ", d" + immname + endl;
            }
            else
                code += space + "vdiv.f32\ts" + res + ", " + lname + ", " + rname + endl;
        }
        else if (lconst && rconst)
            return {dstRegId, code};
        else if (bInst->getKind() == Instruction::kICmpEQ)
            code += space + "cmp\t" + lname + ", " + rname + endl;
        else if (bInst->getKind() == Instruction::kICmpGE)
            code += space + "cmp\t" + lname + ", " + rname + endl;
        else if (bInst->getKind() == Instruction::kICmpGT)
            code += space + "cmp\t" + lname + ", " + rname + endl;
        else if (bInst->getKind() == Instruction::kICmpLE)
            code += space + "cmp\t" + lname + ", " + rname + endl;
        else if (bInst->getKind() == Instruction::kICmpLT)
            code += space + "cmp\t" + lname + ", " + rname + endl;
        else if (bInst->getKind() == Instruction::kICmpNE)
            code += space + "cmp\t" + lname + ", " + rname + endl;

        return {dstRegId, code};
    }

    pair<RegId, string> CodeGen::unaryInst_gen(UnaryInst *uInst, RegId dstRegId)
    {
        string code;
        /**
         *code in here
         */
        auto val = uInst->getOperand();
        string val_name;
        if (uInst->getKind() == Instruction::kNeg)
        {
            if (isa<ConstantValue>(val))
            {
                val_name = to_string(dynamic_cast<ConstantValue *>(val)->getInt());
                code += space + "mov\tr" + uInst->getName() + ", #" + val_name + endl;
                code += space + "mvn\tr" + uInst->getName() + ", #1" + endl;
            }
            else
            {
                val_name = "r" + val->getName();
                code += space + "rsb\tr" + uInst->getName() + ", " + val_name + ", #0" + endl;
            }
        }
        else if (uInst->getKind() == Instruction::kFNeg)
        {
            val_name = "s" + to_string(15 - std::stoi(val->getName()));
            code += space + "vneg.f32\ts" + to_string(15 - std::stoi(uInst->getName())) + ", " + val_name + endl;
        }
        else if (uInst->getKind() == Instruction::kFtoI)
        {
            val_name = "s" + to_string(15 - std::stoi(val->getName()));
            code += space + "vcvt.s32.f32\ts" + to_string(15 - std::stoi(uInst->getName())) + "," + val_name + endl;
        }
        else if (uInst->getKind() == Instruction::kItoF)
        {
            val_name = "s" + to_string(15 - std::stoi(val->getName()));
            code += space + "vcvt.f32.s32\ts" + to_string(15 - std::stoi(uInst->getName())) + "," + val_name + endl;
        }
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
        if (globalval.find(dynamic_cast<GlobalValue *>(pointer)) != globalval.end())
        {
            code += space + "movw\tr10, #:lower16:" + pointer->getName() + endl;
            code += space + "movt\tr10, #:upper16:" + pointer->getName() + endl;
            if (isa<ConstantValue>(value))
            {
                if (value->isInt())
                {
                    int constant_value = dynamic_cast<ConstantValue *>(value)->getInt();
                    code += space + "mov\tr3, #" + to_string(constant_value) + endl;
                    code += space + "str\tr3, [r10]" + endl;
                }
                else if (value->isFloat())
                {
                    double constant_value = dynamic_cast<ConstantValue *>(value)->getDouble();
                    unsigned dec;
                    std::memcpy(&dec, &constant_value, sizeof(dec));
                    code += space + "movw\tr3, #" + to_string(dec & 0x0000FFFF) + endl;
                    code += space + "movt\tr3, #" + to_string(dec >> 16) + endl;
                    code += space + "str\tr3, [r10]\t@ float" + endl;
                }
            }
            else if (isa<CallInst>(value))
            {
                if (dynamic_cast<CallInst *>(value)->getCallee()->getReturnType()->isInt())
                    code += space + "str\tr0, [r10]" + endl;
                else if (dynamic_cast<CallInst *>(value)->getCallee()->getReturnType()->isFloat())
                    code += space + "vstr.32\tr0, [r10]" + endl;
            }
            else
            {
                if (value->getType()->isInt())
                    code += space + "str\tr" + value->getName() + ", [r10]" + endl;
                else if (value->getType()->isFloat())
                    code += space + "vstr.32\ts" + to_string(15 - std::stoi(value->getName())) + ", [r10]" + endl;
            }
            return code;
        }
        int offset;
        if (localVarStOffset.find(dynamic_cast<Instruction *>(pointer)) != localVarStOffset.end())
            offset = localVarStOffset[dynamic_cast<Instruction *>(pointer)];
        else if (paramsStOffset.find(dynamic_cast<Argument *>(pointer)) != paramsStOffset.end())
            offset = paramsStOffset[dynamic_cast<Argument *>(pointer)];
        if (isa<ConstantValue>(value))
        {
            if (value->isInt())
            {
                int constant_value = dynamic_cast<ConstantValue *>(value)->getInt();
                code += space + "mov\tr3, #" + to_string(constant_value) + endl;
                code += space + "str\tr3, [fp, #" + to_string(offset) + "]" + endl;
            }
            else if (value->isFloat())
            {
                float constant_value = dynamic_cast<ConstantValue *>(value)->getDouble();

                unsigned dec;
                std::memcpy(&dec, &constant_value, sizeof(dec));
                code += space + "movw\tr3, #" + to_string(dec & 0x0000FFFF) + endl;
                code += space + "movt\tr3, #" + to_string(dec >> 16) + endl;
                code += space + "str\tr3, [fp, #" + to_string(offset) + "]\t@ float" + endl;
            }
        }
        else if (isa<CallInst>(value))
        {
            if (dynamic_cast<CallInst *>(value)->getCallee()->getReturnType()->isInt())
                code += space + "str\tr0, [fp, #" + to_string(offset) + "]" + endl;
            else if (dynamic_cast<CallInst *>(value)->getCallee()->getReturnType()->isFloat())
                code += space + "vstr.32\ts0, [fp, #" + to_string(offset) + "]" + endl;
        }
        else
        {
            if (value->getType()->isInt())
                code += space + "str\tr" + value->getName() + ", [fp, #" + to_string(offset) + "]" + endl;
            else if (value->getType()->isFloat())
                code += space + "vstr.32\ts" + to_string(15 - std::stoi(value->getName())) + ", [fp, #" + to_string(offset) + "]" + endl;
        }
        return code;
    }
    pair<RegId, string> CodeGen::loadInst_gen(LoadInst *ldInst, RegId dstRegId)
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
        if (localVarStOffset.find(dynamic_cast<Instruction *>(var)) != localVarStOffset.end())
        {
            pos = localVarStOffset[dynamic_cast<Instruction *>(var)];
            if (var->getType()->as<PointerType>()->getBaseType()->isInt())
                code += space + "ldr\tr" + to_string(reg_num) + ", [fp, #" + to_string(pos) + "]" + endl;
            else if (var->getType()->as<PointerType>()->getBaseType()->isFloat())
                code += space + "vldr.32\ts" + to_string(15 - reg_num) + ", [fp, #" + to_string(pos) + "]" + endl;
        }
        // if var is a globalvalue
        else if (globalval.find(dynamic_cast<GlobalValue *>(var)) != globalval.end())
        {
            code += space + "movw\tr10, #:lower16:" + var->getName() + endl;
            code += space + "movt\tr10, #:upper16:" + var->getName() + endl;
            if (var->getType()->as<PointerType>()->getBaseType()->isInt())
                code += space + "ldr\tr" + to_string(reg_num) + ", [r10]" + endl;
            else if (var->getType()->as<PointerType>()->getBaseType()->isFloat())
                code += space + "vldr.32\ts" + to_string(15 - reg_num) + ", [r10]" + endl;
        }
        // if var is an argument
        else
        {
            if (var->getType()->as<PointerType>()->getBaseType()->isInt())
                code += space + "ldr\tr" + to_string(reg_num) + ", [fp, #unk]" + endl;
            else if (var->getType()->as<PointerType>()->getBaseType()->isInt())
                code += space + "vldr.32\ts" + to_string(15 - reg_num) + ", [fp, #unk]" + endl;
            backpatch.push_back(dynamic_cast<Argument *>(var));
        }
        return {dstRegId, code};
    }
    string CodeGen::returnInst_gen(ReturnInst *retInst)
    {
        string code;
        /**
         *code in here
         */
        auto retval = retInst->getReturnValue();
        if (isa<ConstantValue>(retval))
        {
            if (retval->getType()->isInt())
                code += space + "mov\tr0, #" + to_string(dynamic_cast<ConstantValue *>(retval)->getInt()) + endl;
            else if (retval->getType()->isFloat())
            {
                unsigned num1;
                double val1;
                val1 = dynamic_cast<ConstantValue *>(retval)->getDouble();
                memcpy(&num1, &val1, sizeof(val1));
                code += space + "movw\tr3, #" + to_string(num1 & 0x0000FFFF) + endl;
                code += space + "movt\tr3, #" + to_string(num1 >> 16) + endl;
                code += space + "vmov\ts15, r3" + endl;
                code += space + "vmov.f32\ts0, s15" + endl;
            }
        }
        else if (isa<CallInst>(retval))
            ;
        else
        {
            if (retval->getKind() == Instruction::kFtoI)
                code += space + "vmov\tr0,s" + to_string(15 - stoi(retval->getName())) + endl;
            else if (retval->getType()->isInt())
                code += space + "mov\tr0, r" + retval->getName() + endl;
            else if (retval->getType()->isFloat())
                code += space + "vmov.f32\ts0, s" + to_string(15 - stoi(retval->getName())) + endl;
        }
        return code;
    }
    string CodeGen::uncondBrInst_gen(UncondBrInst *ubInst)
    {
        string code;
        /**
         *code in here
         */
        auto goal = ubInst->getBlock();
        code += space + "b\t" + goal->getName() + endl;
        return code;
    }
    string CodeGen::condBrInst_gen(CondBrInst *cbInst)
    {
        string code;
        /**
         *code in here
         */
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
                int lvalue = dynamic_cast<ConstantValue *>(lhs)->getInt();
                int rvalue = dynamic_cast<ConstantValue *>(rhs)->getInt();
                if (bInst->getKind() == Instruction::kICmpEQ)
                    if (lvalue == rvalue)
                        code += space + "b\t" + then_block->getName() + endl;
                    else
                        code += space + "b\t" + else_block->getName() + endl;
                else if (bInst->getKind() == Instruction::kICmpGE)
                    if (lvalue >= rvalue)
                        code += space + "b\t" + then_block->getName() + endl;
                    else
                        code += space + "b\t" + else_block->getName() + endl;
                else if (bInst->getKind() == Instruction::kICmpGT)
                    if (lvalue > rvalue)
                        code += space + "b\t" + then_block->getName() + endl;
                    else
                        code += space + "b\t" + else_block->getName() + endl;
                else if (bInst->getKind() == Instruction::kICmpLE)
                    if (lvalue <= rvalue)
                        code += space + "b\t" + then_block->getName() + endl;
                    else
                        code += space + "b\t" + else_block->getName() + endl;
                else if (bInst->getKind() == Instruction::kICmpLT)
                    if (lvalue < rvalue)
                        code += space + "b\t" + then_block->getName() + endl;
                    else
                        code += space + "b\t" + else_block->getName() + endl;
                else if (bInst->getKind() == Instruction::kICmpNE)
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
            else if (bInst->getKind() == Instruction::kICmpEQ)
            {
                // code += binaryInst_gen(bInst, RegManager::RANY).second;
                code += space + "beq\t" + then_block->getName() + endl;
                code += space + "b\t" + else_block->getName() + endl;
            }
            else if (bInst->getKind() == Instruction::kICmpGE)
            {
                // code += binaryInst_gen(bInst, RegManager::RANY).second;
                code += space + "bge\t" + then_block->getName() + endl;
                code += space + "b\t" + else_block->getName() + endl;
            }
            else if (bInst->getKind() == Instruction::kICmpGT)
            {
                // code += binaryInst_gen(bInst, RegManager::RANY).second;
                code += space + "bgt\t" + then_block->getName() + endl;
                code += space + "b\t" + else_block->getName() + endl;
            }
            else if (bInst->getKind() == Instruction::kICmpLE)
            {
                // code += binaryInst_gen(bInst, RegManager::RANY).second;
                code += space + "ble\t" + then_block->getName() + endl;
                code += space + "b\t" + else_block->getName() + endl;
            }
            else if (bInst->getKind() == Instruction::kICmpLT)
            {
                // code += binaryInst_gen(bInst, RegManager::RANY).second;
                code += space + "blt\t" + then_block->getName() + endl;
                code += space + "b\t" + else_block->getName() + endl;
            }
            else if (bInst->getKind() == Instruction::kICmpNE)
            {
                // code += binaryInst_gen(bInst, RegManager::RANY).second;
                code += space + "bne\t" + then_block->getName() + endl;
                code += space + "b\t" + else_block->getName() + endl;
            }
            else
            {
                code += space + "cmp\tr" + bInst->getName() + ", #0" + endl;
                code += space + "bne\t" + then_block->getName() + endl;
                code += space + "b\t" + else_block->getName() + endl;
            }
        }
        else if (isa<ConstantValue>(cond))
        {
            if (dynamic_cast<ConstantValue *>(cond)->getInt())
                code += space + "b\t" + then_block->getName() + endl;
            else
                code += space + "b\t" + else_block->getName() + endl;
        }
        else
        {
            code += space + "cmp\tr" + cond->getName() + ", #0" + endl;
            code += space + "bne\t" + then_block->getName() + endl;
            code += space + "b\t" + else_block->getName() + endl;
        }
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
            string src_name;
            if (isa<ConstantValue>(arg))
                src_name = "#" + to_string(dynamic_cast<ConstantValue *>(arg)->getInt());
            else
                src_name = "r" + arg->getName();
            code += space + "mov\tr" + to_string(arg_num - 1) + ", " + src_name + endl;
            // code += space + "mov\tr" + to_string(arg_num - 1) + ", r" + src + endl;
        }
        code += space + "bl\t" + callee_fuc->getName() + endl;
        if (callInst->getType()->isInt())
            code += space + "mov\tr" + to_string(stoi(callInst->getName()) + 1) + ", r0" + endl;
        else if (callInst->getType()->isFloat())
            code += space + "vmov\ts" + to_string(16 - stoi(callInst->getName())) + ", s0" + endl;
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
            tmp = binaryFloatInst_gen(bInst, RegManager::RANY);
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
            // code += M_emitComment("return inst");
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
        case Instruction::kFNeg:
        case Instruction::kFtoI:
        case Instruction::kNeg:
        case Instruction::kNot:
        {
            UnaryInst *uInst = dynamic_cast<UnaryInst *>(instr);
            tmp = unaryInst_gen(uInst, RegManager::RANY);
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
            {
                asmCode += space + ".align\t2" + endl;
            }
            asmCode += space + ".type\t" + name + ", " + "%" + "object" + endl;
            asmCode += space + ".size\t" + name + ", 4" + endl;
            asmCode += name + ":\n";
            auto value = dyncast<ConstantValue>(glbvl->getOperand(0));
            string val;
            std::stringstream ss;
            if (type->isInt())
                val = to_string(value->getInt());
            else if (type->isFloat())
            {
                double constant_value = value->getDouble();
                int dec;
                std::memcpy(&dec, &constant_value, sizeof(dec));
                val = to_string(dec);
            }
            asmCode += space + ".word\t" + val + endl;
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
