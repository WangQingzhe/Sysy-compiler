
// Generated from SysY.g4 by ANTLR 4.12.0


#include "SysYVisitor.h"

#include "SysYParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct SysYParserStaticData final {
  SysYParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  SysYParserStaticData(const SysYParserStaticData&) = delete;
  SysYParserStaticData(SysYParserStaticData&&) = delete;
  SysYParserStaticData& operator=(const SysYParserStaticData&) = delete;
  SysYParserStaticData& operator=(SysYParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag sysyParserOnceFlag;
SysYParserStaticData *sysyParserStaticData = nullptr;

void sysyParserInitialize() {
  assert(sysyParserStaticData == nullptr);
  auto staticData = std::make_unique<SysYParserStaticData>(
    std::vector<std::string>{
      "module", "decl", "btype", "varDef", "initValue", "func", "funcType", 
      "funcFParams", "funcFParam", "blockStmt", "blockItem", "stmt", "assignStmt", 
      "expStmt", "ifStmt", "whileStmt", "breakStmt", "continueStmt", "returnStmt", 
      "emptyStmt", "exp", "call", "lValue", "number", "string", "funcRParams"
    },
    std::vector<std::string>{
      "", "'const'", "'int'", "'float'", "'void'", "'if'", "'else'", "'while'", 
      "'break'", "'continue'", "'return'", "'='", "'+'", "'-'", "'*'", "'/'", 
      "'%'", "'<'", "'>'", "'<='", "'>='", "'=='", "'!='", "'&&'", "'||'", 
      "'!'", "'('", "')'", "'['", "']'", "'{'", "'}'", "','", "';'"
    },
    std::vector<std::string>{
      "", "CONST", "INT", "FLOAT", "VOID", "IF", "ELSE", "WHILE", "BREAK", 
      "CONTINUE", "RETURN", "ASSIGN", "ADD", "SUB", "MUL", "DIV", "MODULO", 
      "LT", "GT", "LE", "GE", "EQ", "NE", "AND", "OR", "NOT", "LPAREN", 
      "RPAREN", "LBRACKET", "RBRACKET", "LBRACE", "RBRACE", "COMMA", "SEMICOLON", 
      "ID", "ILITERAL", "FLITERAL", "STRING", "WS", "LINECOMMENT", "BLOCKCOMMENT"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,40,254,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,1,0,1,0,4,0,55,8,0,11,0,12,
  	0,56,1,1,3,1,60,8,1,1,1,1,1,1,1,1,1,5,1,66,8,1,10,1,12,1,69,9,1,1,1,1,
  	1,1,2,1,2,1,3,1,3,1,3,3,3,78,8,3,1,4,1,4,1,4,1,4,1,4,5,4,85,8,4,10,4,
  	12,4,88,9,4,3,4,90,8,4,3,4,92,8,4,1,5,1,5,1,5,1,5,3,5,98,8,5,1,5,1,5,
  	1,5,1,6,1,6,1,7,1,7,1,7,5,7,108,8,7,10,7,12,7,111,9,7,1,8,1,8,1,8,1,8,
  	1,8,1,8,1,8,1,8,5,8,121,8,8,10,8,12,8,124,9,8,3,8,126,8,8,1,9,1,9,5,9,
  	130,8,9,10,9,12,9,133,9,9,1,9,1,9,1,10,1,10,3,10,139,8,10,1,11,1,11,1,
  	11,1,11,1,11,1,11,1,11,1,11,1,11,3,11,150,8,11,1,12,1,12,1,12,1,12,1,
  	12,1,13,1,13,1,13,1,14,1,14,1,14,1,14,1,14,1,14,1,14,3,14,167,8,14,1,
  	15,1,15,1,15,1,15,1,15,1,15,1,16,1,16,1,16,1,17,1,17,1,17,1,18,1,18,3,
  	18,183,8,18,1,18,1,18,1,19,1,19,1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,
  	20,1,20,1,20,1,20,3,20,200,8,20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,
  	20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,5,20,220,8,20,10,
  	20,12,20,223,9,20,1,21,1,21,1,21,3,21,228,8,21,1,21,1,21,1,22,1,22,1,
  	22,1,22,1,22,5,22,237,8,22,10,22,12,22,240,9,22,1,23,1,23,1,24,1,24,1,
  	25,1,25,1,25,5,25,249,8,25,10,25,12,25,252,9,25,1,25,0,1,40,26,0,2,4,
  	6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,0,
  	8,1,0,2,3,1,0,2,4,2,0,12,13,25,25,1,0,14,16,1,0,12,13,1,0,17,20,1,0,21,
  	22,1,0,35,36,265,0,54,1,0,0,0,2,59,1,0,0,0,4,72,1,0,0,0,6,74,1,0,0,0,
  	8,91,1,0,0,0,10,93,1,0,0,0,12,102,1,0,0,0,14,104,1,0,0,0,16,112,1,0,0,
  	0,18,127,1,0,0,0,20,138,1,0,0,0,22,149,1,0,0,0,24,151,1,0,0,0,26,156,
  	1,0,0,0,28,159,1,0,0,0,30,168,1,0,0,0,32,174,1,0,0,0,34,177,1,0,0,0,36,
  	180,1,0,0,0,38,186,1,0,0,0,40,199,1,0,0,0,42,224,1,0,0,0,44,231,1,0,0,
  	0,46,241,1,0,0,0,48,243,1,0,0,0,50,245,1,0,0,0,52,55,3,2,1,0,53,55,3,
  	10,5,0,54,52,1,0,0,0,54,53,1,0,0,0,55,56,1,0,0,0,56,54,1,0,0,0,56,57,
  	1,0,0,0,57,1,1,0,0,0,58,60,5,1,0,0,59,58,1,0,0,0,59,60,1,0,0,0,60,61,
  	1,0,0,0,61,62,3,4,2,0,62,67,3,6,3,0,63,64,5,32,0,0,64,66,3,6,3,0,65,63,
  	1,0,0,0,66,69,1,0,0,0,67,65,1,0,0,0,67,68,1,0,0,0,68,70,1,0,0,0,69,67,
  	1,0,0,0,70,71,5,33,0,0,71,3,1,0,0,0,72,73,7,0,0,0,73,5,1,0,0,0,74,77,
  	3,44,22,0,75,76,5,11,0,0,76,78,3,8,4,0,77,75,1,0,0,0,77,78,1,0,0,0,78,
  	7,1,0,0,0,79,92,3,40,20,0,80,89,5,30,0,0,81,86,3,8,4,0,82,83,5,32,0,0,
  	83,85,3,8,4,0,84,82,1,0,0,0,85,88,1,0,0,0,86,84,1,0,0,0,86,87,1,0,0,0,
  	87,90,1,0,0,0,88,86,1,0,0,0,89,81,1,0,0,0,89,90,1,0,0,0,90,92,1,0,0,0,
  	91,79,1,0,0,0,91,80,1,0,0,0,92,9,1,0,0,0,93,94,3,12,6,0,94,95,5,34,0,
  	0,95,97,5,26,0,0,96,98,3,14,7,0,97,96,1,0,0,0,97,98,1,0,0,0,98,99,1,0,
  	0,0,99,100,5,27,0,0,100,101,3,18,9,0,101,11,1,0,0,0,102,103,7,1,0,0,103,
  	13,1,0,0,0,104,109,3,16,8,0,105,106,5,32,0,0,106,108,3,16,8,0,107,105,
  	1,0,0,0,108,111,1,0,0,0,109,107,1,0,0,0,109,110,1,0,0,0,110,15,1,0,0,
  	0,111,109,1,0,0,0,112,113,3,4,2,0,113,125,5,34,0,0,114,115,5,28,0,0,115,
  	122,5,29,0,0,116,117,5,28,0,0,117,118,3,40,20,0,118,119,5,29,0,0,119,
  	121,1,0,0,0,120,116,1,0,0,0,121,124,1,0,0,0,122,120,1,0,0,0,122,123,1,
  	0,0,0,123,126,1,0,0,0,124,122,1,0,0,0,125,114,1,0,0,0,125,126,1,0,0,0,
  	126,17,1,0,0,0,127,131,5,30,0,0,128,130,3,20,10,0,129,128,1,0,0,0,130,
  	133,1,0,0,0,131,129,1,0,0,0,131,132,1,0,0,0,132,134,1,0,0,0,133,131,1,
  	0,0,0,134,135,5,31,0,0,135,19,1,0,0,0,136,139,3,2,1,0,137,139,3,22,11,
  	0,138,136,1,0,0,0,138,137,1,0,0,0,139,21,1,0,0,0,140,150,3,24,12,0,141,
  	150,3,26,13,0,142,150,3,28,14,0,143,150,3,30,15,0,144,150,3,32,16,0,145,
  	150,3,34,17,0,146,150,3,36,18,0,147,150,3,18,9,0,148,150,3,38,19,0,149,
  	140,1,0,0,0,149,141,1,0,0,0,149,142,1,0,0,0,149,143,1,0,0,0,149,144,1,
  	0,0,0,149,145,1,0,0,0,149,146,1,0,0,0,149,147,1,0,0,0,149,148,1,0,0,0,
  	150,23,1,0,0,0,151,152,3,44,22,0,152,153,5,11,0,0,153,154,3,40,20,0,154,
  	155,5,33,0,0,155,25,1,0,0,0,156,157,3,40,20,0,157,158,5,33,0,0,158,27,
  	1,0,0,0,159,160,5,5,0,0,160,161,5,26,0,0,161,162,3,40,20,0,162,163,5,
  	27,0,0,163,166,3,22,11,0,164,165,5,6,0,0,165,167,3,22,11,0,166,164,1,
  	0,0,0,166,167,1,0,0,0,167,29,1,0,0,0,168,169,5,7,0,0,169,170,5,26,0,0,
  	170,171,3,40,20,0,171,172,5,27,0,0,172,173,3,22,11,0,173,31,1,0,0,0,174,
  	175,5,8,0,0,175,176,5,33,0,0,176,33,1,0,0,0,177,178,5,9,0,0,178,179,5,
  	33,0,0,179,35,1,0,0,0,180,182,5,10,0,0,181,183,3,40,20,0,182,181,1,0,
  	0,0,182,183,1,0,0,0,183,184,1,0,0,0,184,185,5,33,0,0,185,37,1,0,0,0,186,
  	187,5,33,0,0,187,39,1,0,0,0,188,189,6,20,-1,0,189,190,5,26,0,0,190,191,
  	3,40,20,0,191,192,5,27,0,0,192,200,1,0,0,0,193,200,3,44,22,0,194,200,
  	3,46,23,0,195,200,3,48,24,0,196,200,3,42,21,0,197,198,7,2,0,0,198,200,
  	3,40,20,7,199,188,1,0,0,0,199,193,1,0,0,0,199,194,1,0,0,0,199,195,1,0,
  	0,0,199,196,1,0,0,0,199,197,1,0,0,0,200,221,1,0,0,0,201,202,10,6,0,0,
  	202,203,7,3,0,0,203,220,3,40,20,7,204,205,10,5,0,0,205,206,7,4,0,0,206,
  	220,3,40,20,6,207,208,10,4,0,0,208,209,7,5,0,0,209,220,3,40,20,5,210,
  	211,10,3,0,0,211,212,7,6,0,0,212,220,3,40,20,4,213,214,10,2,0,0,214,215,
  	5,23,0,0,215,220,3,40,20,3,216,217,10,1,0,0,217,218,5,24,0,0,218,220,
  	3,40,20,2,219,201,1,0,0,0,219,204,1,0,0,0,219,207,1,0,0,0,219,210,1,0,
  	0,0,219,213,1,0,0,0,219,216,1,0,0,0,220,223,1,0,0,0,221,219,1,0,0,0,221,
  	222,1,0,0,0,222,41,1,0,0,0,223,221,1,0,0,0,224,225,5,34,0,0,225,227,5,
  	26,0,0,226,228,3,50,25,0,227,226,1,0,0,0,227,228,1,0,0,0,228,229,1,0,
  	0,0,229,230,5,27,0,0,230,43,1,0,0,0,231,238,5,34,0,0,232,233,5,28,0,0,
  	233,234,3,40,20,0,234,235,5,29,0,0,235,237,1,0,0,0,236,232,1,0,0,0,237,
  	240,1,0,0,0,238,236,1,0,0,0,238,239,1,0,0,0,239,45,1,0,0,0,240,238,1,
  	0,0,0,241,242,7,7,0,0,242,47,1,0,0,0,243,244,5,37,0,0,244,49,1,0,0,0,
  	245,250,3,40,20,0,246,247,5,32,0,0,247,249,3,40,20,0,248,246,1,0,0,0,
  	249,252,1,0,0,0,250,248,1,0,0,0,250,251,1,0,0,0,251,51,1,0,0,0,252,250,
  	1,0,0,0,23,54,56,59,67,77,86,89,91,97,109,122,125,131,138,149,166,182,
  	199,219,221,227,238,250
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  sysyParserStaticData = staticData.release();
}

}

SysYParser::SysYParser(TokenStream *input) : SysYParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

SysYParser::SysYParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  SysYParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *sysyParserStaticData->atn, sysyParserStaticData->decisionToDFA, sysyParserStaticData->sharedContextCache, options);
}

SysYParser::~SysYParser() {
  delete _interpreter;
}

const atn::ATN& SysYParser::getATN() const {
  return *sysyParserStaticData->atn;
}

std::string SysYParser::getGrammarFileName() const {
  return "SysY.g4";
}

const std::vector<std::string>& SysYParser::getRuleNames() const {
  return sysyParserStaticData->ruleNames;
}

const dfa::Vocabulary& SysYParser::getVocabulary() const {
  return sysyParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView SysYParser::getSerializedATN() const {
  return sysyParserStaticData->serializedATN;
}


//----------------- ModuleContext ------------------------------------------------------------------

SysYParser::ModuleContext::ModuleContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParser::DeclContext *> SysYParser::ModuleContext::decl() {
  return getRuleContexts<SysYParser::DeclContext>();
}

SysYParser::DeclContext* SysYParser::ModuleContext::decl(size_t i) {
  return getRuleContext<SysYParser::DeclContext>(i);
}

std::vector<SysYParser::FuncContext *> SysYParser::ModuleContext::func() {
  return getRuleContexts<SysYParser::FuncContext>();
}

SysYParser::FuncContext* SysYParser::ModuleContext::func(size_t i) {
  return getRuleContext<SysYParser::FuncContext>(i);
}


size_t SysYParser::ModuleContext::getRuleIndex() const {
  return SysYParser::RuleModule;
}


std::any SysYParser::ModuleContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitModule(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::ModuleContext* SysYParser::module() {
  ModuleContext *_localctx = _tracker.createInstance<ModuleContext>(_ctx, getState());
  enterRule(_localctx, 0, SysYParser::RuleModule);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(54); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(54);
      _errHandler->sync(this);
      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 0, _ctx)) {
      case 1: {
        setState(52);
        decl();
        break;
      }

      case 2: {
        setState(53);
        func();
        break;
      }

      default:
        break;
      }
      setState(56); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 30) != 0));
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DeclContext ------------------------------------------------------------------

SysYParser::DeclContext::DeclContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::BtypeContext* SysYParser::DeclContext::btype() {
  return getRuleContext<SysYParser::BtypeContext>(0);
}

std::vector<SysYParser::VarDefContext *> SysYParser::DeclContext::varDef() {
  return getRuleContexts<SysYParser::VarDefContext>();
}

SysYParser::VarDefContext* SysYParser::DeclContext::varDef(size_t i) {
  return getRuleContext<SysYParser::VarDefContext>(i);
}

tree::TerminalNode* SysYParser::DeclContext::SEMICOLON() {
  return getToken(SysYParser::SEMICOLON, 0);
}

tree::TerminalNode* SysYParser::DeclContext::CONST() {
  return getToken(SysYParser::CONST, 0);
}

std::vector<tree::TerminalNode *> SysYParser::DeclContext::COMMA() {
  return getTokens(SysYParser::COMMA);
}

tree::TerminalNode* SysYParser::DeclContext::COMMA(size_t i) {
  return getToken(SysYParser::COMMA, i);
}


size_t SysYParser::DeclContext::getRuleIndex() const {
  return SysYParser::RuleDecl;
}


std::any SysYParser::DeclContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitDecl(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::DeclContext* SysYParser::decl() {
  DeclContext *_localctx = _tracker.createInstance<DeclContext>(_ctx, getState());
  enterRule(_localctx, 2, SysYParser::RuleDecl);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(59);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == SysYParser::CONST) {
      setState(58);
      match(SysYParser::CONST);
    }
    setState(61);
    btype();
    setState(62);
    varDef();
    setState(67);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParser::COMMA) {
      setState(63);
      match(SysYParser::COMMA);
      setState(64);
      varDef();
      setState(69);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(70);
    match(SysYParser::SEMICOLON);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BtypeContext ------------------------------------------------------------------

SysYParser::BtypeContext::BtypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::BtypeContext::INT() {
  return getToken(SysYParser::INT, 0);
}

tree::TerminalNode* SysYParser::BtypeContext::FLOAT() {
  return getToken(SysYParser::FLOAT, 0);
}


size_t SysYParser::BtypeContext::getRuleIndex() const {
  return SysYParser::RuleBtype;
}


std::any SysYParser::BtypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitBtype(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::BtypeContext* SysYParser::btype() {
  BtypeContext *_localctx = _tracker.createInstance<BtypeContext>(_ctx, getState());
  enterRule(_localctx, 4, SysYParser::RuleBtype);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(72);
    _la = _input->LA(1);
    if (!(_la == SysYParser::INT

    || _la == SysYParser::FLOAT)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VarDefContext ------------------------------------------------------------------

SysYParser::VarDefContext::VarDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::LValueContext* SysYParser::VarDefContext::lValue() {
  return getRuleContext<SysYParser::LValueContext>(0);
}

tree::TerminalNode* SysYParser::VarDefContext::ASSIGN() {
  return getToken(SysYParser::ASSIGN, 0);
}

SysYParser::InitValueContext* SysYParser::VarDefContext::initValue() {
  return getRuleContext<SysYParser::InitValueContext>(0);
}


size_t SysYParser::VarDefContext::getRuleIndex() const {
  return SysYParser::RuleVarDef;
}


std::any SysYParser::VarDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitVarDef(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::VarDefContext* SysYParser::varDef() {
  VarDefContext *_localctx = _tracker.createInstance<VarDefContext>(_ctx, getState());
  enterRule(_localctx, 6, SysYParser::RuleVarDef);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(74);
    lValue();
    setState(77);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == SysYParser::ASSIGN) {
      setState(75);
      match(SysYParser::ASSIGN);
      setState(76);
      initValue();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- InitValueContext ------------------------------------------------------------------

SysYParser::InitValueContext::InitValueContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::ExpContext* SysYParser::InitValueContext::exp() {
  return getRuleContext<SysYParser::ExpContext>(0);
}

tree::TerminalNode* SysYParser::InitValueContext::LBRACE() {
  return getToken(SysYParser::LBRACE, 0);
}

std::vector<SysYParser::InitValueContext *> SysYParser::InitValueContext::initValue() {
  return getRuleContexts<SysYParser::InitValueContext>();
}

SysYParser::InitValueContext* SysYParser::InitValueContext::initValue(size_t i) {
  return getRuleContext<SysYParser::InitValueContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::InitValueContext::COMMA() {
  return getTokens(SysYParser::COMMA);
}

tree::TerminalNode* SysYParser::InitValueContext::COMMA(size_t i) {
  return getToken(SysYParser::COMMA, i);
}


size_t SysYParser::InitValueContext::getRuleIndex() const {
  return SysYParser::RuleInitValue;
}


std::any SysYParser::InitValueContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitInitValue(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::InitValueContext* SysYParser::initValue() {
  InitValueContext *_localctx = _tracker.createInstance<InitValueContext>(_ctx, getState());
  enterRule(_localctx, 8, SysYParser::RuleInitValue);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    size_t alt;
    setState(91);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SysYParser::ADD:
      case SysYParser::SUB:
      case SysYParser::NOT:
      case SysYParser::LPAREN:
      case SysYParser::ID:
      case SysYParser::ILITERAL:
      case SysYParser::FLITERAL:
      case SysYParser::STRING: {
        enterOuterAlt(_localctx, 1);
        setState(79);
        exp(0);
        break;
      }

      case SysYParser::LBRACE: {
        enterOuterAlt(_localctx, 2);
        setState(80);
        match(SysYParser::LBRACE);
        setState(89);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 258872455168) != 0)) {
          setState(81);
          initValue();
          setState(86);
          _errHandler->sync(this);
          alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 5, _ctx);
          while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
            if (alt == 1) {
              setState(82);
              match(SysYParser::COMMA);
              setState(83);
              initValue(); 
            }
            setState(88);
            _errHandler->sync(this);
            alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 5, _ctx);
          }
        }
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FuncContext ------------------------------------------------------------------

SysYParser::FuncContext::FuncContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::FuncTypeContext* SysYParser::FuncContext::funcType() {
  return getRuleContext<SysYParser::FuncTypeContext>(0);
}

tree::TerminalNode* SysYParser::FuncContext::ID() {
  return getToken(SysYParser::ID, 0);
}

tree::TerminalNode* SysYParser::FuncContext::LPAREN() {
  return getToken(SysYParser::LPAREN, 0);
}

tree::TerminalNode* SysYParser::FuncContext::RPAREN() {
  return getToken(SysYParser::RPAREN, 0);
}

SysYParser::BlockStmtContext* SysYParser::FuncContext::blockStmt() {
  return getRuleContext<SysYParser::BlockStmtContext>(0);
}

SysYParser::FuncFParamsContext* SysYParser::FuncContext::funcFParams() {
  return getRuleContext<SysYParser::FuncFParamsContext>(0);
}


size_t SysYParser::FuncContext::getRuleIndex() const {
  return SysYParser::RuleFunc;
}


std::any SysYParser::FuncContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitFunc(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::FuncContext* SysYParser::func() {
  FuncContext *_localctx = _tracker.createInstance<FuncContext>(_ctx, getState());
  enterRule(_localctx, 10, SysYParser::RuleFunc);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(93);
    funcType();
    setState(94);
    match(SysYParser::ID);
    setState(95);
    match(SysYParser::LPAREN);
    setState(97);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == SysYParser::INT

    || _la == SysYParser::FLOAT) {
      setState(96);
      funcFParams();
    }
    setState(99);
    match(SysYParser::RPAREN);
    setState(100);
    blockStmt();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FuncTypeContext ------------------------------------------------------------------

SysYParser::FuncTypeContext::FuncTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::FuncTypeContext::VOID() {
  return getToken(SysYParser::VOID, 0);
}

tree::TerminalNode* SysYParser::FuncTypeContext::INT() {
  return getToken(SysYParser::INT, 0);
}

tree::TerminalNode* SysYParser::FuncTypeContext::FLOAT() {
  return getToken(SysYParser::FLOAT, 0);
}


size_t SysYParser::FuncTypeContext::getRuleIndex() const {
  return SysYParser::RuleFuncType;
}


std::any SysYParser::FuncTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitFuncType(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::FuncTypeContext* SysYParser::funcType() {
  FuncTypeContext *_localctx = _tracker.createInstance<FuncTypeContext>(_ctx, getState());
  enterRule(_localctx, 12, SysYParser::RuleFuncType);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(102);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 28) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FuncFParamsContext ------------------------------------------------------------------

SysYParser::FuncFParamsContext::FuncFParamsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParser::FuncFParamContext *> SysYParser::FuncFParamsContext::funcFParam() {
  return getRuleContexts<SysYParser::FuncFParamContext>();
}

SysYParser::FuncFParamContext* SysYParser::FuncFParamsContext::funcFParam(size_t i) {
  return getRuleContext<SysYParser::FuncFParamContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::FuncFParamsContext::COMMA() {
  return getTokens(SysYParser::COMMA);
}

tree::TerminalNode* SysYParser::FuncFParamsContext::COMMA(size_t i) {
  return getToken(SysYParser::COMMA, i);
}


size_t SysYParser::FuncFParamsContext::getRuleIndex() const {
  return SysYParser::RuleFuncFParams;
}


std::any SysYParser::FuncFParamsContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitFuncFParams(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::FuncFParamsContext* SysYParser::funcFParams() {
  FuncFParamsContext *_localctx = _tracker.createInstance<FuncFParamsContext>(_ctx, getState());
  enterRule(_localctx, 14, SysYParser::RuleFuncFParams);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(104);
    funcFParam();
    setState(109);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParser::COMMA) {
      setState(105);
      match(SysYParser::COMMA);
      setState(106);
      funcFParam();
      setState(111);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FuncFParamContext ------------------------------------------------------------------

SysYParser::FuncFParamContext::FuncFParamContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::BtypeContext* SysYParser::FuncFParamContext::btype() {
  return getRuleContext<SysYParser::BtypeContext>(0);
}

tree::TerminalNode* SysYParser::FuncFParamContext::ID() {
  return getToken(SysYParser::ID, 0);
}

std::vector<tree::TerminalNode *> SysYParser::FuncFParamContext::LBRACKET() {
  return getTokens(SysYParser::LBRACKET);
}

tree::TerminalNode* SysYParser::FuncFParamContext::LBRACKET(size_t i) {
  return getToken(SysYParser::LBRACKET, i);
}

std::vector<tree::TerminalNode *> SysYParser::FuncFParamContext::RBRACKET() {
  return getTokens(SysYParser::RBRACKET);
}

tree::TerminalNode* SysYParser::FuncFParamContext::RBRACKET(size_t i) {
  return getToken(SysYParser::RBRACKET, i);
}

std::vector<SysYParser::ExpContext *> SysYParser::FuncFParamContext::exp() {
  return getRuleContexts<SysYParser::ExpContext>();
}

SysYParser::ExpContext* SysYParser::FuncFParamContext::exp(size_t i) {
  return getRuleContext<SysYParser::ExpContext>(i);
}


size_t SysYParser::FuncFParamContext::getRuleIndex() const {
  return SysYParser::RuleFuncFParam;
}


std::any SysYParser::FuncFParamContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitFuncFParam(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::FuncFParamContext* SysYParser::funcFParam() {
  FuncFParamContext *_localctx = _tracker.createInstance<FuncFParamContext>(_ctx, getState());
  enterRule(_localctx, 16, SysYParser::RuleFuncFParam);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(112);
    btype();
    setState(113);
    match(SysYParser::ID);
    setState(125);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == SysYParser::LBRACKET) {
      setState(114);
      match(SysYParser::LBRACKET);
      setState(115);
      match(SysYParser::RBRACKET);
      setState(122);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == SysYParser::LBRACKET) {
        setState(116);
        match(SysYParser::LBRACKET);
        setState(117);
        exp(0);
        setState(118);
        match(SysYParser::RBRACKET);
        setState(124);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BlockStmtContext ------------------------------------------------------------------

SysYParser::BlockStmtContext::BlockStmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::BlockStmtContext::LBRACE() {
  return getToken(SysYParser::LBRACE, 0);
}

tree::TerminalNode* SysYParser::BlockStmtContext::RBRACE() {
  return getToken(SysYParser::RBRACE, 0);
}

std::vector<SysYParser::BlockItemContext *> SysYParser::BlockStmtContext::blockItem() {
  return getRuleContexts<SysYParser::BlockItemContext>();
}

SysYParser::BlockItemContext* SysYParser::BlockStmtContext::blockItem(size_t i) {
  return getRuleContext<SysYParser::BlockItemContext>(i);
}


size_t SysYParser::BlockStmtContext::getRuleIndex() const {
  return SysYParser::RuleBlockStmt;
}


std::any SysYParser::BlockStmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitBlockStmt(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::BlockStmtContext* SysYParser::blockStmt() {
  BlockStmtContext *_localctx = _tracker.createInstance<BlockStmtContext>(_ctx, getState());
  enterRule(_localctx, 18, SysYParser::RuleBlockStmt);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(127);
    match(SysYParser::LBRACE);
    setState(131);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 267462391726) != 0)) {
      setState(128);
      blockItem();
      setState(133);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(134);
    match(SysYParser::RBRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BlockItemContext ------------------------------------------------------------------

SysYParser::BlockItemContext::BlockItemContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::DeclContext* SysYParser::BlockItemContext::decl() {
  return getRuleContext<SysYParser::DeclContext>(0);
}

SysYParser::StmtContext* SysYParser::BlockItemContext::stmt() {
  return getRuleContext<SysYParser::StmtContext>(0);
}


size_t SysYParser::BlockItemContext::getRuleIndex() const {
  return SysYParser::RuleBlockItem;
}


std::any SysYParser::BlockItemContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitBlockItem(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::BlockItemContext* SysYParser::blockItem() {
  BlockItemContext *_localctx = _tracker.createInstance<BlockItemContext>(_ctx, getState());
  enterRule(_localctx, 20, SysYParser::RuleBlockItem);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(138);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SysYParser::CONST:
      case SysYParser::INT:
      case SysYParser::FLOAT: {
        enterOuterAlt(_localctx, 1);
        setState(136);
        decl();
        break;
      }

      case SysYParser::IF:
      case SysYParser::WHILE:
      case SysYParser::BREAK:
      case SysYParser::CONTINUE:
      case SysYParser::RETURN:
      case SysYParser::ADD:
      case SysYParser::SUB:
      case SysYParser::NOT:
      case SysYParser::LPAREN:
      case SysYParser::LBRACE:
      case SysYParser::SEMICOLON:
      case SysYParser::ID:
      case SysYParser::ILITERAL:
      case SysYParser::FLITERAL:
      case SysYParser::STRING: {
        enterOuterAlt(_localctx, 2);
        setState(137);
        stmt();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StmtContext ------------------------------------------------------------------

SysYParser::StmtContext::StmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::AssignStmtContext* SysYParser::StmtContext::assignStmt() {
  return getRuleContext<SysYParser::AssignStmtContext>(0);
}

SysYParser::ExpStmtContext* SysYParser::StmtContext::expStmt() {
  return getRuleContext<SysYParser::ExpStmtContext>(0);
}

SysYParser::IfStmtContext* SysYParser::StmtContext::ifStmt() {
  return getRuleContext<SysYParser::IfStmtContext>(0);
}

SysYParser::WhileStmtContext* SysYParser::StmtContext::whileStmt() {
  return getRuleContext<SysYParser::WhileStmtContext>(0);
}

SysYParser::BreakStmtContext* SysYParser::StmtContext::breakStmt() {
  return getRuleContext<SysYParser::BreakStmtContext>(0);
}

SysYParser::ContinueStmtContext* SysYParser::StmtContext::continueStmt() {
  return getRuleContext<SysYParser::ContinueStmtContext>(0);
}

SysYParser::ReturnStmtContext* SysYParser::StmtContext::returnStmt() {
  return getRuleContext<SysYParser::ReturnStmtContext>(0);
}

SysYParser::BlockStmtContext* SysYParser::StmtContext::blockStmt() {
  return getRuleContext<SysYParser::BlockStmtContext>(0);
}

SysYParser::EmptyStmtContext* SysYParser::StmtContext::emptyStmt() {
  return getRuleContext<SysYParser::EmptyStmtContext>(0);
}


size_t SysYParser::StmtContext::getRuleIndex() const {
  return SysYParser::RuleStmt;
}


std::any SysYParser::StmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitStmt(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::StmtContext* SysYParser::stmt() {
  StmtContext *_localctx = _tracker.createInstance<StmtContext>(_ctx, getState());
  enterRule(_localctx, 22, SysYParser::RuleStmt);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(149);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 14, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(140);
      assignStmt();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(141);
      expStmt();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(142);
      ifStmt();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(143);
      whileStmt();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(144);
      breakStmt();
      break;
    }

    case 6: {
      enterOuterAlt(_localctx, 6);
      setState(145);
      continueStmt();
      break;
    }

    case 7: {
      enterOuterAlt(_localctx, 7);
      setState(146);
      returnStmt();
      break;
    }

    case 8: {
      enterOuterAlt(_localctx, 8);
      setState(147);
      blockStmt();
      break;
    }

    case 9: {
      enterOuterAlt(_localctx, 9);
      setState(148);
      emptyStmt();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AssignStmtContext ------------------------------------------------------------------

SysYParser::AssignStmtContext::AssignStmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::LValueContext* SysYParser::AssignStmtContext::lValue() {
  return getRuleContext<SysYParser::LValueContext>(0);
}

tree::TerminalNode* SysYParser::AssignStmtContext::ASSIGN() {
  return getToken(SysYParser::ASSIGN, 0);
}

SysYParser::ExpContext* SysYParser::AssignStmtContext::exp() {
  return getRuleContext<SysYParser::ExpContext>(0);
}

tree::TerminalNode* SysYParser::AssignStmtContext::SEMICOLON() {
  return getToken(SysYParser::SEMICOLON, 0);
}


size_t SysYParser::AssignStmtContext::getRuleIndex() const {
  return SysYParser::RuleAssignStmt;
}


std::any SysYParser::AssignStmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitAssignStmt(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::AssignStmtContext* SysYParser::assignStmt() {
  AssignStmtContext *_localctx = _tracker.createInstance<AssignStmtContext>(_ctx, getState());
  enterRule(_localctx, 24, SysYParser::RuleAssignStmt);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(151);
    lValue();
    setState(152);
    match(SysYParser::ASSIGN);
    setState(153);
    exp(0);
    setState(154);
    match(SysYParser::SEMICOLON);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExpStmtContext ------------------------------------------------------------------

SysYParser::ExpStmtContext::ExpStmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::ExpContext* SysYParser::ExpStmtContext::exp() {
  return getRuleContext<SysYParser::ExpContext>(0);
}

tree::TerminalNode* SysYParser::ExpStmtContext::SEMICOLON() {
  return getToken(SysYParser::SEMICOLON, 0);
}


size_t SysYParser::ExpStmtContext::getRuleIndex() const {
  return SysYParser::RuleExpStmt;
}


std::any SysYParser::ExpStmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitExpStmt(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::ExpStmtContext* SysYParser::expStmt() {
  ExpStmtContext *_localctx = _tracker.createInstance<ExpStmtContext>(_ctx, getState());
  enterRule(_localctx, 26, SysYParser::RuleExpStmt);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(156);
    exp(0);
    setState(157);
    match(SysYParser::SEMICOLON);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- IfStmtContext ------------------------------------------------------------------

SysYParser::IfStmtContext::IfStmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::IfStmtContext::IF() {
  return getToken(SysYParser::IF, 0);
}

tree::TerminalNode* SysYParser::IfStmtContext::LPAREN() {
  return getToken(SysYParser::LPAREN, 0);
}

SysYParser::ExpContext* SysYParser::IfStmtContext::exp() {
  return getRuleContext<SysYParser::ExpContext>(0);
}

tree::TerminalNode* SysYParser::IfStmtContext::RPAREN() {
  return getToken(SysYParser::RPAREN, 0);
}

std::vector<SysYParser::StmtContext *> SysYParser::IfStmtContext::stmt() {
  return getRuleContexts<SysYParser::StmtContext>();
}

SysYParser::StmtContext* SysYParser::IfStmtContext::stmt(size_t i) {
  return getRuleContext<SysYParser::StmtContext>(i);
}

tree::TerminalNode* SysYParser::IfStmtContext::ELSE() {
  return getToken(SysYParser::ELSE, 0);
}


size_t SysYParser::IfStmtContext::getRuleIndex() const {
  return SysYParser::RuleIfStmt;
}


std::any SysYParser::IfStmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitIfStmt(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::IfStmtContext* SysYParser::ifStmt() {
  IfStmtContext *_localctx = _tracker.createInstance<IfStmtContext>(_ctx, getState());
  enterRule(_localctx, 28, SysYParser::RuleIfStmt);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(159);
    match(SysYParser::IF);
    setState(160);
    match(SysYParser::LPAREN);
    setState(161);
    exp(0);
    setState(162);
    match(SysYParser::RPAREN);
    setState(163);
    stmt();
    setState(166);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 15, _ctx)) {
    case 1: {
      setState(164);
      match(SysYParser::ELSE);
      setState(165);
      stmt();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- WhileStmtContext ------------------------------------------------------------------

SysYParser::WhileStmtContext::WhileStmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::WhileStmtContext::WHILE() {
  return getToken(SysYParser::WHILE, 0);
}

tree::TerminalNode* SysYParser::WhileStmtContext::LPAREN() {
  return getToken(SysYParser::LPAREN, 0);
}

SysYParser::ExpContext* SysYParser::WhileStmtContext::exp() {
  return getRuleContext<SysYParser::ExpContext>(0);
}

tree::TerminalNode* SysYParser::WhileStmtContext::RPAREN() {
  return getToken(SysYParser::RPAREN, 0);
}

SysYParser::StmtContext* SysYParser::WhileStmtContext::stmt() {
  return getRuleContext<SysYParser::StmtContext>(0);
}


size_t SysYParser::WhileStmtContext::getRuleIndex() const {
  return SysYParser::RuleWhileStmt;
}


std::any SysYParser::WhileStmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitWhileStmt(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::WhileStmtContext* SysYParser::whileStmt() {
  WhileStmtContext *_localctx = _tracker.createInstance<WhileStmtContext>(_ctx, getState());
  enterRule(_localctx, 30, SysYParser::RuleWhileStmt);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(168);
    match(SysYParser::WHILE);
    setState(169);
    match(SysYParser::LPAREN);
    setState(170);
    exp(0);
    setState(171);
    match(SysYParser::RPAREN);
    setState(172);
    stmt();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BreakStmtContext ------------------------------------------------------------------

SysYParser::BreakStmtContext::BreakStmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::BreakStmtContext::BREAK() {
  return getToken(SysYParser::BREAK, 0);
}

tree::TerminalNode* SysYParser::BreakStmtContext::SEMICOLON() {
  return getToken(SysYParser::SEMICOLON, 0);
}


size_t SysYParser::BreakStmtContext::getRuleIndex() const {
  return SysYParser::RuleBreakStmt;
}


std::any SysYParser::BreakStmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitBreakStmt(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::BreakStmtContext* SysYParser::breakStmt() {
  BreakStmtContext *_localctx = _tracker.createInstance<BreakStmtContext>(_ctx, getState());
  enterRule(_localctx, 32, SysYParser::RuleBreakStmt);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(174);
    match(SysYParser::BREAK);
    setState(175);
    match(SysYParser::SEMICOLON);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ContinueStmtContext ------------------------------------------------------------------

SysYParser::ContinueStmtContext::ContinueStmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::ContinueStmtContext::CONTINUE() {
  return getToken(SysYParser::CONTINUE, 0);
}

tree::TerminalNode* SysYParser::ContinueStmtContext::SEMICOLON() {
  return getToken(SysYParser::SEMICOLON, 0);
}


size_t SysYParser::ContinueStmtContext::getRuleIndex() const {
  return SysYParser::RuleContinueStmt;
}


std::any SysYParser::ContinueStmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitContinueStmt(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::ContinueStmtContext* SysYParser::continueStmt() {
  ContinueStmtContext *_localctx = _tracker.createInstance<ContinueStmtContext>(_ctx, getState());
  enterRule(_localctx, 34, SysYParser::RuleContinueStmt);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(177);
    match(SysYParser::CONTINUE);
    setState(178);
    match(SysYParser::SEMICOLON);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ReturnStmtContext ------------------------------------------------------------------

SysYParser::ReturnStmtContext::ReturnStmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::ReturnStmtContext::RETURN() {
  return getToken(SysYParser::RETURN, 0);
}

tree::TerminalNode* SysYParser::ReturnStmtContext::SEMICOLON() {
  return getToken(SysYParser::SEMICOLON, 0);
}

SysYParser::ExpContext* SysYParser::ReturnStmtContext::exp() {
  return getRuleContext<SysYParser::ExpContext>(0);
}


size_t SysYParser::ReturnStmtContext::getRuleIndex() const {
  return SysYParser::RuleReturnStmt;
}


std::any SysYParser::ReturnStmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitReturnStmt(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::ReturnStmtContext* SysYParser::returnStmt() {
  ReturnStmtContext *_localctx = _tracker.createInstance<ReturnStmtContext>(_ctx, getState());
  enterRule(_localctx, 36, SysYParser::RuleReturnStmt);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(180);
    match(SysYParser::RETURN);
    setState(182);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 257798713344) != 0)) {
      setState(181);
      exp(0);
    }
    setState(184);
    match(SysYParser::SEMICOLON);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- EmptyStmtContext ------------------------------------------------------------------

SysYParser::EmptyStmtContext::EmptyStmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::EmptyStmtContext::SEMICOLON() {
  return getToken(SysYParser::SEMICOLON, 0);
}


size_t SysYParser::EmptyStmtContext::getRuleIndex() const {
  return SysYParser::RuleEmptyStmt;
}


std::any SysYParser::EmptyStmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitEmptyStmt(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::EmptyStmtContext* SysYParser::emptyStmt() {
  EmptyStmtContext *_localctx = _tracker.createInstance<EmptyStmtContext>(_ctx, getState());
  enterRule(_localctx, 38, SysYParser::RuleEmptyStmt);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(186);
    match(SysYParser::SEMICOLON);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExpContext ------------------------------------------------------------------

SysYParser::ExpContext::ExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SysYParser::ExpContext::getRuleIndex() const {
  return SysYParser::RuleExp;
}

void SysYParser::ExpContext::copyFrom(ExpContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- RelationExpContext ------------------------------------------------------------------

std::vector<SysYParser::ExpContext *> SysYParser::RelationExpContext::exp() {
  return getRuleContexts<SysYParser::ExpContext>();
}

SysYParser::ExpContext* SysYParser::RelationExpContext::exp(size_t i) {
  return getRuleContext<SysYParser::ExpContext>(i);
}

tree::TerminalNode* SysYParser::RelationExpContext::LT() {
  return getToken(SysYParser::LT, 0);
}

tree::TerminalNode* SysYParser::RelationExpContext::GT() {
  return getToken(SysYParser::GT, 0);
}

tree::TerminalNode* SysYParser::RelationExpContext::LE() {
  return getToken(SysYParser::LE, 0);
}

tree::TerminalNode* SysYParser::RelationExpContext::GE() {
  return getToken(SysYParser::GE, 0);
}

SysYParser::RelationExpContext::RelationExpContext(ExpContext *ctx) { copyFrom(ctx); }


std::any SysYParser::RelationExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitRelationExp(this);
  else
    return visitor->visitChildren(this);
}
//----------------- MultiplicativeExpContext ------------------------------------------------------------------

std::vector<SysYParser::ExpContext *> SysYParser::MultiplicativeExpContext::exp() {
  return getRuleContexts<SysYParser::ExpContext>();
}

SysYParser::ExpContext* SysYParser::MultiplicativeExpContext::exp(size_t i) {
  return getRuleContext<SysYParser::ExpContext>(i);
}

tree::TerminalNode* SysYParser::MultiplicativeExpContext::MUL() {
  return getToken(SysYParser::MUL, 0);
}

tree::TerminalNode* SysYParser::MultiplicativeExpContext::DIV() {
  return getToken(SysYParser::DIV, 0);
}

tree::TerminalNode* SysYParser::MultiplicativeExpContext::MODULO() {
  return getToken(SysYParser::MODULO, 0);
}

SysYParser::MultiplicativeExpContext::MultiplicativeExpContext(ExpContext *ctx) { copyFrom(ctx); }


std::any SysYParser::MultiplicativeExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitMultiplicativeExp(this);
  else
    return visitor->visitChildren(this);
}
//----------------- LValueExpContext ------------------------------------------------------------------

SysYParser::LValueContext* SysYParser::LValueExpContext::lValue() {
  return getRuleContext<SysYParser::LValueContext>(0);
}

SysYParser::LValueExpContext::LValueExpContext(ExpContext *ctx) { copyFrom(ctx); }


std::any SysYParser::LValueExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitLValueExp(this);
  else
    return visitor->visitChildren(this);
}
//----------------- NumberExpContext ------------------------------------------------------------------

SysYParser::NumberContext* SysYParser::NumberExpContext::number() {
  return getRuleContext<SysYParser::NumberContext>(0);
}

SysYParser::NumberExpContext::NumberExpContext(ExpContext *ctx) { copyFrom(ctx); }


std::any SysYParser::NumberExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitNumberExp(this);
  else
    return visitor->visitChildren(this);
}
//----------------- AndExpContext ------------------------------------------------------------------

std::vector<SysYParser::ExpContext *> SysYParser::AndExpContext::exp() {
  return getRuleContexts<SysYParser::ExpContext>();
}

SysYParser::ExpContext* SysYParser::AndExpContext::exp(size_t i) {
  return getRuleContext<SysYParser::ExpContext>(i);
}

tree::TerminalNode* SysYParser::AndExpContext::AND() {
  return getToken(SysYParser::AND, 0);
}

SysYParser::AndExpContext::AndExpContext(ExpContext *ctx) { copyFrom(ctx); }


std::any SysYParser::AndExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitAndExp(this);
  else
    return visitor->visitChildren(this);
}
//----------------- UnaryExpContext ------------------------------------------------------------------

SysYParser::ExpContext* SysYParser::UnaryExpContext::exp() {
  return getRuleContext<SysYParser::ExpContext>(0);
}

tree::TerminalNode* SysYParser::UnaryExpContext::ADD() {
  return getToken(SysYParser::ADD, 0);
}

tree::TerminalNode* SysYParser::UnaryExpContext::SUB() {
  return getToken(SysYParser::SUB, 0);
}

tree::TerminalNode* SysYParser::UnaryExpContext::NOT() {
  return getToken(SysYParser::NOT, 0);
}

SysYParser::UnaryExpContext::UnaryExpContext(ExpContext *ctx) { copyFrom(ctx); }


std::any SysYParser::UnaryExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitUnaryExp(this);
  else
    return visitor->visitChildren(this);
}
//----------------- ParenExpContext ------------------------------------------------------------------

tree::TerminalNode* SysYParser::ParenExpContext::LPAREN() {
  return getToken(SysYParser::LPAREN, 0);
}

SysYParser::ExpContext* SysYParser::ParenExpContext::exp() {
  return getRuleContext<SysYParser::ExpContext>(0);
}

tree::TerminalNode* SysYParser::ParenExpContext::RPAREN() {
  return getToken(SysYParser::RPAREN, 0);
}

SysYParser::ParenExpContext::ParenExpContext(ExpContext *ctx) { copyFrom(ctx); }


std::any SysYParser::ParenExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitParenExp(this);
  else
    return visitor->visitChildren(this);
}
//----------------- StringExpContext ------------------------------------------------------------------

SysYParser::StringContext* SysYParser::StringExpContext::string() {
  return getRuleContext<SysYParser::StringContext>(0);
}

SysYParser::StringExpContext::StringExpContext(ExpContext *ctx) { copyFrom(ctx); }


std::any SysYParser::StringExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitStringExp(this);
  else
    return visitor->visitChildren(this);
}
//----------------- OrExpContext ------------------------------------------------------------------

std::vector<SysYParser::ExpContext *> SysYParser::OrExpContext::exp() {
  return getRuleContexts<SysYParser::ExpContext>();
}

SysYParser::ExpContext* SysYParser::OrExpContext::exp(size_t i) {
  return getRuleContext<SysYParser::ExpContext>(i);
}

tree::TerminalNode* SysYParser::OrExpContext::OR() {
  return getToken(SysYParser::OR, 0);
}

SysYParser::OrExpContext::OrExpContext(ExpContext *ctx) { copyFrom(ctx); }


std::any SysYParser::OrExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitOrExp(this);
  else
    return visitor->visitChildren(this);
}
//----------------- CallExpContext ------------------------------------------------------------------

SysYParser::CallContext* SysYParser::CallExpContext::call() {
  return getRuleContext<SysYParser::CallContext>(0);
}

SysYParser::CallExpContext::CallExpContext(ExpContext *ctx) { copyFrom(ctx); }


std::any SysYParser::CallExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitCallExp(this);
  else
    return visitor->visitChildren(this);
}
//----------------- AdditiveExpContext ------------------------------------------------------------------

std::vector<SysYParser::ExpContext *> SysYParser::AdditiveExpContext::exp() {
  return getRuleContexts<SysYParser::ExpContext>();
}

SysYParser::ExpContext* SysYParser::AdditiveExpContext::exp(size_t i) {
  return getRuleContext<SysYParser::ExpContext>(i);
}

tree::TerminalNode* SysYParser::AdditiveExpContext::ADD() {
  return getToken(SysYParser::ADD, 0);
}

tree::TerminalNode* SysYParser::AdditiveExpContext::SUB() {
  return getToken(SysYParser::SUB, 0);
}

SysYParser::AdditiveExpContext::AdditiveExpContext(ExpContext *ctx) { copyFrom(ctx); }


std::any SysYParser::AdditiveExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitAdditiveExp(this);
  else
    return visitor->visitChildren(this);
}
//----------------- EqualExpContext ------------------------------------------------------------------

std::vector<SysYParser::ExpContext *> SysYParser::EqualExpContext::exp() {
  return getRuleContexts<SysYParser::ExpContext>();
}

SysYParser::ExpContext* SysYParser::EqualExpContext::exp(size_t i) {
  return getRuleContext<SysYParser::ExpContext>(i);
}

tree::TerminalNode* SysYParser::EqualExpContext::EQ() {
  return getToken(SysYParser::EQ, 0);
}

tree::TerminalNode* SysYParser::EqualExpContext::NE() {
  return getToken(SysYParser::NE, 0);
}

SysYParser::EqualExpContext::EqualExpContext(ExpContext *ctx) { copyFrom(ctx); }


std::any SysYParser::EqualExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitEqualExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::ExpContext* SysYParser::exp() {
   return exp(0);
}

SysYParser::ExpContext* SysYParser::exp(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  SysYParser::ExpContext *_localctx = _tracker.createInstance<ExpContext>(_ctx, parentState);
  SysYParser::ExpContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 40;
  enterRecursionRule(_localctx, 40, SysYParser::RuleExp, precedence);

    size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(199);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 17, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<ParenExpContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;

      setState(189);
      match(SysYParser::LPAREN);
      setState(190);
      exp(0);
      setState(191);
      match(SysYParser::RPAREN);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<LValueExpContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(193);
      lValue();
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<NumberExpContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(194);
      number();
      break;
    }

    case 4: {
      _localctx = _tracker.createInstance<StringExpContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(195);
      string();
      break;
    }

    case 5: {
      _localctx = _tracker.createInstance<CallExpContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(196);
      call();
      break;
    }

    case 6: {
      _localctx = _tracker.createInstance<UnaryExpContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(197);
      _la = _input->LA(1);
      if (!((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 33566720) != 0))) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(198);
      exp(7);
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(221);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 19, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(219);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 18, _ctx)) {
        case 1: {
          auto newContext = _tracker.createInstance<MultiplicativeExpContext>(_tracker.createInstance<ExpContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExp);
          setState(201);

          if (!(precpred(_ctx, 6))) throw FailedPredicateException(this, "precpred(_ctx, 6)");
          setState(202);
          _la = _input->LA(1);
          if (!((((_la & ~ 0x3fULL) == 0) &&
            ((1ULL << _la) & 114688) != 0))) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(203);
          exp(7);
          break;
        }

        case 2: {
          auto newContext = _tracker.createInstance<AdditiveExpContext>(_tracker.createInstance<ExpContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExp);
          setState(204);

          if (!(precpred(_ctx, 5))) throw FailedPredicateException(this, "precpred(_ctx, 5)");
          setState(205);
          _la = _input->LA(1);
          if (!(_la == SysYParser::ADD

          || _la == SysYParser::SUB)) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(206);
          exp(6);
          break;
        }

        case 3: {
          auto newContext = _tracker.createInstance<RelationExpContext>(_tracker.createInstance<ExpContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExp);
          setState(207);

          if (!(precpred(_ctx, 4))) throw FailedPredicateException(this, "precpred(_ctx, 4)");
          setState(208);
          _la = _input->LA(1);
          if (!((((_la & ~ 0x3fULL) == 0) &&
            ((1ULL << _la) & 1966080) != 0))) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(209);
          exp(5);
          break;
        }

        case 4: {
          auto newContext = _tracker.createInstance<EqualExpContext>(_tracker.createInstance<ExpContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExp);
          setState(210);

          if (!(precpred(_ctx, 3))) throw FailedPredicateException(this, "precpred(_ctx, 3)");
          setState(211);
          _la = _input->LA(1);
          if (!(_la == SysYParser::EQ

          || _la == SysYParser::NE)) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(212);
          exp(4);
          break;
        }

        case 5: {
          auto newContext = _tracker.createInstance<AndExpContext>(_tracker.createInstance<ExpContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExp);
          setState(213);

          if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
          setState(214);
          match(SysYParser::AND);
          setState(215);
          exp(3);
          break;
        }

        case 6: {
          auto newContext = _tracker.createInstance<OrExpContext>(_tracker.createInstance<ExpContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExp);
          setState(216);

          if (!(precpred(_ctx, 1))) throw FailedPredicateException(this, "precpred(_ctx, 1)");
          setState(217);
          match(SysYParser::OR);
          setState(218);
          exp(2);
          break;
        }

        default:
          break;
        } 
      }
      setState(223);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 19, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- CallContext ------------------------------------------------------------------

SysYParser::CallContext::CallContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::CallContext::ID() {
  return getToken(SysYParser::ID, 0);
}

tree::TerminalNode* SysYParser::CallContext::LPAREN() {
  return getToken(SysYParser::LPAREN, 0);
}

tree::TerminalNode* SysYParser::CallContext::RPAREN() {
  return getToken(SysYParser::RPAREN, 0);
}

SysYParser::FuncRParamsContext* SysYParser::CallContext::funcRParams() {
  return getRuleContext<SysYParser::FuncRParamsContext>(0);
}


size_t SysYParser::CallContext::getRuleIndex() const {
  return SysYParser::RuleCall;
}


std::any SysYParser::CallContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitCall(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::CallContext* SysYParser::call() {
  CallContext *_localctx = _tracker.createInstance<CallContext>(_ctx, getState());
  enterRule(_localctx, 42, SysYParser::RuleCall);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(224);
    match(SysYParser::ID);
    setState(225);
    match(SysYParser::LPAREN);
    setState(227);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 257798713344) != 0)) {
      setState(226);
      funcRParams();
    }
    setState(229);
    match(SysYParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LValueContext ------------------------------------------------------------------

SysYParser::LValueContext::LValueContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::LValueContext::ID() {
  return getToken(SysYParser::ID, 0);
}

std::vector<tree::TerminalNode *> SysYParser::LValueContext::LBRACKET() {
  return getTokens(SysYParser::LBRACKET);
}

tree::TerminalNode* SysYParser::LValueContext::LBRACKET(size_t i) {
  return getToken(SysYParser::LBRACKET, i);
}

std::vector<SysYParser::ExpContext *> SysYParser::LValueContext::exp() {
  return getRuleContexts<SysYParser::ExpContext>();
}

SysYParser::ExpContext* SysYParser::LValueContext::exp(size_t i) {
  return getRuleContext<SysYParser::ExpContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::LValueContext::RBRACKET() {
  return getTokens(SysYParser::RBRACKET);
}

tree::TerminalNode* SysYParser::LValueContext::RBRACKET(size_t i) {
  return getToken(SysYParser::RBRACKET, i);
}


size_t SysYParser::LValueContext::getRuleIndex() const {
  return SysYParser::RuleLValue;
}


std::any SysYParser::LValueContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitLValue(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::LValueContext* SysYParser::lValue() {
  LValueContext *_localctx = _tracker.createInstance<LValueContext>(_ctx, getState());
  enterRule(_localctx, 44, SysYParser::RuleLValue);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(231);
    match(SysYParser::ID);
    setState(238);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 21, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(232);
        match(SysYParser::LBRACKET);
        setState(233);
        exp(0);
        setState(234);
        match(SysYParser::RBRACKET); 
      }
      setState(240);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 21, _ctx);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- NumberContext ------------------------------------------------------------------

SysYParser::NumberContext::NumberContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::NumberContext::ILITERAL() {
  return getToken(SysYParser::ILITERAL, 0);
}

tree::TerminalNode* SysYParser::NumberContext::FLITERAL() {
  return getToken(SysYParser::FLITERAL, 0);
}


size_t SysYParser::NumberContext::getRuleIndex() const {
  return SysYParser::RuleNumber;
}


std::any SysYParser::NumberContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitNumber(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::NumberContext* SysYParser::number() {
  NumberContext *_localctx = _tracker.createInstance<NumberContext>(_ctx, getState());
  enterRule(_localctx, 46, SysYParser::RuleNumber);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(241);
    _la = _input->LA(1);
    if (!(_la == SysYParser::ILITERAL

    || _la == SysYParser::FLITERAL)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StringContext ------------------------------------------------------------------

SysYParser::StringContext::StringContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::StringContext::STRING() {
  return getToken(SysYParser::STRING, 0);
}


size_t SysYParser::StringContext::getRuleIndex() const {
  return SysYParser::RuleString;
}


std::any SysYParser::StringContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitString(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::StringContext* SysYParser::string() {
  StringContext *_localctx = _tracker.createInstance<StringContext>(_ctx, getState());
  enterRule(_localctx, 48, SysYParser::RuleString);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(243);
    match(SysYParser::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FuncRParamsContext ------------------------------------------------------------------

SysYParser::FuncRParamsContext::FuncRParamsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParser::ExpContext *> SysYParser::FuncRParamsContext::exp() {
  return getRuleContexts<SysYParser::ExpContext>();
}

SysYParser::ExpContext* SysYParser::FuncRParamsContext::exp(size_t i) {
  return getRuleContext<SysYParser::ExpContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::FuncRParamsContext::COMMA() {
  return getTokens(SysYParser::COMMA);
}

tree::TerminalNode* SysYParser::FuncRParamsContext::COMMA(size_t i) {
  return getToken(SysYParser::COMMA, i);
}


size_t SysYParser::FuncRParamsContext::getRuleIndex() const {
  return SysYParser::RuleFuncRParams;
}


std::any SysYParser::FuncRParamsContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitFuncRParams(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::FuncRParamsContext* SysYParser::funcRParams() {
  FuncRParamsContext *_localctx = _tracker.createInstance<FuncRParamsContext>(_ctx, getState());
  enterRule(_localctx, 50, SysYParser::RuleFuncRParams);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(245);
    exp(0);
    setState(250);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParser::COMMA) {
      setState(246);
      match(SysYParser::COMMA);
      setState(247);
      exp(0);
      setState(252);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

bool SysYParser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 20: return expSempred(antlrcpp::downCast<ExpContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool SysYParser::expSempred(ExpContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 6);
    case 1: return precpred(_ctx, 5);
    case 2: return precpred(_ctx, 4);
    case 3: return precpred(_ctx, 3);
    case 4: return precpred(_ctx, 2);
    case 5: return precpred(_ctx, 1);

  default:
    break;
  }
  return true;
}

void SysYParser::initialize() {
  ::antlr4::internal::call_once(sysyParserOnceFlag, sysyParserInitialize);
}
