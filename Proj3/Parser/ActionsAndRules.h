#ifndef ACTIONSANDRULES_H
#define ACTIONSANDRULES_H
/////////////////////////////////////////////////////////////////////
//  ActionsAndRules.h - declares new parsing rules and actions     //
//  ver 2.2                                                        //
//  Language:      Visual C++ 2015                                 //
//  Platform:      Macbook, Win10                                  //
//  Application:   Prototype for CSE687 Pr3                        //
//  Author:        Wenbin LI, Syracuse University                  //
//                 wli102@syr.edu                                  //
//              Source from Dr. Jim                                //
/////////////////////////////////////////////////////////////////////
/*
  Module Operations:
  ==================
  This module defines several action classes.  Its classes provide
  specialized services needed for specific applications.  The modules
  Parser, SemiExpression, and Tokenizer, are intended to be reusable
  without change.  This module provides a place to put extensions of
  these facilities and is not expected to be reusable.

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
	- Parser.h, Parser.cpp, ScopeStack.h, ScopeStack.cpp,
	  ActionsAndRules.h, ActionsAndRules.cpp, ConfigureParser.cpp,
	  ItokCollection.h, SemiExpression.h, SemiExpression.cpp, tokenizer.h, tokenizer.cpp
  Build commands
	- devenv Project3.sln /rebuild debug

  Maintenance History:
  ====================
  ver 2.2 : 15 Feb 16
  - add class and namespace field stack in Repository
  - add serveral class for rules and actions to anlysis type and dependency
	such as, NameSpaceAndClassCheckDepend, EnumsType, WritEnumsInMap,
	TypeDefType, WritTypedefInMap, UsingType, WritUsingTypeInMap, StructType,
	WritStructType, ForwardClassStructType, WritForwardClassStructType,
	WriteFunctionInMapTable, CatchUsingNamspace, HandleUsingNamspace,
	SpecifiedNameSpacedFunc, HandleSpecifiedNameSpacedFunc
  ver 2.1 : 15 Feb 16
  - small functional change to a few of the actions changes display strategy
  - preface the (new) Toker and SemiExp with Scanner namespace
  ver 2.0 : 01 Jun 11
  - added processing on way to building strong code analyzer
  ver 1.1 : 17 Jan 09
  - changed to accept a pointer to interfaced ITokCollection instead
	of a SemiExpression
  ver 1.0 : 12 Jan 06
  - first release

*/
//
#include <queue>
#include <string>
#include <sstream>
#include <mutex>
#include "Parser.h"
#include "../SemiExp/itokcollection.h"
#include "../ScopeStack/ScopeStack.h"
#include "../Tokenizer/Tokenizer.h"
#include "../SemiExp/SemiExp.h"
#include "../MetricAnalysis/MetricAnalysis.h"
#include "../Utilities/Helpers.h"


///////////////////////////////////////////////////////////////
// Repository instance is used to share resources
// among all actions.
/*
 * ToDo:
 * - add AST Node class
 * - provide field to hold root of AST
 * - provide interface to access AST
 * - provide helper functions to build AST, perhaps in Tree class
 */
 /* class for store the element */
using DependencyList = std::unordered_map<std::string, std::unordered_map<std::string, std::string>>;
class Repository  // application specific
{
	ScopeStack<element*> stack;
	Scanner::Toker* p_Toker;
	MertricAnalysis* p_metricAnalysis;
	std::unordered_map<std::string, element*>* typetableMap = new std::unordered_map<std::string, element*>(); //map for second analysis
	DependencyList* dependencyList = new DependencyList();
	DependencyList* handhcpp = new DependencyList();
	ScopeStack<element*> namespace_stack;
	ScopeStack<element*> class_stack;
	std::vector<std::string> handleusingnamespace;
public:
	std::vector<std::string>& gethandleusingnamespace() { return handleusingnamespace; }
	//get namespace stack
	ScopeStack<element*>& getnamespace_stack() { return namespace_stack; }
	//get class stack
	ScopeStack<element*>& getclass_stack() { return class_stack; }
	//get namespace name
	std::string getNameSpacename() {
		if (namespace_stack.size() > 0)
			return namespace_stack.top()->name;
		else return "default";
	}
	//get class name
	std::string getclassname() {
		if (class_stack.size() > 0)
			return class_stack.top()->name;
		else return "default";
	}
	//get Dependency List
	DependencyList* getDependencyList() {
		return dependencyList;
	}
	//set Denpendency List
	void setDependencyList(std::string fileName, std::string addfile) {
		std::unordered_map<std::string, std::string>* tmp = new std::unordered_map<std::string, std::string>();
		(*tmp)[addfile] = "default";
		(*dependencyList)[fileName].insert((*tmp).begin(), (*tmp).end());
	}
	//set the map of type table
	void setRefTypeTableMap(std::unordered_map<std::string, element*> tablemap) {
		(*typetableMap).insert(tablemap.begin(), tablemap.end());
	}
	//get the map of type table
	std::unordered_map<std::string, element*>* getRefTypeTableMap() {
		return typetableMap;
	}
	//get p_metricAnalysis pointer
	MertricAnalysis* metricAnalysis() {
		return p_metricAnalysis;
	}
	//set toker pointer
	Repository(Scanner::Toker* pToker)
	{
		p_Toker = pToker;
	}
	//get stack
	ScopeStack<element*>& scopeStack()
	{
		return stack;
	}
	//get toker pointer
	Scanner::Toker* Toker()
	{
		return p_Toker;
	}
	//get line number
	size_t lineCount()
	{
		return (size_t)(p_Toker->currentLineCount());
	}
};

///////////////////////////////////////////////////////////////
// rule to detect beginning of anonymous scope

class BeginningOfScope : public IRule
{
	std::mutex mtx_;
public:
	//BeginningOfScope rule test
	bool doTest(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName)
	{
		std::lock_guard<std::mutex> l(mtx_);
		if (pTc->find("{") < pTc->length())
		{
			//std::cout << "  BeginningOfScope match!\n";
			doActions(pTc, abstractSyntaxtTree, fileName);
			return true;
		}
		return true; // if did not find {, stop check this token.
	}

	std::string ruleName() {
		return "BeginningOfScope";
	}
};

///////////////////////////////////////////////////////////////
// action to handle scope stack at end of scope

class HandlePush : public IAction
{
	Repository* p_Repos;
	bool first = true;
public:
	//constructor
	HandlePush(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	//BeginningOfScope do action
	void doAction(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName)
	{
		if (first == true) {
			first = false;
			element* nameSpace = new element;
			nameSpace->type = "Global-NameSpace";
			nameSpace->name = "Root";
			nameSpace->startLineCount = p_Repos->lineCount();
			p_Repos->scopeStack().push(nameSpace);
			abstractSyntaxtTree.pushToTree(nameSpace);
		}
		//std::cout << "\n--BeginningOfScope rule";
		element *elem = new element;
		elem->type = "unknown";
		elem->name = "anonymous";
		elem->startLineCount = p_Repos->lineCount();
		p_Repos->scopeStack().top()->addChild(elem);
		p_Repos->scopeStack().push(elem);
	}
};

///////////////////////////////////////////////////////////////
// rule to detect end of scope

class EndOfScope : public IRule
{
	std::mutex mtx_;
public:
	//EndOfScope rule test
	bool doTest(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName)
	{
		std::lock_guard<std::mutex> l(mtx_);
		if (pTc->find("}") < pTc->length())
		{
			//std::cout << "  EndOfScope match!\n";
			doActions(pTc, abstractSyntaxtTree, fileName);
			return false;
		}
		return true;
	}

	// return the rule's name
	std::string ruleName() {
		return "EndOfScope";
	}
};

///////////////////////////////////////////////////////////////
// action to handle scope stack at end of scope

class HandlePop : public IAction
{
	Repository* p_Repos;
public:
	//constructor
	HandlePop(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	//EndOfScope action
	void doAction(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName)
	{
		//std::cout << "\n--EndOfScope rule";
		if (p_Repos->scopeStack().size() == 0)
			return;
		//p_Repos->scopeStack().top().endLineCount = p_Repos->lineCount();

		p_Repos->scopeStack().top()->endLineCount = p_Repos->lineCount();
		p_Repos->scopeStack().top()->lineCount = p_Repos->scopeStack().top()->endLineCount - p_Repos->scopeStack().top()->startLineCount + 1;
		p_Repos->scopeStack().top()->complexity = p_Repos->metricAnalysis()->analysisComplexity(p_Repos->scopeStack().top(), p_Repos->scopeStack().top()->complexity); //update the complexity
		element* elem = p_Repos->scopeStack().pop();
		if (elem->type == "namespace")
			p_Repos->getnamespace_stack().pop();
		if (elem->type == "class" || elem->type == "struct")
			p_Repos->getclass_stack().pop();
		if (elem->type == "function")
		{
			if (elem->complexity > 10)
				abstractSyntaxtTree.pushToNoticedNode(elem);
			abstractSyntaxtTree.getFilefunction().back()->addChild(elem); //add function to top AST Filefunction node 
		}
	}
};


///////////////////////////////////////////////////////////////
// rule to detect function definitions

class FunctionDefinition : public IRule
{
	std::mutex mtx_;
public:
	/* judge if this is a special key word */
	bool isSpecialKeyWord(const std::string& tok)
	{
		const static std::string keys[]
			= { "for", "while", "switch", "if", "catch" };
		for (int i = 0; i < 5; ++i)
			if (tok == keys[i])
				return true;
		return false;
	}
	//FunctionDefinition rule test
	bool doTest(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName)
	{
		std::lock_guard<std::mutex> l(mtx_);
		ITokCollection& tc = *pTc;
		if (tc[tc.length() - 1] == "{")
		{
			size_t len = tc.find("(");
			if (len < tc.length() && !isSpecialKeyWord(tc[len - 1]))
			{
				doActions(pTc, abstractSyntaxtTree, fileName);
				return false;
			}
		}
		return true;
	}
	std::string ruleName() {
		return "FunctionDefinition";
	}
};

///////////////////////////////////////////////////////////////
// action to push function name onto ScopeStack

class PushFunction : public IAction
{
	Repository* p_Repos;
public:
	/* constructor of PushFunction */
	PushFunction(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	/* FunctionDefinition action */
	void doAction(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName) //PushFunction
	{
		std::string name = (*pTc)[pTc->find("(") - 1];
		if (name == "[]" || name == "=" || name == "+" || name == "+=" || name == "==")
		{
			if ((*pTc)[pTc->find("(") - 2] == "=")
				name = "lambda";
			else
				name = (*pTc)[pTc->find("(") - 2] + name;
		}
		if (name == "]")
			name = "lambda";
		p_Repos->scopeStack().top()->name = name;
		p_Repos->scopeStack().top()->type = "function";
		p_Repos->scopeStack().top()->startLineCount = p_Repos->lineCount();
	}
};
/////////////////////////////////////////////
//action to write function delacaration in map
class WriteFunctionInMapTable : public IAction
{
	Repository* p_Repos;
public:
	/* constructor of PushFunction */
	WriteFunctionInMapTable(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	/* write global function into map action */
	void doAction(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName) //PushFunction
	{
		ITokCollection& tc = *pTc;
		//if (tc.find("::") == tc.length()) //golbal function
		//{
		std::string compare = "default";
		std::string getclassname = p_Repos->getclassname();
		if (getclassname == compare && tc.find("::") == tc.length()) //function does not belong to any clas is a global function
		{
			element* globalele = new element;
			globalele->name = p_Repos->scopeStack().top()->name;
			globalele->type = "function";
			globalele->startLineCount = p_Repos->lineCount();
			globalele->fileName = fileName;
			std::string typeName;
			if (p_Repos->getNameSpacename() == compare)
				typeName = globalele->name;
			else typeName = p_Repos->getNameSpacename() + "::" + globalele->name;
			//should be modified with namespace
			auto type_table = abstractSyntaxtTree.getTypeTable(fileName);
			type_table->insert({ typeName, globalele });
		}

		//}
	}
};

///////////////////////////////////////////////////////////////
// action to send semi-expression that starts a function def
// to console

class PrintFunction : public IAction
{
	Repository* p_Repos;
public:
	// constructor
	PrintFunction(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	/* FunctionDefinition action */
	void doAction(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName)
	{
		std::cout << "\n  FuncDef: " << pTc->show();// .c_str();
	}
};

///////////////////////////////////////////////////////////////
// action to send signature of a function def to console

class PrettyPrintFunction : public IAction
{
public:
	void doAction(ITokCollection*& pTc)
	{
		pTc->remove("public");
		pTc->remove(":");
		pTc->trimFront();
		size_t len = pTc->find(")");
		std::cout << "\n\n  Pretty Stmt:    ";
		for (size_t i = 0; i < len + 1; ++i)
			std::cout << (*pTc)[i] << " ";
		std::cout << "\n";
	}
};

///////////////////////////////////////////////////////////////
// rule to detect special key word

class SpecialKeyWord : public IRule {
	std::mutex mtx_;
public:
	/* test if this is a special key word */
	bool isSpecialKeyWord(const std::string& tok)
	{
		const static std::string keys[]
			= { "for", "while", "switch", "if", "catch" };
		for (int i = 0; i < 5; ++i)
			if (tok == keys[i])
				return true;
		return false;
	}
	/* SpecialKeyWord rule test */
	bool doTest(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName)
	{
		std::lock_guard<std::mutex> l(mtx_);
		ITokCollection& tc = *pTc;
		if (tc[tc.length() - 1] == "{")
		{
			size_t len = tc.find("(");
			if (len < tc.length() && isSpecialKeyWord(tc[len - 1]))
			{
				//std::cout << "  SpecialKeyWord match!\n";
				doActions(pTc, abstractSyntaxtTree, fileName);
				return false;
			}
		}
		return true;
	}
};
///////////////////////////////////////////////
//handle the special key words
class PushSpecialKeyWord : public IAction {
	Repository* p_Repos;
public:
	//constructor
	PushSpecialKeyWord(Repository* pRepos) {
		p_Repos = pRepos;
	}
	/* SpecialKeyWord action */
	void doAction(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName) {
		ITokCollection& tc = *pTc;
		size_t len = tc.find("(");
		p_Repos->scopeStack().top()->name = "  ";
		p_Repos->scopeStack().top()->type = tc[len - 1];
		p_Repos->scopeStack().top()->startLineCount = p_Repos->lineCount();
	}
};

///////////////////////////////////////////////////////////////
// rule to detect special key word

class KeyWordWithoutParentheses : public IRule {
	std::mutex mtx_;
public:
	/* test if this is a Special Key Word Without Parentheses */
	bool isSpecialKeyWordWithoutParentheses(const std::string& tok)
	{
		const static std::string keys[]
			= { "do", "try", "else" };
		for (int i = 0; i < 3; ++i)
			if (tok == keys[i])
				return true;
		return false;
	}

	/* KeyWordWithoutParentheses rule test  */
	bool doTest(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName)
	{
		std::lock_guard<std::mutex> l(mtx_);
		ITokCollection& tc = *pTc;
		if (tc[tc.length() - 1] == "{")
		{
			size_t len = tc.length() - 1;
			if (len > 0)
			{
				while (tc[len - 1] == "\n" && len > 2)
					len--;
				if (isSpecialKeyWordWithoutParentheses(tc[len - 1]))
				{
					//std::cout << "  KeyWordWithoutParentheses match!\n";
					doActions(pTc, abstractSyntaxtTree, fileName);
					return false;
				}
			}
		}
		return true;
	}
};
//////////////////////////////////////////////////////
//action for hanlde key word without parentheses
class PushKeyWordWithoutParentheses : public IAction {
	Repository* p_Repos;
public:
	//constrcutor
	PushKeyWordWithoutParentheses(Repository* pRepos) {
		p_Repos = pRepos;
	}
	/* KeyWordWithoutParentheses action */
	void doAction(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName) {
		ITokCollection& tc = *pTc;
		size_t lenTry = tc.find("try");
		size_t lenDo = tc.find("do");
		size_t lenElse = tc.find("else");
		size_t len = 0;
		if (lenElse < tc.length())
			len = lenElse;
		else if (lenTry < tc.length())
			len = lenTry;
		else
			len = lenDo;
		p_Repos->scopeStack().top()->name = "  ";
		p_Repos->scopeStack().top()->type = tc[len];
		p_Repos->scopeStack().top()->startLineCount = p_Repos->lineCount();
	}
};

///////////////////////////////////////////////////////
//rule for namespace and class
class NameSpaceAndClassMectricAnalysis : public IRule {
	std::mutex mtx_;
public:
	/* judge if the name is class or namespace */
	bool isNameSpaceOrClass(const std::string& tok)
	{
		const static std::string keys[]
			= { "namespace", "class" };
		for (int i = 0; i < 2; ++i)
			if (tok == keys[i])
				return true;
		return false;
	}
	/* NameSpaceAndClass rule test */
	bool doTest(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName)
	{
		std::lock_guard<std::mutex> l(mtx_);
		ITokCollection& tc = *pTc;

		if (tc[tc.length() - 1] == "{" && (tc.find("class") < tc.length() || tc.find("namespace") < tc.length()))
		{
			doActions(pTc, abstractSyntaxtTree, fileName);
			return false;
		}
		return true;
	}
};
//handle doAction for namespace in Metric analysis
class PushNameSpaceAndClassMectricAnalysis : public IAction {
	Repository* p_Repos;
public:
	PushNameSpaceAndClassMectricAnalysis(Repository* pRepos) { // constructor
		p_Repos = pRepos;
	}
	/* NameSpaceAndClass action function */
	void doAction(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName) {
		ITokCollection& tc = *pTc;
		size_t lenClass = tc.find("class");
		size_t lenNameSpace = tc.find("namespace");
		size_t len = 0;
		if (lenClass < tc.length())
			len = lenClass;
		else
			len = lenNameSpace;
		p_Repos->scopeStack().top()->type = ((lenClass < tc.length()) ? "class" : "namespace");
		p_Repos->scopeStack().top()->name = tc[len + 1];
		p_Repos->scopeStack().top()->startLineCount = p_Repos->lineCount();
	}
};

//handle doAction for struct & enum in Metric analysis
class StructEnumMectricAnalysis : public IRule {
	std::mutex mtx_;
public:
	/* judge if the name is struct */
	bool isStructType(const std::string& tok)
	{
		if (tok == "struct" || tok == "enum")
			return true;
		return false;
	}

	/* EnumsType rule test */
	bool doTest(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName)
	{
		std::lock_guard<std::mutex> l(mtx_);
		ITokCollection& tc = *pTc;

		if (tc[tc.length() - 1] == "{" && (tc.find("struct") < tc.length() || tc.find("enum") < tc.length()))
		{
			doActions(pTc, abstractSyntaxtTree, fileName);
			return false;
		}
		return true;
	}
};

// Action for struct and enum in metric analysis
class WritStructEnumMectricAnalysis : public IAction {
	Repository* p_Repos;
public:
	WritStructEnumMectricAnalysis(Repository* pRepos) { // constructor
		p_Repos = pRepos;
	}
	/* WritEnumsInMap action function */
	void doAction(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName) {
		ITokCollection& tc = *pTc;
		size_t len = 0;
		if (tc.find("struct") < tc.length()) {
			len = tc.find("struct");
			p_Repos->scopeStack().top()->type = "struct";
		}
		else {
			len = tc.find("enum");
			p_Repos->scopeStack().top()->type = "enum";
		}
		p_Repos->scopeStack().top()->name = tc[len + 1];
		p_Repos->scopeStack().top()->startLineCount = p_Repos->lineCount();
	}
};

//////////////////////////////////////////
// rule for namespace and class
class NameSpaceAndClass : public IRule {
	std::mutex mtx_;
public:
	/* judge if the name is class or namespace */
	bool isNameSpaceOrClass(const std::string& tok)
	{
		const static std::string keys[]
			= { "namespace", "class" };
		for (int i = 0; i < 2; ++i)
			if (tok == keys[i])
				return true;
		return false;
	}

	/* NameSpaceAndClass rule test */
	bool doTest(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName)
	{
		std::lock_guard<std::mutex> l(mtx_);
		ITokCollection& tc = *pTc;

		if (tc[tc.length() - 1] == "{" && (tc.find("class") < tc.length() || tc.find("namespace") < tc.length()))
		{
			doActions(pTc, abstractSyntaxtTree, fileName);
			return false;
		}
		return true;
	}
};
///////////////////////////////////////////////
//action for namespace and class
class PushNameSpaceAndClass : public IAction {
	Repository* p_Repos;
public:
	PushNameSpaceAndClass(Repository* pRepos) { // constructor
		p_Repos = pRepos;
	}
	/* NameSpaceAndClass action function */
	void doAction(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName) {
		ITokCollection& tc = *pTc;
		size_t lenClass = tc.find("class");
		size_t lenNameSpace = tc.find("namespace");
		size_t len = 0;
		if (lenClass < tc.length())
			len = lenClass;
		else
			len = lenNameSpace;
		p_Repos->scopeStack().top()->type = ((lenClass < tc.length()) ? "class" : "namespace");
		p_Repos->scopeStack().top()->name = tc[len + 1];
		element* namespaceclassele = new element;
		namespaceclassele->type = p_Repos->scopeStack().top()->type;
		namespaceclassele->name = p_Repos->scopeStack().top()->name;
		if (namespaceclassele->type == "namespace")
			p_Repos->getnamespace_stack().push(namespaceclassele); //store namespace in repository
		else {
			p_Repos->getclass_stack().push(namespaceclassele); //store class in repository
			if (p_Repos->getNameSpacename() != "default")
				namespaceclassele->name = p_Repos->getNameSpacename() + "::" + namespaceclassele->name;
		}


		p_Repos->scopeStack().top()->startLineCount = p_Repos->lineCount();
		//orgnize the typetable
		p_Repos->scopeStack().top()->fileName = fileName;
		auto type_table = abstractSyntaxtTree.getTypeTable(fileName);
		std::string typeName = p_Repos->scopeStack().top()->name; //should be modified with namespace
		if (p_Repos->scopeStack().top()->type == "class")
			typeName = namespaceclassele->name;
		type_table->insert({ typeName, p_Repos->scopeStack().top() });
		//UtilityHelpers::ContainerHelpers::showPairs(*type_table);
	}
};

///////////////////////////////////////////////
//action for namespace and class
class NameSpaceAndClassCheckDepend : public IAction {
	Repository* p_Repos;
public:
	NameSpaceAndClassCheckDepend(Repository* pRepos) { // constructor
		p_Repos = pRepos;
	}
	/* NameSpaceAndClass action function */
	void doAction(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName) {
		ITokCollection& tc = *pTc;
		size_t lenClass = tc.find("class");
		size_t lenNameSpace = tc.find("namespace");
		size_t len = 0;
		if (lenClass < tc.length())
			return;
		else
			len = lenNameSpace;
		p_Repos->scopeStack().top()->type = "namespace";
		p_Repos->scopeStack().top()->name = tc[len + 1];
		element* namespaceclassele = new element;
		namespaceclassele->type = p_Repos->scopeStack().top()->type;
		namespaceclassele->name = p_Repos->scopeStack().top()->name;
		if (namespaceclassele->type == "namespace")
			p_Repos->getnamespace_stack().push(namespaceclassele); //store namespace in repository

		p_Repos->scopeStack().top()->startLineCount = p_Repos->lineCount();
		//orgnize the typetable
		p_Repos->scopeStack().top()->fileName = fileName;
	}
};

////////////////////////////////////
//rule for type anlysis detect strut 
class StructType : public IRule {
	std::mutex mtx_;
public:
	/* judge if the name is struct */
	bool isStructType(const std::string& tok) {
		if (tok == "struct")
			return true;
		return false;
	}
	/* StructType rule test */
	bool doTest(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName)
	{
		std::lock_guard<std::mutex> l(mtx_);
		ITokCollection& tc = *pTc;

		if (tc[tc.length() - 1] == "{" && (tc.find("struct") < tc.length()))
		{
			doActions(pTc, abstractSyntaxtTree, fileName);
			return false;
		}
		return true;
	}
};

////////////////////////////////////
//action for type anlysis strut 
class WritStructType : public IAction {
	Repository* p_Repos;
public:
	WritStructType(Repository* pRepos) { // constructor
		p_Repos = pRepos;
	}
	/* WritEnumsInMap action function */
	void doAction(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName) {
		ITokCollection& tc = *pTc;
		size_t lenClass = tc.find("struct");
		size_t len = lenClass;

		p_Repos->scopeStack().top()->type = "struct";
		p_Repos->scopeStack().top()->name = tc[len + 1];
		p_Repos->scopeStack().top()->startLineCount = p_Repos->lineCount();

		element* structele = new element;
		structele->type = p_Repos->scopeStack().top()->type;
		structele->name = p_Repos->scopeStack().top()->name;
		p_Repos->getclass_stack().push(structele); //store struct in repository
		if (p_Repos->getNameSpacename() != "default")
			structele->name = p_Repos->getNameSpacename() + "::" + structele->name;
		//orgnize the typetable
		p_Repos->scopeStack().top()->fileName = fileName;
		std::string typeName = structele->name; //should be modified with namespace
		auto type_table = abstractSyntaxtTree.getTypeTable(fileName);
		type_table->insert({ typeName, p_Repos->scopeStack().top() });
		//UtilityHelpers::ContainerHelpers::showPairs(*type_table);
	}
};


////////////////////////////////////
//rule for type anlysis detect enum 
class EnumsType : public IRule {
	std::mutex mtx_;
public:
	/* judge if the name is enum */
	bool isEnumsType(const std::string& tok)
	{
		if (tok == "enum")
			return true;
		return false;
	}

	/* EnumsType rule test */
	bool doTest(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName)
	{
		std::lock_guard<std::mutex> l(mtx_);
		ITokCollection& tc = *pTc;

		if (tc[tc.length() - 1] == "{" && (tc.find("enum") < tc.length()))
		{
			doActions(pTc, abstractSyntaxtTree, fileName);
			return false;
		}
		return true;
	}
};

////////////////////////////////////
//acion for type  enum 
class WritEnumsInMap : public IAction {
	Repository* p_Repos;
public:
	WritEnumsInMap(Repository* pRepos) { // constructor
		p_Repos = pRepos;
	}
	/* WritEnumsInMap action function */
	void doAction(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName) {
		ITokCollection& tc = *pTc;
		size_t lenClass = tc.find("enum");
		size_t len = lenClass;
		p_Repos->scopeStack().top()->type = "enum";
		p_Repos->scopeStack().top()->name = tc[len + 1];
		p_Repos->scopeStack().top()->startLineCount = p_Repos->lineCount();

		if (p_Repos->getNameSpacename() != "default")
			p_Repos->scopeStack().top()->name = p_Repos->getNameSpacename() + "::" + p_Repos->scopeStack().top()->name;

		//orgnize the typetable
		p_Repos->scopeStack().top()->fileName = fileName;
		std::string typeName = p_Repos->scopeStack().top()->name; //should be modified with namespace
		auto type_table = abstractSyntaxtTree.getTypeTable(fileName);
		type_table->insert({ typeName, p_Repos->scopeStack().top() });
		//UtilityHelpers::ContainerHelpers::showPairs(*type_table); //display the map--the type table
	}
};

////////////////////////////////////
//rule for type anlysis deftype
class TypeDefType : public IRule {
	std::mutex mtx_;
public:
	/* TypeDefType rule test */
	bool doTest(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName)
	{
		std::lock_guard<std::mutex> l(mtx_);
		ITokCollection& tc = *pTc;
		if (tc.find("typedef") < tc.length())
		{
			doActions(pTc, abstractSyntaxtTree, fileName);
			return false;
		}
		return true;
	}
};

////////////////////////////////////
//acion for type anlysis deftype
class WritTypedefInMap : public IAction {
	Repository* p_Repos;
public:
	WritTypedefInMap(Repository* pRepos) { // constructor
		p_Repos = pRepos;
	}
	/* WritEnumsInMap action function */
	void doAction(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName) {
		std::vector<std::string> stdTypeCollection{ "int", "string", "size_t", "std", "unsigned", "long", "double", "float" };
		ITokCollection& tc = *pTc;
		size_t lenTypedef = tc.find(";");
		size_t len = lenTypedef;
		element* mapElement = new element;
		mapElement->type = "typedef";
		mapElement->name = tc[len - 1]; // xvector /typedef vector<X> xvector;
		if (tc[len - 1] == "]" || tc[len - 1] == "[]") {
			while ((tc[len] != "[]") && (tc[len] != "[") && (len - 1) > 0)
				len--;                    //keep away typedef a[] b[];
			mapElement->name = tc[len - 1];
		}
		if (p_Repos->getNameSpacename() != "default")
			mapElement->name = p_Repos->getNameSpacename() + "::" + mapElement->name;

		mapElement->fileName = fileName;
		mapElement->startLineCount = p_Repos->lineCount();
		len = tc.find("typedef");
		std::string typedefsource = tc[len + 1]; //std /typedef std::vector<X> xvector; should push into aliases vector
		if (typedefsource == "std")
		{
			if (tc[len + 3] == "vector")
			{
				len = tc.find("<");
				mapElement->aliases.push_back(tc[len]);
			}
			else if (tc[len + 3] == "map" || tc[len + 3] == "unordered_map")
			{
				len = tc.find("<");
				mapElement->aliases.push_back(tc[len + 1]); //first
				mapElement->aliases.push_back(tc[len + 3]); //second
			}
			else
			{
				mapElement->aliases.push_back("std");
			}
		}
		else if (std::find(stdTypeCollection.begin(), stdTypeCollection.end(), typedefsource) != stdTypeCollection.end()) { //be found is true
			mapElement->aliases.push_back("std"); //mapElement->aliases.push_back("typedefsource");//int 
		}
		else
			mapElement->aliases.push_back("typedefsource");
		std::string typeName = mapElement->name; //should be modified with namespace
		auto type_table = abstractSyntaxtTree.getTypeTable(fileName);
		type_table->insert({ mapElement->name, mapElement });
		//UtilityHelpers::ContainerHelpers::showPairs(*type_table); //display the map
	}
};
////////////////////////////////////
//rule for type anlysis using 
class UsingType : public IRule {
	std::mutex mtx_;
public:
	/* EnumsType rule test */
	bool doTest(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName)
	{
		std::lock_guard<std::mutex> l(mtx_);
		ITokCollection& tc = *pTc;
		if (tc.find("using") < tc.length() && tc.find("=") < tc.length())
		{
			doActions(pTc, abstractSyntaxtTree, fileName);
			return false;
		}
		return true;
	}
};
////////////////////////////////////
//acion for type anlysis enum
class WritUsingTypeInMap : public IAction {
	Repository* p_Repos;
public:
	WritUsingTypeInMap(Repository* pRepos) { // constructor
		p_Repos = pRepos;
	}
	/* Writ Using alias action function */
	void doAction(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName) {
		std::vector<std::string> stdTypeCollection{ "int", "string", "size_t", "std", "unsigned", "long", "double", "float" };
		ITokCollection& tc = *pTc;
		size_t lenTypedef = tc.find("=");
		size_t len = lenTypedef;
		element* mapElement = new element;
		mapElement->fileName = fileName;
		mapElement->startLineCount = p_Repos->lineCount();
		mapElement->type = "using";
		mapElement->name = tc[len - 1]; // Value /using Value = element;
		if (p_Repos->getNameSpacename() != "default")
			mapElement->name = p_Repos->getNameSpacename() + "::" + tc[len - 1];
		std::string typedefsource = tc[len + 1]; //std /typedef std::vector<X> xvector; should push into aliases vector
		if (typedefsource == "std")
		{
			if (tc[len + 3] == "vector")
			{
				len = tc.find("<");
				mapElement->aliases.push_back(tc[len]);
			}
			else if (tc[len + 3] == "map" || tc[len + 3] == "unordered_map")
			{
				len = tc.find("<");
				mapElement->aliases.push_back(tc[len + 1]); //first
				mapElement->aliases.push_back(tc[len + 3]); //second
			}
			else
				mapElement->aliases.push_back("std");
		}
		else if (std::find(stdTypeCollection.begin(), stdTypeCollection.end(), typedefsource) != stdTypeCollection.end()) {
			mapElement->aliases.push_back("std"); //mapElement->aliases.push_back("typedefsource");//int 
		}
		else
			mapElement->aliases.push_back("typedefsource"); //should be modified with namespace
		std::string typeName = mapElement->name; //should be modified with namespace
		auto type_table = abstractSyntaxtTree.getTypeTable(fileName);
		type_table->insert({ mapElement->name, mapElement });
		//UtilityHelpers::ContainerHelpers::showPairs(*type_table);
	}
};
//////////////////////////////////////////////////////
//Forward declaration for Forward Class Struct analysis
class ForwardClassStructType : public IRule {
	std::mutex mtx_;
public:
	/* judge if the name is struct */
	bool isEnumsType(const std::string& tok)
	{
		if (tok == "struct")
			return true;
		return false;
	}
	/* Forward class and struct rule test */
	bool doTest(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName)
	{
		std::lock_guard<std::mutex> l(mtx_);
		ITokCollection& tc = *pTc;

		if (tc[tc.length() - 1] == ";" && (tc.find("struct") < tc.length() || tc.find("class") < tc.length()))
		{
			doActions(pTc, abstractSyntaxtTree, fileName);
			return false;
		}
		return true;
	}
};

/////////////////////////////////////////////////////
//acion for type anlysis  Forward Class Struct analysis
class WritForwardClassStructType : public IAction {
	Repository* p_Repos;
public:
	WritForwardClassStructType(Repository* pRepos) { // constructor
		p_Repos = pRepos;
	}
	/* WritEnumsInMap action function */
	void doAction(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName) {
		ITokCollection& tc = *pTc;
		size_t len = 0;
		element* mapElement = new element;
		if (tc.find("struct") < tc.length()) {
			len = tc.find("struct");
			mapElement->type = "struct";
		}
		else {
			len = tc.find("class");
			mapElement->type = "class";
		}
		mapElement->name = tc[len + 1];
		mapElement->startLineCount = p_Repos->lineCount();
		mapElement->fileName = fileName;
		std::string typeName = mapElement->name; //should be modified with namespace
		if (p_Repos->getNameSpacename() != "default")
			typeName = p_Repos->getNameSpacename() + "::" + typeName; // modified with namespace
		auto type_table = abstractSyntaxtTree.getTypeTable(fileName);
		type_table->insert({ typeName, mapElement });
		//UtilityHelpers::ContainerHelpers::showPairs(*type_table);
	}
};

////////////////////////////////////////////////////////////////

/////////seconde analysis rules and actions////////////////////

///////////////////////////////////////////////////////////////
// rule to to catch using namespace
class CatchUsingNamspace : public IRule {
	std::mutex mtx_;
public:
	/* using namespace and struct rule test */
	bool doTest(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName)
	{
		std::lock_guard<std::mutex> l(mtx_);
		ITokCollection& tc = *pTc;

		if (tc[tc.length() - 1] == ";" && tc.find("using") < tc.length() && tc.find("namespace") < tc.length())
		{
			doActions(pTc, abstractSyntaxtTree, fileName);
			return false;
		}
		return true;
	}
};

/* action for hanling using namespace */
class HandleUsingNamspace : public IAction
{
	Repository* p_Repos;
public:
	HandleUsingNamspace(Repository* pRepos)
	{
		p_Repos = pRepos;
	}//compare the catch the using namespace
	void doAction(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName) {
		ITokCollection& tc = *pTc;
		auto len = tc.find("namespace");
		if (tc[len + 1] != "std" && tc[len + 1] != "\n")
			p_Repos->gethandleusingnamespace().push_back(tc[len + 1]);
	}
};


///////////////////////////////////////////////////////////////
// rule to detect declaration for dependency analysis

class Declaration : public IRule          // declar ends in semicolon
{                                         // has type, name, modifiers &
public:                                   // initializers.  So eliminate
	bool isModifier(const std::string& tok);
	void condenseTemplateTypes(ITokCollection& tc);
	bool isSpecialKeyWord(const std::string& tok) //test if it is a special key word
	{
		const static std::string keys[]
			= { "for", "while", "switch", "if", "catch" };
		for (int i = 0; i < 5; ++i)
			if (tok == keys[i])
				return true;
		return false;
	}
	void removeInvocationParens(ITokCollection& tc);
	// remove modifiers, comments, newlines, returns, and initializers
	void removeSomeTokens(Scanner::SemiExp& se, Scanner::SemiExp& tc) {
		for (size_t i = 0; i < tc.length(); ++i) {
			if (isModifier(tc[i]))
				continue;
			if (se.isComment(tc[i]) || tc[i] == "\n" || tc[i] == "return")
				continue;
			if (tc[i] == "=" || tc[i] == ";")
				break;
			else
				se.push_back(tc[i]);
		}
	}
	// rule to detect declaration
	bool doTest(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName) {
		ITokCollection& in = *pTc;
		Scanner::SemiExp tc;
		for (size_t i = 0; i < in.length(); ++i)
			tc.push_back(in[i]);
		if (tc[tc.length() - 1] == ";" && tc.length() > 2) {
			//std::cout << "\n  ++ " << tc.show();
			removeInvocationParens(tc);
			condenseTemplateTypes(tc);
			// remove modifiers, comments, newlines, returns, and initializers
			Scanner::SemiExp se;
			removeSomeTokens(se, tc);
			//std::cout << "\n  ** " << se.show();
			if (se.length() == 2)  // type & name, so declaration
			{
				doActions(pTc, abstractSyntaxtTree, fileName);
				return true;
			}
		}
		return true;
	}
};
// Declaration class function for condense templateTypes
inline void Declaration::condenseTemplateTypes(ITokCollection& tc)
{
	size_t start = tc.find("<");
	size_t end = tc.find(">");
	if (start >= end || start == 0)
		return;
	else
	{
		if (end == tc.length())
			end = tc.find(">::");
		if (end == tc.length())
			return;
	}
	std::string save = tc[end];
	std::string tok = tc[start - 1];
	for (size_t i = start; i < end + 1; ++i)
		tok += tc[i];
	for (size_t i = start; i < end + 1; ++i)
		tc.remove(start);
	if (save == ">::")
	{
		tok += tc[start + 1];
		tc.remove(start);
	}
	tc[start - 1] = tok;
	//std::cout << "\n  -- " << tc.show();
}
/* helper code for do test, remove invocation parens */
inline void Declaration::removeInvocationParens(ITokCollection& tc)
{
	size_t start = tc.find("(");
	size_t end = tc.find(")");
	if (start >= end || end == tc.length() || start == 0)
		return;
	if (isSpecialKeyWord(tc[start - 1]))
		return;
	for (size_t i = start; i < end + 1; ++i)
		tc.remove(start);
	//std::cout << "\n  -- " << tc.show();
}
/* detect if it is a modifer */
inline bool Declaration::isModifier(const std::string& tok) {// modifiers and initializers.// If you have two things left
	const size_t numKeys = 22;            // its declar else executable.
	const static std::string keys[numKeys] = {
		"const", "extern", "friend", "mutable", "signed", "static",
		"typedef", "typename", "unsigned", "volatile", "&", "*", "std", "::",
		"public", "protected", "private", ":", "typename", "typedef", "++", "--"
	};
	for (int i = 0; i < numKeys; ++i)
		if (tok == keys[i])
			return true;
	return false;
}

///////////////////////////////////////////////////////////////
// action to show declaration

class HandleDeclaration : public IAction
{
	Repository* p_Repos;
public:
	HandleDeclaration(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void removeSomeTokens(Scanner::SemiExp& se, ITokCollection& tc) {
		for (size_t i = 0; i < tc.length(); ++i) {
			if (tc[i] == "=")
				break;
			if (!se.isComment(tc[i]) && tc[i] != "\n" && tc[i] != ";")
				se.push_back(tc[i]);
		}
	}
	//compare the declaration with type map
	void doAction(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName) {
		ITokCollection& tc = *pTc;
		// remove comments
		Scanner::SemiExp se;
		removeSomeTokens(se, tc);
		size_t index = 0;
		while (index != se.length())
		{
			std::string target = se[index];
			if (p_Repos->getNameSpacename() != "default")
				target = p_Repos->getNameSpacename() + "::" + se[index];
			auto len = p_Repos->getRefTypeTableMap()->find(target); //find se[pindex + 1] in type table
			if (len != p_Repos->getRefTypeTableMap()->end())       // could find se[pindex + 1] in type table
			{
				//std::cout << "\n\n-----Find!" << std::endl;
				auto tmpmap = (*(p_Repos->getRefTypeTableMap()))[target];

				p_Repos->setDependencyList(fileName, tmpmap->fileName);
			}
			index++;
		}
		// show cleaned semiExp
		//std::cout << "\n  Declaration: " << se.show();
	}
};


///////////////////////////////////////////////////////////////
// action to build declaration dependency on using namespace

class HandleDeclarationUSING : public IAction
{
	Repository* p_Repos;
public:
	HandleDeclarationUSING(Repository* pRepos)
	{
		p_Repos = pRepos;
	}//compare the declaration with type map
	void doAction(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName) {
		ITokCollection& tc = *pTc;
		// remove comments
		Scanner::SemiExp se;
		for (size_t i = 0; i < tc.length(); ++i) {
			if (tc[i] == "=")
				break;
			if (!se.isComment(tc[i]) && tc[i] != "\n" && tc[i] != ";")
				se.push_back(tc[i]);
		}
		size_t index = 0;
		if (p_Repos->gethandleusingnamespace().size() > 0)
		{
			for (auto ele : p_Repos->gethandleusingnamespace())
			{
				size_t indexusing = 0;
				while (indexusing != se.length())
				{
					auto target = ele + "::" + se[indexusing];
					auto len = p_Repos->getRefTypeTableMap()->find(target); //find se[pindex + 1] in type table
					if (len != p_Repos->getRefTypeTableMap()->end())       // could find se[pindex + 1] in type table
					{
						//std::cout << "\n\n-----Find!" << std::endl;
						auto tmpmap = (*(p_Repos->getRefTypeTableMap()))[target];

						p_Repos->setDependencyList(fileName, tmpmap->fileName);
					}
					indexusing++;
				}
			}
		}
	}
};



// Handle FuncDeclaration For Dependency
class HandleFuncDeclarationForDependency : public IAction
{
	Repository* p_Repos;
public:
	HandleFuncDeclarationForDependency(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName) {
		ITokCollection& tc = *pTc;
		// remove comments
		Scanner::SemiExp se;
		for (size_t i = 0; i < tc.length(); ++i) {
			if (tc[i] == "=")
				break;
			if (!se.isComment(tc[i]) && tc[i] != "\n" && tc[i] != ";" && tc[i] != "main") //keep main out
				se.push_back(tc[i]);
		}
		size_t index = 0;
		while (index != se.length())
		{
			/*std::string target;
			if (p_Repos->getNameSpacename() != "default")
				target = p_Repos->getNameSpacename() + "::" + se[index];*/
			auto len = p_Repos->getRefTypeTableMap()->find(se[index]); //find se[pindex + 1] in type table
			if (len != p_Repos->getRefTypeTableMap()->end())       // could find se[pindex + 1] in type table
			{
				//std::cout << "\n\n-----Find!" << std::endl;
				auto tmpmap = (*(p_Repos->getRefTypeTableMap()))[se[index]];

				p_Repos->setDependencyList(fileName, tmpmap->fileName);
			}
			index++;
		}
	}
};

//--< handle using namespace process tokens >-------------------
class HandleFuncDeclarationForDependencyUSING : public IAction
{
	Repository* p_Repos;
public:
	HandleFuncDeclarationForDependencyUSING(Repository* pRepos)
	{
		p_Repos = pRepos;
	}//action for handle using namespace in title
	void doAction(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName) {
		ITokCollection& tc = *pTc;
		// remove comments
		Scanner::SemiExp se;
		for (size_t i = 0; i < tc.length(); ++i) {
			if (tc[i] == "=")
				break;
			if (!se.isComment(tc[i]) && tc[i] != "\n" && tc[i] != ";")
				se.push_back(tc[i]);
		}
		if (p_Repos->gethandleusingnamespace().size() > 0)
		{
			for (auto ele : p_Repos->gethandleusingnamespace())
			{
				size_t indexusing = 0;
				while (indexusing != se.length())
				{
					auto target = ele + "::" + se[indexusing];
					auto len = p_Repos->getRefTypeTableMap()->find(target); //find se[pindex + 1] in type table
					if (len != p_Repos->getRefTypeTableMap()->end())       // could find se[pindex + 1] in type table
					{
						//std::cout << "\n\n-----Find!" << std::endl;
						auto tmpmap = (*(p_Repos->getRefTypeTableMap()))[target];

						p_Repos->setDependencyList(fileName, tmpmap->fileName);
					}
					indexusing++;
				}
			}
		}
	}
};

//////////////////////////////////////////////////////////
//action for build relationship from header to cpp file
class HandleFuncDeforHeadertoCpp : public IAction
{
	Repository* p_Repos;
public:
	HandleFuncDeforHeadertoCpp(Repository* pRepos)
	{
		p_Repos = pRepos;
	}//handle special situation for header depends on cpp
	void doAction(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName) {
		ITokCollection& tc = *pTc;
		// remove comments
		Scanner::SemiExp se;
		for (size_t i = 0; i < tc.length(); ++i) {
			if (tc[i] == "=")
				break;
			if (!se.isComment(tc[i]) && tc[i] != "\n" && tc[i] != ";")
				se.push_back(tc[i]);
		}
		if (tc.find("::") < tc.length())
		{
			if (FileSystem::Path::getExt(fileName) == "cpp")
			{
				auto  file = FileSystem::Path::getBef(fileName);
				file = file + ".h";
				p_Repos->setDependencyList(file, fileName);
			}
		}
	}
};


///////////////////////////////////////////////////////////////
// rule to detect executable

class Executable : public IRule           // declar ends in semicolon
{
	Repository* p_Repos;                  // has type, name, modifiers &
public:                                   // initializers.  So eliminate
	bool isModifier(const std::string& tok) // modifiers and initializers.
	{                                       // If you have two things left
		const size_t numKeys = 22;            // its declar else executable.
		const static std::string keys[numKeys] = {
			"const", "extern", "friend", "mutable", "signed", "static",
			"typedef", "typename", "unsigned", "volatile", "&", "*", "std", "::",
			"public", "protected", "private", ":", "typename", "typedef", "++", "--"
		};
		for (int i = 0; i < numKeys; ++i)
			if (tok == keys[i])
				return true;
		return false;
	}
	Executable(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void condenseTemplateTypes(ITokCollection& tc)
	{
		size_t start = tc.find("<");
		size_t end = tc.find(">");
		if (start >= end || start == 0)
			return;
		else
		{
			if (end == tc.length())
				end = tc.find(">::");
			if (end == tc.length())
				return;
		}
		std::string save = tc[end];
		std::string tok = tc[start - 1];
		for (size_t i = start; i < end + 1; ++i)
			tok += tc[i];
		for (size_t i = start; i < end + 1; ++i)
			tc.remove(start);
		if (save == ">::")
		{
			tok += tc[start + 1];
			tc.remove(start);
		}
		tc[start - 1] = tok;
		//std::cout << "\n  -- " << tc.show();
	}

	bool isSpecialKeyWord(const std::string& tok)
	{
		const static std::string keys[]
			= { "for", "while", "switch", "if", "catch" };
		for (int i = 0; i < 5; ++i)
			if (tok == keys[i])
				return true;
		return false;
	}
	void removeInvocationParens(ITokCollection& tc)
	{
		size_t start = tc.find("(");
		size_t end = tc.find(")");
		if (start >= end || end == tc.length() || start == 0)
			return;
		if (isSpecialKeyWord(tc[start - 1]))
			return;
		for (size_t i = start; i < end + 1; ++i)
			tc.remove(start);
		//std::cout << "\n  -- " << tc.show();
	}
	/* collect tokens into another container with trim  */
	void collectiontokens(Scanner::SemiExp& se, Scanner::SemiExp& tc) {
		for (size_t i = 0; i < tc.length(); ++i)
		{
			if (isModifier(tc[i]))
				continue;
			if (se.isComment(tc[i]) || tc[i] == "\n" || tc[i] == "return")
				continue;
			if (tc[i] == "=" || tc[i] == ";")
				break;
			else
				se.push_back(tc[i]);
		}
	}
	/* remove modifiers, comments, newlines, returns, and initializers */
	bool doTest(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName) {
		ITokCollection& in = *pTc;
		Scanner::SemiExp tc;
		for (size_t i = 0; i < in.length(); ++i)
			tc.push_back(in[i]);

		if (tc[tc.length() - 1] == ";" && tc.length() > 2)
		{
			//std::cout << "\n  ++ " << tc.show();
			removeInvocationParens(tc);
			condenseTemplateTypes(tc);

			// remove modifiers, comments, newlines, returns, and initializers
			Scanner::SemiExp se;
			collectiontokens(se, tc);
			//std::cout << "\n  ** " << se.show();
			if (se.length() != 2)  // not a declaration
			{
				doActions(pTc, abstractSyntaxtTree, fileName);
				return true;
			}
		}
		return true;
	}
};


/////////////////////////////////////////////////////////////////////
// action to hanlde executable to get relation ship with table map

class HandleExecutable : public IAction
{
	Repository* p_Repos;
public:
	HandleExecutable(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	/* action to hanlde executable to get relation ship with table map */
	void doAction(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName) {
		ITokCollection& tc = *pTc;
		// remove comments
		Scanner::SemiExp se;
		for (size_t i = 0; i < tc.length(); ++i) {
			if (!se.isComment(tc[i]) && tc[i] != "\n" && tc[i] != ";")
				se.push_back(tc[i]);
		}
		size_t index = 0;
		while (index < se.length())
		{
			if (se[index] == "." || se[index] == "->")
			{
				index += 2;
				continue;
			}
			std::string target = se[index];
			if (p_Repos->getNameSpacename() != "default") //check namespace
				target = p_Repos->getNameSpacename() + "::" + se[index];

			auto len = p_Repos->getRefTypeTableMap()->find(target); //find se[pindex + 1] in type table
			if (len != p_Repos->getRefTypeTableMap()->end())       // could find se[pindex + 1] in type table
			{
				//std::cout << "\n\n-----Find!" << std::endl;
				//std::cout << "\n  Executable: " << se.show();
				auto tmpmap = (*(p_Repos->getRefTypeTableMap()))[target];

				p_Repos->setDependencyList(fileName, tmpmap->fileName);
			}
			index++;
		}
	}
};

class HandleExecutableUSING : public IAction
{
	Repository* p_Repos;
public:
	HandleExecutableUSING(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	/* action to hanlde executable to get relation ship with table map */
	void doAction(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName) {
		ITokCollection& tc = *pTc;
		// remove comments
		Scanner::SemiExp se;
		for (size_t i = 0; i < tc.length(); ++i) {
			if (!se.isComment(tc[i]) && tc[i] != "\n" && tc[i] != ";")
				se.push_back(tc[i]);
		}
		if (p_Repos->gethandleusingnamespace().size() > 0)
		{
			for (auto ele : p_Repos->gethandleusingnamespace())
			{
				size_t indexusing = 0;
				while (indexusing != se.length())
				{
					auto target = ele + "::" + se[indexusing];
					auto len = p_Repos->getRefTypeTableMap()->find(target); //find se[pindex + 1] in type table
					if (len != p_Repos->getRefTypeTableMap()->end())       // could find se[pindex + 1] in type table
					{
						//std::cout << "\n\n-----Find!" << std::endl;
						auto tmpmap = (*(p_Repos->getRefTypeTableMap()))[target];

						p_Repos->setDependencyList(fileName, tmpmap->fileName);
					}
					indexusing++;
				}
			}
		}
	}
};

//rule for specific namespace and class
class SpecifiedNameSpacedFunc : public IRule {
	std::mutex mtx_;
public:
	/* detect two "::" in semi expression */
	bool doTest(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName)
	{
		std::lock_guard<std::mutex> l(mtx_);
		ITokCollection& tc = *pTc;
		auto len = tc.find("::");
		if (len > 1 && len < tc.length() && ((len + 3) < tc.length())) //void F::C::func();
		{
			if (tc[len + 2] == "::") {
				doActions(pTc, abstractSyntaxtTree, fileName);
				return true;
			}
		}
		return true;
	}
};
//action for specific namespace and class to call funciton
class HandleSpecifiedNameSpacedFunc : public IAction
{
	Repository* p_Repos;
public:
	HandleSpecifiedNameSpacedFunc(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	/* action for specific namespace and class calling function */
	void doAction(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName) {
		ITokCollection& tc = *pTc;
		// remove comments
		auto leng = tc.find("::");
		std::string target = tc[leng - 1] + tc[leng] + tc[leng + 1];

		auto len = p_Repos->getRefTypeTableMap()->find(target); //find se[pindex + 1] in type table
		if (len != p_Repos->getRefTypeTableMap()->end())       // could find se[pindex + 1] in type table
		{
			//std::cout << "\n\n-----Find!" << std::endl;
			auto tmpmap = (*(p_Repos->getRefTypeTableMap()))[target];

			p_Repos->setDependencyList(fileName, tmpmap->fileName);
		}
	}
};


//rule for specific namespace and class defination like N::C obj;
class SpecifiedNameSpaced : public IRule {
	std::mutex mtx_;
public:
	/* detect only one "::" in semi expression */
	bool doTest(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName)
	{
		std::lock_guard<std::mutex> l(mtx_);
		ITokCollection& tc = *pTc;
		auto len = tc.find("::");
		if (len < tc.length())
		{
			if (len - 1 > 0)    //keep std::string away
			{
				if (tc[len - 1] != "std")
				{
					for (size_t i = len + 1; i < tc.length(); i++)  //to check only one :: token in semi
					{
						if (tc[i] == "::") //if find another quit
						{
							return true;
						}
					}
					doActions(pTc, abstractSyntaxtTree, fileName);
					return true;
				}
			}
		}
		return true;
	}
};


//action for specific namespace and class defination like N::C obj;
class HandleSpecifiedNameSpaced : public IAction
{
	Repository* p_Repos;
public:
	HandleSpecifiedNameSpaced(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	/* action for specific namespace and class */
	void doAction(ITokCollection*& pTc, AST<element>& abstractSyntaxtTree, std::string fileName) {
		ITokCollection& tc = *pTc;
		// remove comments
		auto leng = tc.find("::");
		std::string target;

		if ((leng - 1) > 0 && (leng + 1) < tc.length())
		{
			target = tc[leng - 1] + tc[leng] + tc[leng + 1];
			auto len = p_Repos->getRefTypeTableMap()->find(target); //find se[pindex + 1] in type table
			if (len != p_Repos->getRefTypeTableMap()->end())       // could find se[pindex + 1] in type table
			{
				//std::cout << "\n\n-----Find!" << std::endl;
				auto tmpmap = (*(p_Repos->getRefTypeTableMap()))[target];

				p_Repos->setDependencyList(fileName, tmpmap->fileName);
			}
		}	
	}
};


#endif
