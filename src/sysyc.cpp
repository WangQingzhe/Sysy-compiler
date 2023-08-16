/*
 * @Description:
 * @Author: 王清哲
 * @Date: 2023-07-29 17:14:14
 * @LastEditTime: 2023-08-16 20:30:44
 * @LastEditors: 王清哲
 */
#include "tree/ParseTreeWalker.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
using namespace std;
#include "SysYLexer.h"
#include "SysYParser.h"
using namespace antlr4;
// #include "SysYFormatter.h"
#include "SysYIRGenerator.h"
#include "backend/codegen.hpp"
using namespace sysy;
using backend::CodeGen;

int main(int argc, char **argv)
{
    if (argc > 3)
    {
        cerr << "Usage: " << argv[0] << "inputfile [ir]\n";
        return EXIT_FAILURE;
    }
    bool genir = false;
    if (argc > 2)
    {
        genir = true;
    }
    ifstream fin(argv[1]);
    if (not fin)
    {
        cerr << "Failed to open file " << argv[1];
        return EXIT_FAILURE;
    }
    ANTLRInputStream input(fin);
    SysYLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    SysYParser parser(&tokens);
    auto moduleAST = parser.module();

    SysYIRGenerator generator;
    generator.visitModule(moduleAST);
    auto moduleIR = generator.get();
    Dom dom(moduleIR);
    auto DOM = dom.Run();
    Loop loop(DOM);
    auto loopIR = loop.Run();
    // loop.PRINT_ACCESS(cout);
    // Inline Inl(moduleIR);
    // auto InlIR = Inl.Run();
    // LoadCut ldCut(InlIR);
    // auto ldCutIR = ldCut.Run();
    // CommonExp CmExp(ldCutIR);
    // auto CmExpIR = CmExp.Run();
    // ConstSpread ConSp(CmExpIR);
    // auto ConSpIR = ConSp.Run();
    // InstrCombine InstrCb(ConSpIR);
    // auto InstrCbIR = InstrCb.Run();
    // auto func = InstrCbIR->getFunction("main");
    // auto bblist = func->getBasicBlocks();
    // for (auto &iter : bblist)
    // {
    //     auto bb = iter.get();
    //     cout << "***" << bb->getName() << "***\n";
    //     cout << "pres\n";
    //     for (auto p : bb->getPredecessors())
    //         cout << p->getName() << " ";
    //     cout << "\nsucs\n";
    //     for (auto s : bb->getSuccessors())
    //         cout << s->getName() << " ";
    //     cout << "\n";
    // }
    // DCE dce(InstrCbIR);
    // auto DceIR = dce.Run();
    // LoadCut ldCut2(DceIR);
    // auto ldCutIR2 = ldCut2.Run();
    // CommonExp CmExp2(ldCutIR2);
    // auto CmExpIR2 = CmExp2.Run();
    // ConstSpread ConSp2(CmExpIR2);
    // auto ConSpIR2 = ConSp2.Run();
    // DCE dce2(ConSpIR2);
    // auto DceIR2 = dce2.Run();
    // auto functions = loopIR->getFunctions();
    // int i = 0;
    // // 找出所有回边，<b,a>为由b指向a的边。
    // vector<pair<BasicBlock *, BasicBlock *>> back_edge;
    // for (auto iter = functions->begin(); iter != functions->end(); iter++, i++)
    // {
    //     cout << iter->first << ":" << endl;
    //     for (auto loop : iter->second->getLoops())
    //     {
    //         for (auto l : loop)
    //         {
    //             cout << l->getName() << " ";
    //         }
    //         cout << endl;
    //     }
    // }

    if (genir)
    {
        if (strcmp(argv[2], "ir") == 0)
        {
            cout << "Dce2\n";
            moduleIR->print(cout);
        }
        else if (strcmp(argv[2], "ir1") == 0)
        {
            cout << "module\n";
            moduleIR->print(cout);
        }
        // else if (strcmp(argv[2], "ir1") == 0)
        // {
        //     cout << "DCE2\n";
        //     DceIR2->print(cout);
        // }
        // else if (strcmp(argv[2], "ir1") == 0)
        // {
        //     cout << "ldCut\n";
        //     ldCutIR->print(cout);
        // }
        // else if (strcmp(argv[2], "ir2") == 0)
        // {
        //     cout << "CmExp\n";
        //     CmExpIR->print(cout);
        // }
        // else if (strcmp(argv[2], "ir3") == 0)
        // {
        //     cout << "ldCut2\n";
        //     ldCutIR2->print(cout);
        // }
        // else if (strcmp(argv[2], "ir4") == 0)
        // {
        //     cout << "ConSp\n";
        //     ConSpIR->print(cout);
        // }
        // else if (strcmp(argv[2], "ir5") == 0)
        // {
        //     cout << "CmExp2\n";
        //     CmExpIR2->print(cout);
        // }
        // else if (strcmp(argv[2], "ir6") == 0)
        // {
        //     cout << "Dce\n";
        //     DceIR->print(cout);
        // }
        return EXIT_SUCCESS;
    }

    CodeGen codegen(moduleIR);
    string asmCode = codegen.code_gen();
    cout << asmCode << endl;
    ;

    return EXIT_SUCCESS;
}