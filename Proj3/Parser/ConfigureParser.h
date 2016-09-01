#ifndef CONFIGUREPARSER_H
#define CONFIGUREPARSER_H
/////////////////////////////////////////////////////////////////////
//  ConfigureParser.h - builds and configures parsers              //
//  ver 2.2                                                        //
//                                                                 //
//  Language:      Visual C++ 2015, SP1                            //
//  Platform:      Macbook, Win10                                  //
//  Author:        Wenbin Li,      Syracuse University             //
//                 wli102@syr.edu                                  //
//              Source from Dr. Jim                                //
/////////////////////////////////////////////////////////////////////
/*
  Module Operations: 
  ==================
  This module builds and configures parsers.  It builds the parser
  parts and configures them with application specific rules and actions.

  Public Interface:
  =================
  ConfigParseToConsole conConfig;
  conConfig.BuildMetricAnalysis     //Metric analysis
  conConfig.Build                   //type analysis
  conConfig.BuildDependencyAnalysis //dependency analysis
  conConfig.Attach(someFileName);

  Build Process:
  ==============
  Required files
    - ConfigureParser.h, ConfigureParser.cpp, Parser.h, Parser.cpp,
      ActionsAndRules.h, ActionsAndRules.cpp,
      SemiExpression.h, SemiExpression.cpp, tokenizer.h, tokenizer.cpp
  Build commands 
    - devenv Project3.sln /rebuild debug


  Maintenance History:
  ====================
  ver 2.2 : 04 Apr 16
  - Added getRepository function
  - Modified BuildMetricAnalysis
  - Added build, and BuildDependencyAnalysis function for paralleanalysis
  - Add some properties(rules and actioins) for type and dependency analysis
    such as get enum type and create type table, and so on.
  ver 2.1 : 19 Feb 16
  - Added PrintFunction action to FunctionDefinition rule
  ver 2.0 : 01 Jun 11
  - Major revisions to begin building a strong code analyzer
  ver 1.1 : 01 Feb 06
  - cosmetic changes to ConfigureParser.cpp
  ver 1.0 : 12 Jan 06
  - first release

*/

//
#include <fstream>
#include "Parser.h"
#include "../SemiExp/SemiExp.h"
#include "../Tokenizer/Tokenizer.h"
#include "ActionsAndRules.h"
#include "../AbstractSyntaxTree/AbstractSyntaxTree.h"

///////////////////////////////////////////////////////////////
// build parser that writes its output to console

class ConfigParseToConsole : IBuilder
{
public:
  ConfigParseToConsole(AST<element> abstractSyntaxtTree) {};
  ConfigParseToConsole() {}
  ~ConfigParseToConsole();
  bool Attach(const std::string& name, bool isFile=true);
  Parser* Build(AST<element>& abstractSyntaxtTree);
  Parser* BuildMetricAnalysis(AST<element>& abstractSyntaxtTree);
  Parser* BuildDependencyAnalysis(AST<element>& abstractSyntaxtTree);
  //Parser* Build();
  Repository* getRepository() { return pRepo; }
  void buildStrutConfigure();
  void buildDependencyConfigure();

private:
  // Builder must hold onto all the pieces

  std::ifstream* pIn;
  Scanner::Toker* pToker;
  Scanner::SemiExp* pSemi;
  Parser* pParser;
  Repository* pRepo;

  // add Rules and Actions

  BeginningOfScope* pBeginningOfScope;
  HandlePush* pHandlePush;
  EndOfScope* pEndOfScope;
  HandlePop* pHandlePop;
  FunctionDefinition* pFunctionDefinition;
  PushFunction* pPushFunction;
  PrintFunction* pPrintFunction;
  SpecialKeyWord* pSpecialKeyWord;
  PushSpecialKeyWord* pPushSpecialKeyWord;
  KeyWordWithoutParentheses* pKeyWordWithoutParentheses;
  PushKeyWordWithoutParentheses* pPushKeyWordWithoutParentheses;
  NameSpaceAndClassMectricAnalysis* pNameSpaceAndClassMectricAnalysis;
  PushNameSpaceAndClassMectricAnalysis* pPushNameSpaceAndClassMectricAnalysis;
  StructEnumMectricAnalysis* pStructEnumMectricAnalysis;
  WritStructEnumMectricAnalysis* pWritStructEnumMectricAnalysis;
   
  NameSpaceAndClass* pNameSpaceAndClass; //some rules and actions for type analysis
  PushNameSpaceAndClass* pPushNameSpaceAndClass;
  NameSpaceAndClassCheckDepend* pNameSpaceAndClassCheckDepend;
  EnumsType* pEnumsType;
  WritEnumsInMap* pWritEnumsInMap;
  TypeDefType* pTypeDefType;
  WritTypedefInMap* pWritTypedefInMap;
  UsingType* pUsingType;
  WritUsingTypeInMap* pWritUsingTypeInMap;
  StructType* pStructType;
  WritStructType* pWritStructType;
  ForwardClassStructType* pForwardClassStructType;
  WritForwardClassStructType* pWritForwardClassStructType;
  WriteFunctionInMapTable* pWriteFunctionInMapTable; //wite function into typetable

  Declaration* pDeclaration;  //some rules and actions for dependency analysis
  HandleDeclaration* pHandleDeclaration;
  HandleDeclarationUSING* pHandleDeclarationUSING;
  Executable* pExecutable;
  HandleExecutable* pHandleExecutable;
  HandleExecutableUSING* pHandleExecutableUSING;
  HandleFuncDeclarationForDependency* pHandleFuncDeclarationForDependency;
  HandleFuncDeclarationForDependencyUSING* pHandleFuncDeclarationForDependencyUSING;
  HandleFuncDeforHeadertoCpp* pHandleFuncDeforHeadertoCpp;
  CatchUsingNamspace* pCatchUsingNamspace;
  HandleUsingNamspace* pHandleUsingNamspace;
  SpecifiedNameSpacedFunc* pSpecifiedNameSpacedFunc;
  HandleSpecifiedNameSpacedFunc* pHandleSpecifiedNameSpacedFunc;
  SpecifiedNameSpaced* pSpecifiedNameSpaced;
  HandleSpecifiedNameSpaced* pHandleSpecifiedNameSpaced;

  // prohibit copies and assignments
  ConfigParseToConsole(const ConfigParseToConsole&) = delete;
  ConfigParseToConsole& operator=(const ConfigParseToConsole&) = delete;
};


#endif
