#ifndef PARALLEDEPENDENCYEXECUTIVE_H
#define PARALLEDEPENDENCYEXECUTIVE_H
/////////////////////////////////////////////////////////////////////
//  ParalleDependencyExecutive.h - Execute Paralle Analysis        //
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
* FileSystem.h, Task.h, ThreadPool.h
* Build commands
* - devenv Project3.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.0 : 07 Apr 2016
* - Add execute class wrap ParalledependencyExecutive
* ver 1.0 : 07 Apr 2016
* - first release by Wenbin
*/
#include "../Utilities/Utilities.h"
#include "../FileMgr/FileSystem.h"
#include "../FileMgr/FileMgr.h"
#include <iostream>
#include "../TypeAnalysis/TypeAnalysis.h"
#include "../DependencyAnalysis/DependencyAnalysis.h"
#include "../ParallelDependencyAnalysis/ParallelDependencyAnalysis.h"

#define Util StringHelper
using namespace Utilities;


class ParalleDependencyExecutive { 
public:
	ParalleDependencyExecutive();
	void printWholeTestTitle();
	int getNumberOfArguments(int argc);
	DataStore getPath(std::string path, DataStore& ds, std::vector<std::string> pattern);
	void searchPatternCommand(std::vector<std::string>& pattern, int argc, char** argv);
	int handlePathAndPattern(int& i, int argc, char* argv[],
		DataStore& tmpPathDstore, std::vector<std::string>& pattern,
		std::string commandD, std::string commandF, std::string commandP);
	void displayDependencyrelatinship(int numfilesfordependency, BlockingQueue<DependencyList*>& dependencyListQeue);
	void mergeTable(TypeAnalysis* typeAnalysis, int& numofFiles, std::unordered_map<std::string, element*>& wholeMap,
		BlockingQueue<std::unordered_map<std::string, element*>>* typeQueue);
	BlockingQueue<std::string>* getfilePathQueue() { return &filePathQueue; }
	void FileManagerAndTypeAnalysis(int threadnum);
	void setArgcv(int argc, char* argv[]);
	int& getnumofFiles() { return numofFiles; }
	void setnumofFiles(int num) { numofFiles = num; }
	BlockingQueue<int>* getcountofFilesQueue() { return &countofFilesQueue; }
	BlockingQueue<std::unordered_map<std::string, element*>>* gettypeQueue() { return &typeQueue; }
	TypeAnalysis* gettypeAnalysis() { return ptypeAnalysis; }
	DependencyAnalysis* getdependencyAnalysis() { return pdependencyAnalysis; }
	void DependencyAnaly();
	void setwholeMap(std::unordered_map<std::string, element*> _wholeMap) { wholeMap = _wholeMap; }
	void setnumfilesfordependency(int num) { numfilesfordependency = num; } 
	BlockingQueue<DependencyList*>* getdependencyListQeue() { return &dependencyListQeue; }
	void endThread(){ tasks.endThreadPool(); tasks.wait();}
	int exe(int argc, char* argv[], int threadnum);
private:
	Task tasks;
	BlockingQueue<std::string> filePathQueue; //store all the file pathes
	BlockingQueue<std::string> filePathQueueforDepdncy; //for dependency
	BlockingQueue<std::unordered_map<std::string, element*>> typeQueue; //store all the partial type table
	BlockingQueue<DependencyList*> dependencyListQeue; //container for dependency result
	int numofFiles = 0; //hold file num
	int numfilesfordependency = 0;
	BlockingQueue<int> countofFilesQueue; //container for int numofFiles = 0; to block before merge
	std::unordered_map<std::string, element*> wholeMap;
	WorkItem workItemForFileMgr;             //for file manager
	WorkItem workItemTypeAnalysis;           //for analysis type
	WorkItem workItemForDependencyAnalysis;  //for analysis dependency
	TypeAnalysis* ptypeAnalysis;               //obj for analysis type
	DependencyAnalysis* pdependencyAnalysis;   //obj for analysis dependency
	int argc;
	char** argv;
	BlockingQueue<std::string> ThreadIDQueue; //store the thread id for demonstrate
};



// execute class, the entrance for whole project
class execute {
public:
	ParalleDependencyExecutive& exe() { return pParalleDependencyExecutive; }
private:
	ParalleDependencyExecutive pParalleDependencyExecutive;
};
#endif 
