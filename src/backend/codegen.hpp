/**
 * TODO : 简单的寄存器分配方法 处理分配不到寄存器的情况.
 *
 * 后端设计需要注意的点
 *
 * label分配   : 全局变量的label,常量的label,basicblock的label,函数入口label,函数出口label
 * 全局数据处理
 * 常量处理
 * 临时变量的处理(包括函数参数也是临时变量)
 * 函数参数的处理
 * 块参数的处理
 * 函数栈空间结构的设计
 * 块的遍历问题
 * arm32的过程调用约定
 *
 * 后端测试需要注意的点
 *
 * 如何汇编与链接.
 * qemu以及树莓派如何运行.
 */
#ifndef __CODEGEN_HPP_
#define __CODEGEN_HPP_
#include <string>
#include <set>
#include <iostream>
#include <algorithm>
#include <IR.h>
#include <vector>
#include <sstream>

using namespace sysy;
using std::find;
using std::map;
using std::pair;
using std::set;
using std::string;
using std::to_string;
using std::vector;

#define Macro_ERROR_MSG(...)                                             \
    do                                                                   \
    {                                                                    \
        fprintf(stderr, "ERROR in line-%d of %s: ", __LINE__, __FILE__); \
        fprintf(stderr, __VA_ARGS__);                                    \
        fprintf(stderr, "\n");                                           \
        fflush(stderr);                                                  \
        exit(1);                                                         \
    } while (0);

#define M_emitInst(STR) (space + STR + endl)
#define M_emitComment(STR) (space + "//" + STR + endl)
#define M_emitLabel(STR) (STR + ":" + endl)

// #define REG_EXP

namespace backend
{
    static const string space = "\t";
    static const string endl = "\n";
    // only for armv7
    static const int int_align = 4;
    static const int int_size = 4;
    static const int int_p2align = 2;
    static const int reg_size = 4;

    // default arm32 max imm
    //
    static const uint32_t maxMovImm = 0xFFF;
    static const string stackIndexReg = "fp";

    static uint16_t getWordHigh(uint32_t cval)
    {
        return (cval >> 16) & 0xFFFF;
    }
    static uint16_t getWordLow(uint32_t cval)
    {
        return cval & 0xFFFF;
    }
    //
    static string
    emitInst_1srcR_noDstR(string name, string srcReg)
    {
        return space + name + " " + srcReg + endl;
    }
    static string
    emitInst_2srcR_1dstR(string name, string srcReg0, string srcReg1, string dstReg)
    {
        return space + name + " " + dstReg + ", " +
               srcReg0 + ", " + srcReg1 + endl;
    }
    //
    class RegManager
    {
    public:
        //{0,1,2,3,4,5,6,7,8,9,10};
        enum RegId : unsigned
        {
            R0 = 0,
            R1 = 1,
            R2 = 2,
            R3 = 3,
            R4 = 4,
            R5 = 5,
            R6 = 6,
            R7 = 7,
            R8 = 8,
            R9 = 9,
            R10 = 10,
            RNONE = 1024,
            RANY = 2048,
        };
        static string toString(RegId reg)
        {
            if (reg == RNONE)
                return "RNONE";
            if (reg == RANY)
                return "RANY";
            return "r" + to_string(reg);
        }
    };

    class Operand
    {
    public:
        using RegId = RegManager::RegId;
        enum Kind
        {
            kReg,
            kImm,
        };
        Kind kind;
        union
        {
            uint32_t imm;
            RegId regId;
        };
        Operand(){};
        Operand(uint32_t imm) : kind(kImm), imm(imm) {}
        Operand(RegId regId) : kind(kReg), regId(regId) {}
        bool isImm(void) { return kind == kImm; }
        bool isReg(void) { return kind == kReg; }
        uint32_t getImm(void)
        {
            assert(kind == kImm);
            return imm;
        }
        RegId getRegId(void)
        {
            assert(kind == kReg);
            return regId;
        }
        string toString(void)
        {
            if (kind == kImm)
                return "#" + to_string(imm);
            else
                return RegManager::toString(regId);
        }
    };

    class CodeGen
    {
    public:
        using RegId = RegManager::RegId;

    private:
        Module *module;
        Function *curFunc;
        BasicBlock *curBB;
        //
        RegManager regm;
        // globalValue
        bool loadGlobalValByMOVWT = true;
        // basicBlock
        vector<BasicBlock *> linear_bb;
        int bb_no = 0;
        // function params, return value and localVar
        map<Argument *, int> paramsStOffset;
        map<Instruction *, int> localVarStOffset;
        set<GlobalValue *> globalval;
        int retValueStOffset = 0;
        size_t stOffsetAcc = 0;
        // label manager
        map<BasicBlock *, string> bb_labels;
        uint64_t label_no = 0;
        int top_offset = -8;
        int above_offset = 4;
        int temp_offset = 0;
        int protect_reg_offset = 0;
        int max_param = 0;
        int max_protect = 1;
        int imm_offset = 0;
        vector<double> imms;
        string imms_name;
        bool haveCall = false;
        set<string> libfunc = {"getint", "getch", "getfloat", "getarray", "getfarray", "putint", "putch", "putfloat", "putarray", "putfarray", "starttime", "stoptime", "putf"};

    public:
        CodeGen(Module *module) : module(module) {}
        // code_gen function list
        string code_gen();
        string module_gen(Module *module);
        string function_gen(Function *func);
        string basicBlock_gen(BasicBlock *bb);
        string instruction_gen(Instruction *instr);
        string globaldata_gen(GlobalValue *glbvl);
        string prologueCode_gen(Function *func);
        string epilogueCode_gen(Function *func);
        string literalPoolsCode_gen(Function *func);
        string functionHead_gen(Function *func);
        // Module
        void clearModuleRecord(Module *module)
        {
            label_no = 0;
        }
        // instruction gen function list
        //<dstReg, code>
        pair<RegId, string> loadInst_gen(LoadInst *ldInst, RegId dstRegId);
        string storeInst_gen(StoreInst *stInst);
        pair<RegId, string> allocaInst_gen(AllocaInst *aInst, RegId dstRegId);
        string returnInst_gen(ReturnInst *retInst);
        pair<RegId, string> callInst_gen(CallInst *retInst, RegId dstRegId);
        pair<RegId, string> binaryInst_gen(BinaryInst *bInst, RegId dstRegId);
        pair<RegId, string> binaryFloatInst_gen(BinaryInst *bInst, RegId dstRegId);
        pair<RegId, string> unaryInst_gen(UnaryInst *bInst, RegId dstRegId);
        string uncondBrInst_gen(UncondBrInst *ubInst);
        string condBrInst_gen(CondBrInst *ubInst);

        //
        string storeRegToStack_gen(RegId regId, Instruction *inst)
        {
            string code;
            /**
             *code in here
             */
            return code;
        }
        // function
        void clearFunctionRecord(Function *func)
        {
            localVarStOffset.clear();
            paramsStOffset.clear();
            retValueStOffset = 0;
            bb_labels.clear();
            haveCall = false;
            top_offset = -8;
            above_offset = 4;
            max_protect = 1;
            max_param = 0;
            temp_offset = 0;
            protect_reg_offset = 0;
            imm_offset = 0;
            imms.clear();
            imms_name = "IMM_" + func->getName();
            //
            stOffsetAcc = 0;
        }
        string getBBLabel(BasicBlock *bb)
        {
            auto t = bb_labels.find(bb);
            string label;
            if (t == bb_labels.end())
            {
                label = ".LBB_" + to_string(label_no++);
                bb_labels.emplace(bb, label);
            }
            else
            {
                label = t->second;
            }
            return label;
        }
    };
}

#endif //__CODEGEN_HPP_
