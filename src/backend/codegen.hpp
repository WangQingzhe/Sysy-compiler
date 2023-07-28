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
#include <queue>
#include <functional>

using namespace sysy;
using std::find;
using std::map;
using std::pair;
using std::priority_queue;
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
    emitInst_mem(string name, string reg, string base, int offset = 0)
    {
        string inst = space + name + "\t" + reg + ", [" + base;
        if (offset == 0)
            inst += "]" + endl;
        else
            inst += ", #" + to_string(offset) + "]" + endl;
        return inst;
    }
    static string
    emitInst_nosrcR_1DstR(string name, string dstReg, int imm = 0)
    {
        return space + name + "\t" + dstReg + ", #" + to_string(imm) + endl;
    }
    static string
    emitInst_1srcR_1DstR(string name, string dstReg, string srcReg, int imm = 0)
    {
        string inst = space + name + "\t" + dstReg + ", " + srcReg;
        if (imm == 0)
            inst += endl;
        else
            inst += ", #" + to_string(imm) + endl;
        return inst;
    }
    static string
    emitInst_2srcR_1dstR(string name, string dstReg, string srcReg0, string srcReg1)
    {
        return space + name + "\t" + dstReg + ", " + srcReg0 + ", " + srcReg1 + endl;
    }
    //
    class RegManager
    {
    public:
        // 默认构造函数
        RegManager() {}
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
            S0 = 11,
            S1 = 12,
            S2 = 13,
            S3 = 14,
            S4 = 15,
            S5 = 16,
            S6 = 17,
            S7 = 18,
            S8 = 19,
            S9 = 20,
            S10 = 21,
            S11 = 22,
            S12 = 23,
            S13 = 24,
            S14 = 25,
            S15 = 26,
            NONE = 1024,
            ANY = 2048,
        };
        static string toString(RegId reg)
        {
            if (reg == NONE)
                return "NONE";
            if (reg == ANY)
                return "ANY";
            if (reg >= 0 && reg <= 10)
                return "r" + to_string(reg);
            int sreg = reg;
            sreg -= 11;
            return "s" + to_string(sreg);
        }
        static const int R = 9;  // 通用R寄存器个数:r0-r8
        static const int S = 14; // 通用S寄存器个数:s0-s13
        // 当前空闲的r寄存器池
        priority_queue<RegId, vector<RegId>, std::greater<RegId>> reg_pool;
        // 当前空闲的s寄存器池
        priority_queue<RegId, vector<RegId>, std::greater<RegId>> sreg_pool;
        // 将所有通用寄存器设为空闲状态
        void ResetRegPool()
        {
            while (!reg_pool.empty())
                reg_pool.pop();
            while (!sreg_pool.empty())
                sreg_pool.pop();
            reg_pool.emplace(R0);
            reg_pool.emplace(R1);
            reg_pool.emplace(R2);
            reg_pool.emplace(R3);
            reg_pool.emplace(R4);
            reg_pool.emplace(R5);
            reg_pool.emplace(R6);
            reg_pool.emplace(R7);
            reg_pool.emplace(R8);
            sreg_pool.emplace(S0);
            sreg_pool.emplace(S1);
            sreg_pool.emplace(S2);
            sreg_pool.emplace(S3);
            sreg_pool.emplace(S4);
            sreg_pool.emplace(S5);
            sreg_pool.emplace(S6);
            sreg_pool.emplace(S7);
            sreg_pool.emplace(S8);
            sreg_pool.emplace(S9);
            sreg_pool.emplace(S10);
            sreg_pool.emplace(S11);
            sreg_pool.emplace(S12);
            sreg_pool.emplace(S13);
        }
        // 从r寄存器池中获取一个空闲r寄存器
        RegId GetFreeReg()
        {
            RegId free_reg = reg_pool.top();
            reg_pool.pop();
            return free_reg;
        }
        // 从s寄存器池中获取一个空闲s寄存器
        RegId GetFreesReg()
        {
            RegId free_reg = sreg_pool.top();
            sreg_pool.pop();
            return free_reg;
        }
        // 将一个r寄存器释放
        void AddFreeReg(RegId reg)
        {
            reg_pool.push(reg);
        }
        // 将一个s寄存器释放
        void AddFreesReg(RegId reg)
        {
            sreg_pool.push(reg);
        }
        // 将数字转化为R寄存器
        RegId ConvertToR(unsigned reg_num)
        {
            if (reg_num == 0)
                return R0;
            else if (reg_num == 1)
                return R1;
            else if (reg_num == 2)
                return R2;
            else if (reg_num == 3)
                return R3;
            else if (reg_num == 4)
                return R4;
            else if (reg_num == 5)
                return R5;
            else if (reg_num == 6)
                return R6;
            else if (reg_num == 7)
                return R7;
            else if (reg_num == 8)
                return R8;
            else if (reg_num == 9)
                return R9;
            else if (reg_num == 10)
                return R10;
            return NONE;
        }
        // 将数字转化为S寄存器
        RegId ConvertToS(unsigned reg_num)
        {
            if (reg_num == 0)
                return S0;
            else if (reg_num == 1)
                return S1;
            else if (reg_num == 2)
                return S2;
            else if (reg_num == 3)
                return S3;
            else if (reg_num == 4)
                return S4;
            else if (reg_num == 5)
                return S5;
            else if (reg_num == 6)
                return S6;
            else if (reg_num == 7)
                return S7;
            else if (reg_num == 8)
                return S8;
            else if (reg_num == 9)
                return S9;
            else if (reg_num == 10)
                return S10;
            else if (reg_num == 11)
                return S11;
            else if (reg_num == 12)
                return S12;
            else if (reg_num == 13)
                return S13;
            else if (reg_num == 14)
                return S14;
            else if (reg_num == 15)
                return S15;
            return NONE;
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
        // 寄存器管理
        RegManager regm;
        // globalValue
        bool loadGlobalValByMOVWT = true;
        // basicBlock
        vector<BasicBlock *> linear_bb;
        int bb_no = 0;
        // function params, return value and localVar
        map<Argument *, int> paramsStOffset;      // 记录函数接收的形参的栈偏移
        map<Instruction *, int> localVarStOffset; // 记录函数局部变量的栈偏移
        set<GlobalValue *> globalval;
        int retValueStOffset = 0;
        size_t stOffsetAcc = 0;
        // label manager
        map<BasicBlock *, string> bb_labels;
        uint64_t label_no = 0;

        int top_offset = -8;        // 记录当前栈顶的偏移
        int above_offset = 4;       // 记录通过栈传递的参数的栈偏移
        int temp_offset = 0;        // 记录中间变量的栈偏移
        int protect_reg_offset = 0; // 记录保护区第一个元素的栈偏移
        int max_param = 0;          // 记录函数中所有函数调用的参数最多个数
        int max_protect = 1;        // 记录函数需要保护的变量的最多个数
        int imm_offset = 0;         // 记录当前引用的静态区立即数在静态区的偏移量
        vector<double> imms;        // 记录函数引用的所有静态区立即数
        string imms_name;           // 记录函数静态区立即数的标号名
        bool haveCall = false;      // 函数是否有子函数调用
        struct cmp
        {
            bool operator()(Instruction *const &a, Instruction *const &b) const
            {
                if (a->GetEnd() == b->GetEnd())
                    return a->GetStart() < b->GetStart();
                return a->GetEnd() < b->GetEnd();
            }
        };
        set<Instruction *, cmp> active;     // 记录当前活跃变量
        map<Instruction *, RegId> Register; // 记录为每个变量分配的寄存器
        map<RegId, Instruction *> RVALUE;   // 记录当前每个寄存器存储的变量
        struct value_info
        {
            bool InReg = false;
            RegId reg_num = RegManager::NONE; // 变量当前所在的寄存器
            int stack_offset = 0;             // 变量当前所在位置的栈偏移
        };
        map<Instruction *, value_info> AVALUE; // 记录当前每个变量的信息
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
        // 线性扫描:释放已到期区间
        int ExpireOldIntervals(Instruction *i)
        {
            int expired_intervals = 0;
            for (auto interval : active)
            {
                if (interval->GetEnd() > i->GetStart())
                    break;
                else
                {
                    // active.erase(interval);
                    expired_intervals++;
                    RegId freereg = Register[interval];
                    if (freereg >= RegManager::R0 && freereg <= RegManager::R10)
                        regm.AddFreeReg(freereg);
                    else if (freereg >= RegManager::S0 && freereg <= RegManager::S15)
                        regm.AddFreesReg(freereg);
                }
            }
            for (int i = 0; i < expired_intervals; i++)
            {
                auto iter = active.begin();
                active.erase(iter);
            }
            return expired_intervals;
        }
        // 线性扫描:区间溢出
        void SpillAtIntervals(Instruction *i)
        {
            auto iter = active.end();
            auto spill = *(--iter);
            // 溢出spill
            if (spill->GetEnd() >= i->GetEnd())
            {
                Register[i] = Register[spill];
                Register[spill] = RegManager::NONE;
                spill->setLocation(top_offset);
                top_offset -= 4;
                active.erase(spill);
                active.insert(i);
            }
            // 溢出i
            else
            {
                Register[i] = RegManager::NONE;
                i->setLocation(top_offset);
                top_offset -= 4;
            }
        }
        // 清空函数记录
        void clearFunctionRecord(Function *func)
        {
            localVarStOffset.clear();
            paramsStOffset.clear();
            retValueStOffset = 0;
            bb_labels.clear();
            curFunc = func;
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
            active.clear();
            regm.ResetRegPool();
            Register.clear();
            RVALUE.clear();
            AVALUE.clear();
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
