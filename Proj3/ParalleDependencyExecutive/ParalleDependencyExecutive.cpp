/////////////////////////////////////////////////////////////////////
//  ParalleDependencyExecutive.cpp - Execute Paralle Analysis      //
//  ver 1.0                                                        //
//  Language:      C++, Visual Studio 2015                         //
//  Platform:      Macbook win10                                   //
//  Application:   Parser component, OOD Project#3                 //
//  Author:        Webnin Li, Syracuse University,                 //
//                 wli102@syr.edu                                  //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides the entrance for the project 3. we can only
* provide the files and num of thread to start the whole analysis.
*
* Public Interface:
* --------------
* int exe(int argc, char* argv[], int threadnum); //start analysis
*
* Build Process:
* --------------
* Required Files
* TypeAnalysis.h, DependencyAnalysis.h
* ParallelDependencyAnalysis.h, FileMgr.h,
* FileSystem.h
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
#include "ParalleDependencyExecutive.h"
#include "../BlockingQueue/BlockingQueue.h"
#include "../DependencyAnalysis/DependencyAnalysis.h"
#include <queue>
#include <string>

#define Util StringHelper

using namespace Scanner;

inline ParalleDependencyExecutive::ParalleDependencyExecutive()
{
	printWholeTestTitle();
}

/* print the whole test title */
void ParalleDependencyExecutive::printWholeTestTitle() {
	Util::Title("Testing ParallelDependencyExecutive Package", '=');
	Utilities::putline();
}

/* judge if the argument number not less than 2 */
int ParalleDependencyExecutive::getNumberOfArguments(int argc) {
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
DataStore ParalleDependencyExecutive::getPath(std::string path, DataStore& ds, std::vector<std::string> pattern) {
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
void ParalleDependencyExecutive::searchPatternCommand(std::vector<std::string>& pattern, int argc, char** argv) {
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
int ParalleDependencyExecutive::handlePathAndPattern(int& i, int argc, char* argv[],
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

/* merge the type table and display it */
void ParalleDependencyExecutive::mergeTable(TypeAnalysis* typeAnalysis, int& numofFiles, std::unordered_map<std::string, element*>& wholeMap,
	BlockingQueue<std::unordered_map<std::string, element*>>* typeQueue) {
	std::cout << "\n  Merge type table working on thread " << std::this_thread::get_id() << std::endl;
	while (numofFiles > 1)
	{
		numofFiles--;
		auto tmpTpTable = gettypeQueue()->deQ();
		wholeMap.insert(tmpTpTable.begin(), tmpTpTable.end());
	}
	typeAnalysis->showPairs(wholeMap); //show the merged map
	setwholeMap(wholeMap);//set whole map in class
}

//Merge and dislpaly the result of dependency analysis
void ParalleDependencyExecutive::displayDependencyrelatinship(int numfilesfordependency, BlockingQueue<DependencyList*>& dependencyListQeue) {
	DependencyList* tmp = new DependencyList;
	std::ostringstream statisticsForThread;
	statisticsForThread << "\n   Merge the dependency result table working on thread " << std::this_thread::get_id(); //collect thread ID
	std::string idstr = statisticsForThread.str();    //collect thread ID
	std::cout << idstr;
	while (numfilesfordependency > 0)
	{
		auto relationship = dependencyListQeue.deQ();
		numfilesfordependency--;
		for (auto mainlist : *relationship) {
			//std::cout << "\t" << mainlist.first << "-------" << std::endl;
			for (auto secondList : mainlist.second) {
				//if (mainlist.first != secondList.first)
					//std::cout << "\t\t" << secondList.first << std::endl;
				(*tmp)[mainlist.first].insert({ secondList.first, "default" });
			}
		}
	}
	//display result
	auto relationship = tmp;
	for (auto file : FileNames_)
	{
		std::cout << "\t" << file << "-------" << std::endl;
		auto second = (*relationship)[file];
		for (auto secondList : second) {
			if (file != secondList.first)
				std::cout << "\t\t" << secondList.first << std::endl;
		}
	}
}
/* process for file manager and type analysis */
void ParalleDependencyExecutive::FileManagerAndTypeAnalysis(int threadnum) {
	tasks.begin(threadnum);
	std::cout << "\n  Main thread ID = " << std::this_thread::get_id();
	//std::string statisticForThreadinfor;
	workItemTypeAnalysis = [this]() { //type analysis lambda
		AST<element> abstractSyntaxtTree; //AST 
		std::string fs = filePathQueue.deQ();
		auto myid = std::this_thread::get_id();
		std::ostringstream statisticsForThread;
		statisticsForThread << "\n  File--" << fs << " working on thread " << std::this_thread::get_id(); //collect thread ID
		std::string idstr = statisticsForThread.str();    //collect thread ID
		ThreadIDQueue.enQ(idstr);                         //collect thread ID
		ConfigParseToConsole configure;
		Parser* pParser = configure.Build(abstractSyntaxtTree);
		std::unordered_map<std::string, element*> type_table;
		gettypeAnalysis()->inserDefaultTypeTable(type_table, fs, abstractSyntaxtTree); //handle end of merger. It will record a type_table with name"endofMerge" and enQ()
		gettypeAnalysis()->analysisType(fs, abstractSyntaxtTree, pParser, configure);
		typeQueue.enQ(type_table); //stroe type table to this blocking queue
	};

	workItemForFileMgr = [this]() { // get all the files lambda
		std::cout << "\n  File manager working on thread " << std::this_thread::get_id();
		std::string commandD = "-d", commandF = "-f", commandP = "-p";
		BlockingQueue<std::string>* fileQueue = new BlockingQueue<std::string>(); //create a queue in heap
		DataStore tmpPathDstore;
		for (int i = 1; i < argc; ++i) {
			std::vector<std::string> pattern;
			if (handlePathAndPattern(i, argc, argv, tmpPathDstore, pattern, commandD, commandF, commandP) == 1)
				continue;
			for (auto fs : tmpPathDstore) {
				numofFiles++;
				fileQueue->enQ(fs);
				auto name = FileSystem::Path::getName(fs);
				FileNames_.push_back(name);          //store the file names
				auto filePath = fileQueue->deQ();
				filePathQueue.enQ(filePath);
				filePathQueueforDepdncy.enQ(filePath);
				tasks.doWork(&workItemTypeAnalysis); //async type analysis
			}
			countofFilesQueue.enQ(numofFiles);//enQ the num of files to unlock megere start
		}
	};
	tasks.doWork(&workItemForFileMgr);
}

/* set the command line arguments */
void ParalleDependencyExecutive::setArgcv(int _argc, char* _argv[])
{
	argc = _argc;
	argv = _argv;
}

/* dependency analysis following merge type table */
void ParalleDependencyExecutive::DependencyAnaly() {
	//Dependency analysis
	workItemForDependencyAnalysis = [this]() { //Dependency analysis lambda
		AST<element> abstractSyntaxtTree; //AST 
		std::string fs = filePathQueueforDepdncy.deQ();
		ConfigParseToConsole configure;
		Parser* pParser = configure.BuildDependencyAnalysis(abstractSyntaxtTree);
		configure.getRepository()->setRefTypeTableMap(wholeMap); //copy type table map to repository
		std::unordered_map<std::string, element*> type_table;
		getdependencyAnalysis()->dependencyAnalysis(fs, abstractSyntaxtTree, pParser, configure);
		auto denpendencylist = configure.getRepository()->getDependencyList();
		dependencyListQeue.enQ(denpendencylist); //store the dependency relationship in dependencyListQeue
	};
	for (int i = 0; i < numfilesfordependency; i++) //START dependency analysis
		tasks.doWork(&workItemForDependencyAnalysis);
}

/* get the thread number and command line to process the whole program */
int ParalleDependencyExecutive::exe(int argc, char* argv[], int threadnum) {
	if (getNumberOfArguments(argc) == 1) //check garguments number more than 1
		return 1;
	else
	{
		setArgcv(argc, argv); //get arguments from command line
		FileManagerAndTypeAnalysis(10);//START the FileManager and TypeAnalysis

		int numfilesfordependency = getcountofFilesQueue()->deQ(); //block and get file num, Until type analysis completed
		for (int i = numfilesfordependency; i > 0; i--)     //print the thread ID
			std::cout << " " << ThreadIDQueue.deQ() << "\n";//print the thread ID
		std::cout << "\n\n";
		setnumofFiles(numfilesfordependency);  //get file num
		setnumfilesfordependency(numfilesfordependency); //set file num for dependency
		auto wholeMap = gettypeQueue()->deQ(); //first get a partial type table
		mergeTable(gettypeAnalysis(), getnumofFiles(), wholeMap, gettypeQueue()); //merge table and display it

		DependencyAnaly();//START the Dependency analysis

		auto ListQeue = getdependencyListQeue(); //get dependency ListQeue
		displayDependencyrelatinship(numfilesfordependency, *ListQeue); //display dependency relationship
		endThread();//End the thread pool
		return 0;
	}
}



#ifdef TEST_PARALLEDEPENDENCYEXECUTIVE

int main(int argc, char* argv[]) {
	execute exe;                    // get command line arguments
	exe.exe().exe(argc, argv, 15);  // and thread number
}

#endif