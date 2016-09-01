/////////////////////////////////////////////////////////////////////
//  FileMgr.cpp - find files matching specified patterns           //
//               on a specified path                               //
//  ver 1.0                                                        //
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
* This package provides test module for FileMgr.h, which will
* process for searching specified directory recursively find
* all the file in this directory.
*
* Build Process:
* --------------
* Required Files
* ScopeStack.h, MetricAnalysis.h
* Build commands
* - devenv Project3.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.0 : 10 Mar 2016
* - first release by Jim
*/
#include "FileMgr.h"
#include "../BlockingQueue/BlockingQueue.h"
#include <iostream>


#ifdef TEST_FILEMGR

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

int main(int argc, char* argv[])
{
	BlockingQueue<std::string> fileQueue;
	if (getNumberOfArguments(argc) == 1)
		return 1;
	std::string commandD = "-d", commandF = "-f", commandP = "-p";
	for (int i = 1; i < argc; ++i) {
		DataStore tmpPathDstore;
		std::vector<std::string> pattern;
		if (handlePathAndPattern(i, argc, argv, tmpPathDstore, pattern, commandD, commandF, commandP) == 1)
			continue;
		for (auto fs : tmpPathDstore) {
			fileQueue.enQ(fs);
			std::cout << "file: " << fs << std::endl;
		}
	}
	//DataStore ds;
	//FileMgr fm(".", ds); // path and datastore
	////fm.addPattern("*.h");
	////fm.addPattern("*.partial");
	//fm.search();
	//for (auto fs : ds)
	//{
	//	std::cout << "\n  " << fs;
	//}
	//std::cout << "\n\n";
	return 0;
}
#endif
