#ifndef PARALLEDEPENDENCYANALYSIS_H
#define PARALLEDEPENDENCYANALYSIS_H
//////////////////////////////////////////////////////////////////////////////
//  ParallelDependencyAnalysis.h - Paralle Type and Dependency Analysis     //
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
* , second one is dependency analysis.
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
* , ThreadPool.h
* Build commands
* - devenv Project3.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.0 : 07 Apr 2016
* - first release by Wenbin
*/
#include "../Utilities/Utilities.h"
#include "../FileMgr/FileSystem.h"
#include "../FileMgr/FileMgr.h"
#include <iostream>

#define Util StringHelper
using namespace Utilities;

class ParallelDependencyAnalysis {
public:
	ParallelDependencyAnalysis();
	void printWholeTestTitle();
	int getNumberOfArguments(int argc);
	DataStore getPath(std::string path, DataStore& ds, std::vector<std::string> pattern);
	void searchPatternCommand(std::vector<std::string>& pattern, int argc, char** argv);
	int handlePathAndPattern(int& i, int argc, char* argv[],
		DataStore& tmpPathDstore, std::vector<std::string>& pattern,
		std::string commandD, std::string commandF, std::string commandP);
	element* createEndofMergeMap();
};

inline ParallelDependencyAnalysis::ParallelDependencyAnalysis()
{
	printWholeTestTitle();
}

/* print the whole test title */
void ParallelDependencyAnalysis::printWholeTestTitle() {
	Util::Title("Testing ParallelDependencyAnalysis Class", '=');
	Utilities::putline();
}

#endif 
