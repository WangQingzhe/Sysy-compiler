/*
 * @Description:
 * @Author: 王清哲
 * @Date: 2023-07-29 17:14:14
 * @LastEditTime: 2023-08-11 17:33:08
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
  // only generate SysY IR code
  if (strcmp(argv[2], "ir") == 0)
  {
    cout << "Module\n";
    moduleIR->print(cout);
    return 0;
  }
  LoadCut ldCut(moduleIR);
  auto ldCutIR = ldCut.Run();
  if (strcmp(argv[2], "ir1") == 0)
  {
    cout << "ldCut\n";
    ldCutIR->print(cout);
    return 0;
  }
  CommonExp CmExp(ldCutIR);
  auto CmExpIR = CmExp.Run();
  if (strcmp(argv[2], "ir2") == 0)
  {
    Dom dom(CmExpIR);
    dom.Run();
    dom.PRINT_DOM(cout);
    cout << "CmExp\n";
    CmExpIR->print(cout);
    return EXIT_SUCCESS;
  }
  // DCE dce(CmExpIR);
  // auto DceIR = dce.Run();
  // if (strcmp(argv[2], "ir3") == 0)
  // {
  //   cout << "Dce\n";
  //   DceIR->print(cout);
  //   return EXIT_SUCCESS;
  // }
  LoadCut ldCut2(CmExpIR);
  auto ldCutIR2 = ldCut2.Run();
  if (strcmp(argv[2], "ir3") == 0)
  {
    cout << "ldCut2\n";
    ldCutIR2->print(cout);
    return EXIT_SUCCESS;
  }
  if (genir)
  {
    // cout << "dce\n";
    // DceIR->print(cout);
    return EXIT_SUCCESS;
  }

  CodeGen codegen(ldCutIR2);
  string asmCode = codegen.code_gen();
  cout << asmCode << endl;
  ;

  return EXIT_SUCCESS;
}