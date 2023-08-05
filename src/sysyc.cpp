/*
 * @Description:
 * @Author: 王清哲
 * @Date: 2023-07-29 17:14:14
 * @LastEditTime: 2023-08-06 01:43:15
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
  if (genir)
  {
    moduleIR->Print_topology(cout);
    ldCut.print_IN_OUT(cout);
    ldCutIR->print(cout);
    // ldCut.print_KILL_GEN(cout);
    // ldCut.print_IN_OUT(cout);
    moduleIR->print(cout);
    return EXIT_SUCCESS;
  }

  // CodeGen codegen(moduleIR);
  CodeGen codegen(ldCutIR);
  string asmCode = codegen.code_gen();
  cout << asmCode << endl;
  ;

  return EXIT_SUCCESS;
}