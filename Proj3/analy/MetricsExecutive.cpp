/////////////////////////////////////////////////////////////////////
//  MetricsExecutive.cpp - Analyzes C++ language                   //
//  ver 1.0                                                        //
//  Language:      C++, Visual Studio 2015                         //
//  Platform:      Macbook win10                                   //
//  Application:   Parser component, OOD Project#2                 //
//  Author:        Webnin Li, Syracuse University,                 //
//                 wli102@syr.edu                                  //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package is the entrance for this whole solutioin.
* It can analysis single or multiple files with specified
* path, or file name. The class, namespace, function, if, 
* while, try, catch, else and operator==, operator=, operator[],
* and so on will be analysised. The analysis information includes
* complextiy of scope and funciton lines, and their structure.
*
* Build Process:
* --------------
* Required Files: Utilities.h, Utilities.cpp
* ActionsAndRules.h, ConfigureParser.h, 
* FileSystem.h, FileMgr.h
* Build commands
* - devenv Project2HelperCode.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 3.5 : 10 Mar 2016
* - fixed the main() lines less than 50
* ver 1.4 : 10 Mar 2016
* - demonstrate requirements(what I do)
* ver 1.3 : 10 Mar 2016
* - adpat to add multiple patterns for searching
* ver 1.2 : 10 Mar 2016
* - modified the Parser constructor parser() to parser(AbstractSyntaxTree)
* ver 1.1 : 10 Mar 2016
* - fixed fileMgr bug.
* ver 1.0 : 10 Mar 2014
* - first release by Li
*/

#include <iostream>
#include <string>
#include "../Utilities/Utilities.h"
#include "../Parser/ActionsAndRules.h"
#include "../Parser/ConfigureParser.h"
#include "../FileMgr/FileSystem.h"
#include "../FileMgr/FileMgr.h"
#include <queue>
#include <string>
#define Util StringHelper

using namespace Scanner;
using namespace Utilities;

/* ---get the path of file--- */
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
void searchPatternCommand(std::vector<std::string>& pattern, int argc, char** argv) {
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

/* print the whole test title */
void printWholeTestTitle() {
	Util::Title("Testing Parser Class", '=');
	Utilities::putline();
}
/* judge if the argument number not less than 2 */
int getNumberOfArguments(int argc) {
	if (argc < 2)
	{
		std::cout
			<< "\n  please enter name of file to process on command line\n\n";
		return 1;
	}
	else
		return 0;
}

/* handle multiple path and specified files, and add multiple patterns */
int handlePathAndPattern(int& i, int argc, char* argv[], 
	DataStore& tmpPathDstore, std::vector<std::string>& pattern, 
	std::string commandD, std::string commandF, std::string commandP) {

	if (argv[i] == commandD)
	{
		i++;
		searchPatternCommand(pattern, argc, argv);
		getPath(argv[i], tmpPathDstore, pattern);
		return 0;
	}

	else if (argv[i] == commandF)
	{
		i++;
		tmpPathDstore.save(argv[i]);
		return 0;
	}

	else if (argv[i] == commandP)
	{
		i++;
		return 1;
	}
	else {
		i++;
		return 1;
	}
}

#ifdef TEST_METRICSEXECUTIVE

int main(int argc, char* argv[]) {
	printWholeTestTitle();
	if (getNumberOfArguments(argc) == 1)
		return 1;
	AST<element> abstractSyntaxtTree; //AST 
	std::string commandD = "-d", commandF = "-f", commandP = "-p";
	for (int i = 1; i < argc; ++i) {
		DataStore tmpPathDstore;
		std::vector<std::string> pattern;
		if (handlePathAndPattern(i, argc, argv, tmpPathDstore, pattern, commandD, commandF, commandP) == 1)
			continue;
		std::string fileSpec = FileSystem::Path::getFullFileSpec(argv[i]);
		std::string msg = "Processing file " + fileSpec;
		Util::title(msg);
		Utilities::putline();
		for (auto fs : tmpPathDstore) {
			ConfigParseToConsole configure;
			Parser* pParser = configure.BuildMetricAnalysis(abstractSyntaxtTree);
			element* functionEle = new element;
			abstractSyntaxtTree.pushToFilefunction(functionEle); //add a node for hold the function of the single file
			functionEle->filepathandname = fs;
			functionEle->filename = FileSystem::Path::getName(fs);
			try {
				if (pParser) {
					if (!configure.Attach(fs)) {
						std::cout << "\n  could not open file " << fileSpec << std::endl;
						continue;
					}
				}
				else {
					std::cout << "\n\n  Parser not built\n\n";
					return 1;
				}
				while (pParser->next())
					pParser->parse(abstractSyntaxtTree, fs);
				abstractSyntaxtTree.getTree().back()->filepathandname = fs; //get ful path and name
				abstractSyntaxtTree.getTree().back()->filename = FileSystem::Path::getName(fs);
				std::cout << "\n";
			}
			catch (std::exception& ex) {
				std::cout << "\n\n    " << ex.what() << "\n\n";
			}
		}
	}

	MertricAnalysis displayAnalysisTree;	
	displayAnalysisTree.displayStructureAndSummaryFuction(abstractSyntaxtTree);
	std::cout << "\n";
	return 0;
}



#endif