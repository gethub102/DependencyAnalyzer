//////////////////////////////////////////////////////////////////////////////
//  ParallelDependencyAnalysis.cpp - Paralle Type and Dependency Analysis   //
//  ver 1.0                                                                 //
//  Language:      C++, Visual Studio 2015                                  //
//  Platform:      Macbook win10                                            //
//  Application:   Parser component, OOD Project#3                          //
//  Author:        Webnin Li, Syracuse University,                          //
//                 wli102@syr.edu                                           //
//////////////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides ability to paralle analysis type and denpendence
* for c++ files. The process includes two parse. The first analysis is type
* , second one is dependency analysis. This cpp file mainly provides function
* implementation and test stub.
*
* Public Interface:
* --------------
* getNumberOfArguments(int argc);                                          //analysis commandline
* int handlePathAndPattern(int& i, int argc, char* argv[],
*		DataStore& tmpPathDstore, std::vector<std::string>& pattern,       //get all the files
*		std::string commandD, std::string commandF, std::string commandP); //with specified patterns
*
* Build Process:
* --------------
* Required Files
* TypeAnalysis.h, DependencyAnalysis.h
* FileMgr.h, FileSystem.h, Task.h
* , ThreadPool.h, ActionsAndRules.h,
* ConfigureParser.h, BlockingQueue.h
* Build commands
* - devenv Project3.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.0 : 07 Apr 2016
* - first release by Wenbin
*/

#include <string>
#include <unordered_map>
#include "../Parser/ActionsAndRules.h"
#include "../Parser/ConfigureParser.h"
#include "ParallelDependencyAnalysis.h"
#include "../BlockingQueue/BlockingQueue.h"
#include "../DependencyAnalysis/DependencyAnalysis.h"
#include <queue>
#include <string>

#define Util StringHelper

using namespace Scanner;

/* judge if the argument number not less than 2 */
int ParallelDependencyAnalysis::getNumberOfArguments(int argc) {
	if (argc < 2)
	{
		std::cout
			<< "\n  please enter name of file to process on command line\n\n";
		return 1;
	}
	else
		return 0;
}

/* ---get the path of file--- */
DataStore ParallelDependencyAnalysis::getPath(std::string path, DataStore& ds, std::vector<std::string> pattern) {
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
void ParallelDependencyAnalysis::searchPatternCommand(std::vector<std::string>& pattern, int argc, char** argv) {
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

/* handle multiple path and specified files, and add multiple patterns */
int ParallelDependencyAnalysis::handlePathAndPattern(int& i, int argc, char* argv[],
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
	else
	{
		i++;
		return 1;
	}
}
/* creat the end element for merge map */
element* ParallelDependencyAnalysis::createEndofMergeMap()
{
	element* endElement = new element;
	endElement->fileName = "endofMerge";
	return endElement;
}
/* get the merged table */
void mergeTable(TypeAnalysis& typeAnalysis, int& numofFiles, std::unordered_map<std::string, element*>& wholeMap,
	BlockingQueue<std::unordered_map<std::string, element*>>& typeQueue) {
	while (numofFiles > 1)
	{
		numofFiles--;
		auto tmpTpTable = typeQueue.deQ();
		wholeMap.insert(tmpTpTable.begin(), tmpTpTable.end());
	}
	typeAnalysis.showPairs(wholeMap); //show the merged map
}
/* display the dependency */
void displayDependencyrelatinship(int numfilesfordependency, BlockingQueue<DependencyList*>& dependencyListQeue) {
	while (numfilesfordependency > 0)
	{
		numfilesfordependency--;
		auto relationship = dependencyListQeue.deQ();
		for (auto mainlist : *relationship) {
			std::cout << "\t" << mainlist.first << "-------" << std::endl;
			for (auto secondList : mainlist.second) {
				if (mainlist.first != secondList.first)
					std::cout << "\t\t" << secondList.first << std::endl;
			}
		}

	}
}

#ifdef TEST_PARALLEDEPENDENCYANALYSIS

int main(int argc, char* argv[]) {
	ParallelDependencyAnalysis parallelDependencyAnalysis;
	if (parallelDependencyAnalysis.getNumberOfArguments(argc) == 1) //check garguments number more than 1
		return 1;
	Task tasks;//return type is void
	tasks.begin(10); //START 10 threads, the num can be changed
	BlockingQueue<std::string> filePathQueue; //store all the file pathes
	BlockingQueue<std::string> filePathQueueforDepdncy; //for dependency
	BlockingQueue<std::unordered_map<std::string, element*>> typeQueue; //store all the partial type table
	BlockingQueue<DependencyList*> dependencyListQeue; //container for dependency result
	int numofFiles = 0; //hold file num
	BlockingQueue<int> countofFilesQueue; //container for int numofFiles = 0; to block before merge
	
	WorkItem workItemForFileMgr;             //for file manager
	WorkItem workItemTypeAnalysis;           //for analysis type
	WorkItem workItemForDependencyAnalysis;  //for analysis dependency
	TypeAnalysis typeAnalysis;               //obj for analysis type
	DependencyAnalysis dependencyAnalysis;   //obj for analysis dependency
	workItemTypeAnalysis = [&]() { //type analysis lambda
		AST<element> abstractSyntaxtTree; //AST 
		std::string fs = filePathQueue.deQ();
		ConfigParseToConsole configure;
		Parser* pParser = configure.Build(abstractSyntaxtTree);
		std::unordered_map<std::string, element*> type_table;
		typeAnalysis.inserDefaultTypeTable(type_table, fs, abstractSyntaxtTree); //handle end of merger. It will record a type_table with name"endofMerge" and enQ()
		typeAnalysis.analysisType(fs, abstractSyntaxtTree, pParser, configure);
		typeQueue.enQ(type_table); //stroe type table to this blocking queue
	};
	workItemForFileMgr = [&]() { // get all the files lambda
		std::string commandD = "-d", commandF = "-f", commandP = "-p";
		BlockingQueue<std::string>* fileQueue = new BlockingQueue<std::string>(); //create a queue in heap
		DataStore tmpPathDstore;
		for (int i = 1; i < argc; ++i) {
			std::vector<std::string> pattern;
			if (parallelDependencyAnalysis.handlePathAndPattern(i, argc, argv, tmpPathDstore, pattern, commandD, commandF, commandP) == 1)
				continue;
			for (auto fs : tmpPathDstore) {
				numofFiles++;
				fileQueue->enQ(fs);
				auto filePath = fileQueue->deQ();
				filePathQueue.enQ(filePath);
				filePathQueueforDepdncy.enQ(filePath);
				tasks.doWork(&workItemTypeAnalysis); //async type analysis
			}
			countofFilesQueue.enQ(numofFiles);//enQ the num of files to unlock megere start
		}
	};
	tasks.doWork(&workItemForFileMgr); //START file manager
	//Merge the table on main thread
	int numfilesfordependency = numofFiles = countofFilesQueue.deQ(); //block the num of files
	auto wholeMap = typeQueue.deQ();
	mergeTable(typeAnalysis, numofFiles, wholeMap, typeQueue); //merge table and display it

	//Dependency analysis
	workItemForDependencyAnalysis = [&]() { //Dependency analysis lambda
		AST<element> abstractSyntaxtTree; //AST 
		std::string fs = filePathQueueforDepdncy.deQ();
		ConfigParseToConsole configure;
		Parser* pParser = configure.BuildDependencyAnalysis(abstractSyntaxtTree);
		configure.getRepository()->setRefTypeTableMap(wholeMap); //copy type table map to repository
		std::unordered_map<std::string, element*> type_table;
		dependencyAnalysis.dependencyAnalysis(fs, abstractSyntaxtTree, pParser, configure);
		auto denpendencylist  = configure.getRepository()->getDependencyList();
		dependencyListQeue.enQ(denpendencylist); //store the dependency relationship in dependencyListQeue
	};
	for (int i = 0; i < numfilesfordependency; i++) //START dependency analysis
		tasks.doWork(&workItemForDependencyAnalysis); 
	displayDependencyrelatinship(numfilesfordependency, dependencyListQeue); //display relationship
	tasks.endThreadPool();
	tasks.wait();
	return 0;
}

#endif