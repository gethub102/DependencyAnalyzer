/////////////////////////////////////////////////////////////////////
//  TypeAnalysis.cpp - Analyzes Type, Global func inC++ language   //
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
* This package provides the analysis tools for type analysis
* for project 3. It could build a type table that contains 
* class, struct, enum, alias, typedef, and global function.
* This type table is used for dependence analysis. This cpp
* file mainly test type analysis.
*
* Build Process:
* --------------
* Required Files: 
* - TypeAnalysis.cpp
* Build commands
* - devenv Project3.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.0 : 07 Apr 2016
* - first release by Li
*/
#ifdef TEST_TYPEANALYSIS

#include "TypeAnalysis.h"




int main() {
	Util::Title("Testing TypeAnalysis Class", '=');
	Utilities::putline();
	TypeAnalysis typeAnalysis;

	
	Task task;//return type is TypeAnlaysisResult 
	task.begin(2); 	//-----create threadpool------
	WorkItem workItemForTypeAnalysis;
	// -----create file path to test ------
	DataStore tmpPathDstore;
	std::string file1 = "../x64/Test1/1.h";
	tmpPathDstore.save(file1);
	std::string file2 = "../x64/Test1/2.h";
	tmpPathDstore.save(file2);

	BlockingQueue<std::string> filePathQueue;
	filePathQueue.enQ(file1);
	filePathQueue.enQ(file2);
	BlockingQueue<TypeAnlaysisResult> typeAnaQueue;
	BlockingQueue<TypeQueue> typeQueue;
	std::unordered_map<std::string, element*> totalMap;

	workItemForTypeAnalysis = [&]() { //work item of lambda
		AST<element> abstractSyntaxtTree; //AST 
		std::string fs = filePathQueue.deQ();
		ConfigParseToConsole configure;
		Parser* pParser = configure.Build(abstractSyntaxtTree);
		std::unordered_map<std::string, element*> type_table;
		typeAnalysis.inserDefaultTypeTable(type_table, fs, abstractSyntaxtTree);
		typeAnalysis.analysisType(fs, abstractSyntaxtTree, pParser, configure);
		typeQueue.enQ(type_table); //stroe type table to this blocking queue
	};
	for (auto fst : tmpPathDstore) 
		task.doWork(&workItemForTypeAnalysis);
	auto wholeMap = typeQueue.deQ();
	auto tempMap = typeQueue.deQ();
	wholeMap.insert(tempMap.begin(), tempMap.end());
	typeAnalysis.showPairs(wholeMap);

	task.endThreadPool();
	task.wait();

	return 0;
}




#endif