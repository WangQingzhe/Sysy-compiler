#include "ASTPrinter.h"
#include "tree/ParseTreeWalker.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
using namespace std;
using namespace antlr4;
#include "SysYFormatter.h"
#include "SysYLexer.h"
#include "SysYParser.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    cerr << "Usage: " << argv[0] << "inputfile\n";
    return EXIT_FAILURE;
  }
  ifstream fin(argv[1]);
  if (not fin) {
    cerr << "Failed to open file " << argv[1];
    return EXIT_FAILURE;
  }
  ANTLRInputStream input(fin);
  SysYLexer lexer(&input);
  CommonTokenStream tokens(&lexer);
  SysYParser parser(&tokens);
  auto module = parser.module();
  // tree::ParseTreeWalker walker;
  // SysYASTPrinter printer(cout);
  // cout << module->getText() << '\n';
  // walker.walk(&printer, module);
  SysYFormatter formatter(cout);
  formatter.visitModule(module);

  return EXIT_SUCCESS;
}