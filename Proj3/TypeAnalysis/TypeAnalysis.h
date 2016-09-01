#ifndef TYPEANALYSIS_H
#define TYPEANALYSIS_H
/////////////////////////////////////////////////////////////////////
//  TypeAnalysis.h - Analysis type and global func in c++ files    //
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
* This package provides TypeAnalysis class and
* analysisType function for type analysis.
*
* Public Interface:
* --------------
* analysisType(std::string fs, AST<element>& abstractSyntaxtTree, 
*		 Parser* pParser, ConfigParseToConsole& configure);
*
* Build Process:
* --------------
* Required Files
* ActionsAndRules.h, ConfigureParser.h,
* Task.h, Utilities.h, Helpers.h
* Build commands
* - devenv Project3.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.0 : 07 Apr 2016
* - first release by Wenbin
*/

#include "../Parser/ActionsAndRules.h"
#include "../Parser/ConfigureParser.h"
#include "../Task/Task.h"
#include "../Utilities/Utilities.h"
#include "../Utilities/Helpers.h"
#include <iomanip>


#define Util StringHelper
using namespace Utilities;
using TypeQueue = std::unordered_map<std::string, element*>;
using TypeAnlaysisResult = std::unordered_map<std::string, element*>*;

class TypeAnalysis {
	std::mutex mtx_;
public:
	TypeAnlaysisResult analysisType(std::string fs, AST<element>& abstractSyntaxtTree, 
		 Parser* pParser, ConfigParseToConsole& configure);
	void showPairs(std::unordered_map<std::string, element*>& totalMap);
	void inserDefaultTypeTable(std::unordered_map<std::string, element*>& type_table,
		std::string& fs, AST<element>& abstractSyntaxtTree);
};

/* main process for analysis file types  */
inline TypeAnlaysisResult TypeAnalysis::analysisType(std::string fs, AST<element>& abstractSyntaxtTree, // template argument
	Parser* pParser, ConfigParseToConsole& configure) //template function
{
	std::string fileName = FileSystem::Path::getName(fs);

	element* functionEle = new element;
	abstractSyntaxtTree.pushToFilefunction(functionEle); //add a node for hold the function of the single file
	functionEle->filepathandname = fs;
	functionEle->filename = fileName;

	try {
		//fs = filePathQueue.deQ();
		if (pParser) {
			if (!configure.Attach(fs)) {
				std::cout << "\n  could not open file " << fs << std::endl; 
				return nullptr;
			}
		}
		else {
			return nullptr;
		}
		while (pParser->next())
			pParser->parse(abstractSyntaxtTree, fileName);
		if (abstractSyntaxtTree.getTree().size() > 0) // the reason get abort
		{
			abstractSyntaxtTree.getTree().back()->filepathandname = fs; //get ful path and name
			abstractSyntaxtTree.getTree().back()->filename = FileSystem::Path::getName(fs);
		}
		else
		{
			abstractSyntaxtTree.buildTree();
			abstractSyntaxtTree.getTree().back()->filepathandname = fs; //get ful path and name
			abstractSyntaxtTree.getTree().back()->filename = FileSystem::Path::getName(fs);
		}
		std::cout << "\n";
	}
	catch (std::exception& ex) {
		std::cout << "\n\n    " << ex.what() << "\n\n";
		return nullptr;
	}
	auto result = abstractSyntaxtTree.getTypeTable(fileName);
	return result;
}

/* show the type table,  the map format */
void TypeAnalysis::showPairs(std::unordered_map<std::string, element*>& totalMap) { // template argument
	std::cout << std::setw(30) << "name" << std::setw(30) << "type" << std::setw(30) << "file" << std::setw(30) << std::endl;
	std::cout << "===================================================================================================\n";
	for (auto it : totalMap)
	{
		
		auto first = it.first;
		auto second = it.second;
		std::string elefileName = second->fileName;
		std::string eleType = second->type;
		std::string eleNamespace = second->nameSpace;
		
		if (first != "namespace")
		{
			std::cout << std::setw(30) << first << std::setw(30); //name
			std::cout << std::setw(30) << eleType << std::setw(30); //type
			std::cout << std::setw(30) << elefileName << std::setw(30);//filename
			//std::cout << std::setw(20) << eleNamespace << std::setw(20);//namespace
			std::cout << std::setw(30) << std::endl;//line
		}
	}
}
/* insert a default type table during type analysis */
inline void TypeAnalysis::inserDefaultTypeTable(std::unordered_map<std::string, element*>& type_table,
	std::string& fs, AST<element>& abstractSyntaxtTree) { // template argument
	element* default_ele = new element;
	if (fs == "endofMerge")
		default_ele->type = "endofMerge";
	else
		default_ele->type = "namespace";
	auto fileName = default_ele->fileName = default_ele->filename = FileSystem::Path::getName(fs);
	type_table.insert({ "namespace", default_ele });
	auto file_table = abstractSyntaxtTree.getFileTable();
	file_table->insert({ fileName, &type_table });
}


#endif