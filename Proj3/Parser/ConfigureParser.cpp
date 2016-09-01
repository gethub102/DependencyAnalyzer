/////////////////////////////////////////////////////////////////////
//  ConfigureParser.cpp - builds and configures parsers            //
//  ver 2.2                                                        //
//                                                                 //
//  Lanaguage:     Visual C++ 2015                                 //
//  Platform:      Mac, Window 10                                  //
//  Application:   Prototype for CSE687 Pr3 Fall2016               //
//  Author:        Wenbin Li, Syracuse University                  //
//                 wli102@syr.edu                                  //
//                 Source from Dr. Jim                             //
/////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================
This module implemenation of Parser class functions.
Make parser class have the ability of attach file and
Aggregation the rules and acions. Moreover implemenation
could add parser rules, add actions to rule.

Public Interface:
=================
ConfigParseToConsole conConfig;
conConfig.Build(abstractSyntaxtTree);
conConfig.Attach(someFileName);

Build Process:
==============
Required files
- ConfigureParser.h, ConfigureParser.cpp, Parser.h, Parser.cpp,
ActionsAndRules.h, ActionsAndRules.cpp, AbstractSyntaxTree.h,
MetricAnalysis.h, SemiExpression.h, SemiExpression.cpp, tokenizer.h, tokenizer.cpp
Build commands
- devenv Project3.sln /rebuild debug

Maintenance History:
====================
ver 2.2 : 07 Apr 16
- Added BuildMetricAnalysis
- Modified Build for TypeAnalysis
- Added BuildDependencyAnalysis
ver 2.1 : 19 Feb 16
- Added PrintFunction action to FunctionDefinition rule
ver 2.0 : 01 Jun 11
- Major revisions to begin building a strong code analyzer
ver 1.1 : 01 Feb 06
- cosmetic changes to ConfigureParser.cpp
ver 1.0 : 12 Jan 06
- first release

*/
#include <fstream>
#include "Parser.h"
#include "../SemiExp/SemiExp.h"
#include "../Tokenizer/Tokenizer.h"
#include "ActionsAndRules.h"
#include "ConfigureParser.h"
#include "../AbstractSyntaxTree/AbstractSyntaxTree.h"
#include "../MetricAnalysis/MetricAnalysis.h"

using namespace Scanner;

//----< destructor releases all parts >------------------------------

ConfigParseToConsole::~ConfigParseToConsole()
{
	// when Builder goes out of scope, everything must be deallocated
	delete pRepo;
	delete pParser;
	delete pSemi;
	delete pToker;
	pIn->close();
	delete pIn;
}
//----< attach toker to a file stream or stringstream >------------

bool ConfigParseToConsole::Attach(const std::string& name, bool isFile)
{
	if (pToker == 0)
		return false;
	pIn = new std::ifstream(name);
	if (!pIn->good())
		return false;
	return pToker->attach(pIn);
}

//----< Here's part of type analysis >----------------
void  ConfigParseToConsole::buildStrutConfigure() {
	//configure to build struct
	pStructType = new StructType;
	pWritStructType = new WritStructType(pRepo);
	pStructType->addAction(pWritStructType);
	pParser->addRule(pStructType);
	// configure to build enums
	pEnumsType = new EnumsType;
	pWritEnumsInMap = new WritEnumsInMap(pRepo);
	pEnumsType->addAction(pWritEnumsInMap);
	pParser->addRule(pEnumsType);
	// configure to build typefed
	pTypeDefType = new TypeDefType;
	pWritTypedefInMap = new WritTypedefInMap(pRepo);
	pTypeDefType->addAction(pWritTypedefInMap);
	pParser->addRule(pTypeDefType);
	// configure to build using =
	pUsingType = new UsingType;
	pWritUsingTypeInMap = new WritUsingTypeInMap(pRepo);
	pUsingType->addAction(pWritUsingTypeInMap);
	pParser->addRule(pUsingType);
	// configure to build forward declaration for class and struct
	pForwardClassStructType = new ForwardClassStructType;
	pWritForwardClassStructType = new WritForwardClassStructType(pRepo);
	pForwardClassStructType->addAction(pWritForwardClassStructType);
	pParser->addRule(pForwardClassStructType);
}
//----< Here's configure for type analysis >----------------
Parser* ConfigParseToConsole::Build(AST<element>& abstractSyntaxtTree) {
	try {
		// add Parser's main parts
		pToker = new Toker; // why it does not use Toker()
		pToker->returnComments(false);
		pSemi = new SemiExp(pToker);
		pParser = new Parser(pSemi);
		pRepo = new Repository(pToker);
		pBeginningOfScope = new BeginningOfScope(); //rule
		pHandlePush = new HandlePush(pRepo); //IAction push to stack
		pBeginningOfScope->addAction(pHandlePush);
		pParser->addRule(pBeginningOfScope);
		pEndOfScope = new EndOfScope();
		pHandlePop = new HandlePop(pRepo);
		pEndOfScope->addAction(pHandlePop);
		pParser->addRule(pEndOfScope);
		pFunctionDefinition = new FunctionDefinition; //FunctionDefinition Rule
		pPushFunction = new PushFunction(pRepo);  // no action
		//pPrintFunction = new PrintFunction(pRepo);
		pWriteFunctionInMapTable = new WriteFunctionInMapTable(pRepo); //write the function into map for Projec3
		pFunctionDefinition->addAction(pPushFunction);
		pFunctionDefinition->addAction(pWriteFunctionInMapTable);
		//pFunctionDefinition->addAction(pPrintFunction);
		pParser->addRule(pFunctionDefinition);
		// configure to detect and act on special modifications
		pSpecialKeyWord = new SpecialKeyWord;
		pPushSpecialKeyWord = new PushSpecialKeyWord(pRepo);
		pSpecialKeyWord->addAction(pPushSpecialKeyWord);
		pParser->addRule(pSpecialKeyWord);
		// configure to detect and act on special modifications without parentheses
		pKeyWordWithoutParentheses = new KeyWordWithoutParentheses;
		pPushKeyWordWithoutParentheses = new PushKeyWordWithoutParentheses(pRepo);
		pKeyWordWithoutParentheses->addAction(pPushKeyWordWithoutParentheses);
		pParser->addRule(pKeyWordWithoutParentheses);
		// configure to detect and act on class and namespace
		pNameSpaceAndClass = new NameSpaceAndClass;
		pPushNameSpaceAndClass = new PushNameSpaceAndClass(pRepo);
		pNameSpaceAndClass->addAction(pPushNameSpaceAndClass);
		pParser->addRule(pNameSpaceAndClass);
		buildStrutConfigure();
		return pParser;
	}
	catch (std::exception& ex) {
		std::cout << "\n\n  " << ex.what() << "\n\n";
		return 0;
	}
}
// configure to MetricAnalysis
Parser * ConfigParseToConsole::BuildMetricAnalysis(AST<element>& abstractSyntaxtTree)
{
	try {// add Parser's main parts
		pToker = new Toker; // why it does not use Toker()
		pToker->returnComments(false);
		pSemi = new SemiExp(pToker);
		pParser = new Parser(pSemi);
		pRepo = new Repository(pToker);
		pBeginningOfScope = new BeginningOfScope(); //rule
		pHandlePush = new HandlePush(pRepo); //IAction push to stack
		pBeginningOfScope->addAction(pHandlePush);
		pParser->addRule(pBeginningOfScope);
		pEndOfScope = new EndOfScope();
		pHandlePop = new HandlePop(pRepo);
		pEndOfScope->addAction(pHandlePop);
		pParser->addRule(pEndOfScope);
		pFunctionDefinition = new FunctionDefinition; //FunctionDefinition Rule
		pPushFunction = new PushFunction(pRepo);  // no action
		pPrintFunction = new PrintFunction(pRepo);
		pFunctionDefinition->addAction(pPushFunction);
		//pFunctionDefinition->addAction(pPrintFunction);
		pParser->addRule(pFunctionDefinition);
		// configure to detect and act on special modifications
		pSpecialKeyWord = new SpecialKeyWord;
		pPushSpecialKeyWord = new PushSpecialKeyWord(pRepo);
		pSpecialKeyWord->addAction(pPushSpecialKeyWord);
		pParser->addRule(pSpecialKeyWord);
		// configure to detect and act on special modifications without parentheses
		pKeyWordWithoutParentheses = new KeyWordWithoutParentheses;
		pPushKeyWordWithoutParentheses = new PushKeyWordWithoutParentheses(pRepo);
		pKeyWordWithoutParentheses->addAction(pPushKeyWordWithoutParentheses);
		pParser->addRule(pKeyWordWithoutParentheses);
		// configure to detect and act on class and namespace for MectricAnalysis
		pNameSpaceAndClassMectricAnalysis = new NameSpaceAndClassMectricAnalysis;
		pPushNameSpaceAndClassMectricAnalysis = new PushNameSpaceAndClassMectricAnalysis(pRepo);
		pNameSpaceAndClassMectricAnalysis->addAction(pPushNameSpaceAndClassMectricAnalysis);
		pParser->addRule(pNameSpaceAndClassMectricAnalysis);
		//configure to build struct & enum
		pStructEnumMectricAnalysis = new StructEnumMectricAnalysis;
		pWritStructEnumMectricAnalysis = new WritStructEnumMectricAnalysis(pRepo);
		pStructEnumMectricAnalysis->addAction(pWritStructEnumMectricAnalysis);
		pParser->addRule(pStructEnumMectricAnalysis);
	}
	catch (std::exception& ex) {
		std::cout << "\n\n  " << ex.what() << "\n\n";
		return 0;
	}
	return pParser;
}
//configure rules and actions for dependency  analysis
void ConfigParseToConsole::buildDependencyConfigure() {
	// add Parser's main parts
	pToker = new Toker; // why it does not use Toker()
	pToker->returnComments(false);
	pSemi = new SemiExp(pToker);
	pParser = new Parser(pSemi);
	pRepo = new Repository(pToker);
	//handle using namespace
	pCatchUsingNamspace = new CatchUsingNamspace;
	pHandleUsingNamspace = new HandleUsingNamspace(pRepo);
	pCatchUsingNamspace->addAction(pHandleUsingNamspace);
	pParser->addRule(pCatchUsingNamspace);
	//begin scop and end scope
	pBeginningOfScope = new BeginningOfScope(); //rule
	pHandlePush = new HandlePush(pRepo); //IAction push to stack
	pBeginningOfScope->addAction(pHandlePush);
	pParser->addRule(pBeginningOfScope);
	pEndOfScope = new EndOfScope();
	pHandlePop = new HandlePop(pRepo);
	pEndOfScope->addAction(pHandlePop);
	pParser->addRule(pEndOfScope);
	//namespace and class
	pNameSpaceAndClass = new NameSpaceAndClass;
	pNameSpaceAndClassCheckDepend = new NameSpaceAndClassCheckDepend(pRepo);
	pNameSpaceAndClass->addAction(pNameSpaceAndClassCheckDepend);
	pParser->addRule(pNameSpaceAndClass);
}

// configure to detect and act on dependency analysis
Parser * ConfigParseToConsole::BuildDependencyAnalysis(AST<element>& abstractSyntaxtTree)
{
	try {
		buildDependencyConfigure();
		//specified namespace and class call function like N::C::fun();
		pSpecifiedNameSpacedFunc = new SpecifiedNameSpacedFunc;
		pHandleSpecifiedNameSpacedFunc = new HandleSpecifiedNameSpacedFunc(pRepo);
		pSpecifiedNameSpacedFunc->addAction(pHandleSpecifiedNameSpacedFunc);
		pParser->addRule(pSpecifiedNameSpacedFunc);
		//specified nameapace and class like, N::C obj;
		pSpecifiedNameSpaced = new SpecifiedNameSpaced;
		pHandleSpecifiedNameSpaced = new HandleSpecifiedNameSpaced(pRepo);
		pSpecifiedNameSpaced->addAction(pHandleSpecifiedNameSpaced);
		pParser->addRule(pSpecifiedNameSpaced);
		//function detect
		pFunctionDefinition = new FunctionDefinition; //FunctionDefinition Rule
		//pPrintFunction = new PrintFunction(pRepo);
		pHandleFuncDeclarationForDependency = new HandleFuncDeclarationForDependency(pRepo); 
		pFunctionDefinition->addAction(pHandleFuncDeclarationForDependency);
		pHandleFuncDeclarationForDependencyUSING = new HandleFuncDeclarationForDependencyUSING(pRepo); //using namespace --
		pFunctionDefinition->addAction(pHandleFuncDeclarationForDependencyUSING); 
		pHandleFuncDeforHeadertoCpp = new HandleFuncDeforHeadertoCpp(pRepo); //header to cpp
		pFunctionDefinition->addAction(pHandleFuncDeforHeadertoCpp);
		pParser->addRule(pFunctionDefinition);
		// configure to detect and act on declarations and Executables
		pDeclaration = new Declaration;
		pHandleDeclaration = new HandleDeclaration(pRepo);
		pDeclaration->addAction(pHandleDeclaration);
		pHandleDeclarationUSING = new HandleDeclarationUSING(pRepo);
		pDeclaration->addAction(pHandleDeclarationUSING);
		pParser->addRule(pDeclaration);
		pExecutable = new Executable(pRepo);
		pHandleExecutable = new HandleExecutable(pRepo);
		pExecutable->addAction(pHandleExecutable);
		pHandleExecutableUSING = new HandleExecutableUSING(pRepo);
		pExecutable->addAction(pHandleExecutableUSING);
		pParser->addRule(pExecutable);
		return pParser;
	}
	catch (std::exception& ex) {
		std::cout << "\n\n  " << ex.what() << "\n\n";
		return 0;
	}
}

#ifdef TEST_CONFIGUREPARSER

#include <queue>
#include <string>

int main(int argc, char* argv[])
{
	std::cout << "\n  Testing ConfigureParser module\n "
		<< std::string(32, '=') << std::endl;

	// collecting tokens from files, named on the command line

	if (argc < 2)
	{
		std::cout
			<< "\n  please enter name of file to process on command line\n\n";
		return 1;
	}

	for (int i = 1; i < argc; ++i)
	{
		std::cout << "\n  Processing file " << argv[i];
		std::cout << "\n  " << std::string(16 + strlen(argv[i]), '-');

		ConfigParseToConsole configure;
		Parser* pParser = configure.Build();
		try
		{
			if (pParser)
			{
				if (!configure.Attach(argv[i]))
				{
					std::cout << "\n  could not open file " << argv[i] << std::endl;
					continue;
				}
			}
			else
			{
				std::cout << "\n\n  Parser not built\n\n";
				return 1;
			}
			// now that parser is built, use it

			while (pParser->next())
				pParser->parse();
			std::cout << "\n\n";
		}
		catch (std::exception& ex)
		{
			std::cout << "\n\n    " << ex.what() << "\n\n";
		}
		std::cout << "\n\n";
	}
}

#endif
