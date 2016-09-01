/////////////////////////////////////////////////////////////////////
//  Parser.cpp - Analyzes C++ language constructs                  //
//  ver 1.5                                                        //
//  Language:      C++, Visual Studio 2015                         //
//  Platform:      Macbook win10                                   //
//  Application:   Parser component, OOD Project#2                 //
//  Author:        Webnin Li, Syracuse University,                 //
//                 wli102@syr.edu                                  //
//                 Source from Dr. Jim                             //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package demonstrated the analysis of target file. To analysis the 
* function scope class scope and namespace scope.
*
* Build Process:
* --------------
* Required Files: Utilities.h, Utilities.cpp
* Tokenizer.h, SemiExp.h, Parser.h, ActionsAndRules.h
* ConfigureParser.h, FileSystem.h, FileMgr.h, AbstractSyntaxTree.h,
* MetricAnalysis.h, Tokenizer.cpp, SemiExp.cpp, Tokenizer.cpp,
* MetricAnalysis.cpp, FileSystem.cpp, Scopestack.cpp
* Build commands 
* - devenv Project3.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 3.5 : 10 Feb 2016
* - fixed the main() lines less than 50
* ver 1.4 : 03 Feb 2016
* - fixed the main() complexity
* ver 1.3 : 02 Feb 2016
* - fix the addPattern function to search multiple patterns files.
* ver 1.2 : 28 Jan 2016
* - modified the Parser constructor parser() to parser(AbstractSyntaxTree)
* ver 1.1 : 27 Jan 2016
* - add the getPath() funciton for get sepcific file.
* ver 1.0 : 11 Jun 2014
* - first release by Jim
*/

#include <iostream>
#include <string>
#include "../Utilities/Utilities.h"
#include "../Tokenizer/Tokenizer.h"
#include "../SemiExp/SemiExp.h"
#include "Parser.h"
#include "ActionsAndRules.h"
#include "ConfigureParser.h"
#include "../FileMgr/FileSystem.h"
#include "../FileMgr/FileMgr.h"
#include "../AbstractSyntaxTree/AbstractSyntaxTree.h"
#include "../MetricAnalysis/MetricAnalysis.h"

using namespace Scanner;
using namespace Utilities;

//----< register parsing rule >--------------------------------

void Parser::addRule(IRule* pRule)
{
	rules.push_back(pRule);
}
//----< get next ITokCollection >------------------------------

bool Parser::next()
{
	// default operation of doFold(pTokColl) is to simply return
	// - code analysis provides specific rules for handling
	//   for statements and operator expressions which are
	//   bound in ConfigureParser.cpp

	bool succeeded = pTokColl->get();
	if (!succeeded)
		return false;
	return true;
}



//----< parse the SemiExp by applying all rules to it >--------

bool Parser::parse(AST<element>& abstractSyntaxtTree, std::string fileName)
{
	bool succeeded = false;
	//std::cout << "\nTo Match--rules.size(): " << rules.size() << ", " << "Semi size is ->" << pTokColl->length()<<", pTokColl:------------- " << pTokColl->show() <<std::endl;
	for (size_t i = 0; i < rules.size(); ++i)
	{
		if (rules[i]->doTest(pTokColl, abstractSyntaxtTree, fileName))
			succeeded = true;
		else
			break;
		//std::cout << "rule: " << rules[i]->ruleName() << std::endl;
	}
	return succeeded;
}

//----< register action with a rule >--------------------------

void IRule::addAction(IAction *pAction)
{
	actions.push_back(pAction);
}
//----< invoke all actions associated with a rule >------------ 

void IRule::doActions(ITokCollection*& pTokColl, AST<element>& abstractSyntaxtTree, std::string fileName)
{
	if (actions.size() > 0)
		for (size_t i = 0; i < actions.size(); ++i)
			actions[i]->doAction(pTokColl, abstractSyntaxtTree, fileName);
}

//----< test stub >--------------------------------------------


#ifdef TEST_PARSER

#include <queue>
#include <string>
#define Util StringHelper

int testPath()
{
	DataStore ds;
	FileMgr fm("../TEST/TEST.cpp", ds); // path and datastore
						 //fm.addPattern("*.h");
						 //fm.addPattern("*.partial");
	fm.search();
	for (auto fs : ds)
	{
		std::cout << "\n  " << fs;
	}
	std::cout << "\n\n";
	return 0;
}

DataStore getPath(std::string path, DataStore& ds, std::vector<std::string> pattern) {
	FileMgr fm(path, ds); // path and datastore
	auto iter = pattern.begin();
	while (iter != pattern.end()) {
		fm.addPattern(*iter);
		iter++;
	}
	//fm.addPattern("*.partial");
	fm.search();
	return ds;
}
/*-- search the pattern command-- */
void searchPatternCommand(std::vector<std::string>& pattern, int argc, char** argv){
	std::string patterncommand = "-p";
	pattern.push_back("*.*");
	bool first = true;
	for (int j = 1; j < (argc - 1); j++)
	{
		try
		{
			if (argv[j] == patterncommand) {
				if (first == true)
				{
					first = false;
					pattern.pop_back();
				}
				pattern.push_back(argv[j + 1]);
			}
		}
		catch (const std::exception& e)
		{
			std::cout << "search pattern: " << e.what() << std::endl;
		}
	}
}

bool argcNumberLessThanTow(int number) {
	if (number < 2)
	{
		std::cout
			<< "\n  please enter name of file to process on command line\n\n";
		return true;
	}
	else
		return false;
}

int main(int argc, char* argv[])
{
	Util::Title("Testing Parser Class", '=');
	Utilities::putline();
	//testPath();
	if (argcNumberLessThanTow(argc))
		return 1;
	AST<element> abstractSyntaxtTree; //AST 
	for (int i = 1; i < argc; ++i)
	{
		DataStore tmpPathDstore; 
		std::vector<std::string> pattern;
		std::string fileSpec = FileSystem::Path::getFullFileSpec(argv[i]);
		std::string msg = "Processing file" + fileSpec;
		Util::title(msg);
		Utilities::putline();

		for (auto fs : tmpPathDstore)
		{
			ConfigParseToConsole configure;
			Parser* pParser = configure.Build(abstractSyntaxtTree);
			try
			{
				if (pParser)
				{
					if (!configure.Attach(fs))
					{
						std::cout << "\n  could not open file " << fileSpec << std::endl;
						continue;
					}
				}
				else
				{
					std::cout << "\n\n  Parser not built\n\n";
					return 1;
				}
				while (pParser->next())
					pParser->parse(abstractSyntaxtTree, fs);
				std::cout << "\n";
			}
			catch (std::exception& ex)
			{
				std::cout << "\n\n    " << ex.what() << "\n\n";
			}
		}	
	}
	std::cout << "\n";
}

#endif
