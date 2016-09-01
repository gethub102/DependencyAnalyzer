#ifndef PARSER_H
#define PARSER_H
/////////////////////////////////////////////////////////////////////
//  Parser.h - Analyzes C++ language constructs                    //
//  ver 1.4                                                        //
//  Language:      Visual C++ 2015, SP1                            //
//  Platform:      Macbook, Win10                                  //
//  Author:        Wenbin Li,      Syracuse University             //
//                 wli102@syr.edu                                  //
/////////////////////////////////////////////////////////////////////
/*
  Module Operations:
  ==================
  This module defines a Parser class.  Its instances collect
  semi-expressions from a file for analysis.  Analysis consists of
  applying a set of rules to the semi-expression, and for each rule
  that matches, a set of actions is invoked.

  Public Interface:
  =================
  Toker t(someFile);              // create tokenizer instance
  SemiExp se(&t);                 // create a SemiExp attached to tokenizer
  Parser parser(se);              // now we have a parser
  Rule1 r1;                       // create instance of a derived Rule class
  Action1 a1;                     // create a derived action
  r1.addAction(&a1);              // register action with the rule
  parser.addRule(&r1);            // register rule with parser
  while(se.getSemiExp())          // get semi-expression
	parser.parse();               //   and parse it

  Build Process:
  ==============
  Required files
	- Parser.h, Parser.cpp, SemiExpression.h, SemiExpression.cpp,
	  tokenizer.h, tokenizer.cpp,
	  ActionsAndRules.h, ActionsAndRules.cpp,
	  ConfigureParser.h, ConfigureParser.cpp
  Build commands (either one)
	- devenv Project2HelperCode.sln /rebuild debug

  Maintenance History:
  ====================
  ver 1.4 : 15 Feb 16
  - removed all folding rules code
  - changed solution folder layout which caused changes in many of the
	include paths
  - small modifications to one of the derived actions
  - now preface (new) Toker and SemiExp with Scanner namespace
  ver 1.3 : 02 Jun 11
  - added use of folding rules in get();
  ver 1.2 : 17 Jan 09
  - modified to accept pointer to interface ITokCollection instead of
	SemiExpression
  ver 1.1 : 17 Sep 07
  - cosmetic modifications to comments on this page
  ver 1.0 : 12 Jan 06
  - first release

*/

#include <string>
#include <iostream>
#include <vector>
#include "../SemiExp/itokcollection.h"
#include "../AbstractSyntaxTree/AbstractSyntaxTree.h"
#include "../MetricAnalysis/MetricAnalysis.h"


///////////////////////////////////////////////////////////////
// abstract base class for parsing actions
//   - when a rule succeeds, it invokes any registered action

class IAction
{
public:
	virtual ~IAction() {}
	virtual void doAction(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName) = 0;
};

///////////////////////////////////////////////////////////////
// abstract base class for parser language construct detections
//   - rules are registered with the parser for use

class IRule
{
public:
	virtual ~IRule() {}
	void addAction(IAction* pAction);
	void doActions(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName);
	virtual bool doTest(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName) = 0;
	//virtual std::string ruleName() = 0;
protected:
	std::vector<IAction*> actions;
};

class Parser
{
public:
	Parser(ITokCollection* pTokCollection);
	~Parser();
	void addRule(IRule* pRule);
	//bool parse(AST<element>& abstractSyntaxtTree);
	bool parse(AST<element>& abstractSyntaxtTree, std::string fileName);
	bool next();
private:
	ITokCollection* pTokColl;
	std::vector<IRule*> rules;
};

inline Parser::Parser(ITokCollection* pTokCollection) : pTokColl(pTokCollection) {}

inline Parser::~Parser() {}

///////////////////////////////////////////////////////////////
// abstract base class for parsing actions
//   - when a rule succeeds, it invokes any registered action

//class Parser; // forward reference

class IBuilder
{
public:
	virtual ~IBuilder() {}
	virtual Parser* Build(AST<element>& abstractSyntaxtTree) = 0;
};


#endif
