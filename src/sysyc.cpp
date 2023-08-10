/*
 * @Description:
 * @Author: 王清哲
 * @Date: 2023-07-29 17:14:14
 * @LastEditTime: 2023-08-10 18:35:13
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
  LoadCut ldCut(moduleIR);
  auto ldCutIR = ldCut.Run();

  // if (strcmp(argv[2], "ir1") == 0)
  //   ldCutIR->print(cout);
  DCE dce(ldCutIR);
  auto DceIR = dce.Run();
  // if (strcmp(argv[2], "ir2") == 0)
  // DceIR->print(cout);

  if (genir)
  {
    DceIR->print(cout);
    // DceIR->print(cout);
    // moduleIR->Print_topology(cout);
    // ldCut.print_IN_OUT(cout);
    // if (strcmp(argv[2], "ir2") == 0)
    // ldCutIR->print(cout);
    // else if (strcmp(argv[2], "ir1") == 0)
    // DceIR->print(cout);
    // ldCut.print_KILL_GEN(cout);
    // ldCut.print_IN_OUT(cout);
    // moduleIR->print(cout);
    // Lifetime lifetime(ldCutIR);
    // Module *ir = lifetime.Run();
    // lifetime.print_USE_DEF(cout);
    // lifetime.print_Live_IN_OUT(cout);
    // DCE dce(ir);
    // Module *ir2 = dce.Run();
    return EXIT_SUCCESS;
  }

  // CodeGen codegen(moduleIR);
  CodeGen codegen(DceIR);
  string asmCode = codegen.code_gen();
  cout << asmCode << endl;
  ;

  return EXIT_SUCCESS;
}