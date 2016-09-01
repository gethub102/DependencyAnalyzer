#ifndef METRICANALYSIS_H
#define METRICANALYSIS_H
/////////////////////////////////////////////////////////////////////
//  MetricAnalysis.h - Display and Analysis AST                    //
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
* This package provides element struct and 
* MetricAnalysis class. element struct defines 
* the node's field and basic function su as addchild(),
* show() element. MetricAnalysis class defines pulic
* interface for analysis AST.
*
* Public Interface:
* --------------
* analysisComplexity(element* elem, int& countComplexity) 
* displayStructureAndSummaryFuction(AST<element> abstractSyntaxtTree)
* analysisComplexity(element* elem, int& countComplexity) 
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
* - first release by Wenbin
*/

#include <queue>
#include <string>
#include <sstream>
#include <iostream>
#include "../AbstractSyntaxTree/AbstractSyntaxTree.h"
#include "../FileMgr/FileSystem.h"
#include "../FileMgr/FileMgr.h"
/* element for store the node of every scope */
struct element
{
	std::string type;
	std::string name;
	size_t lineCount = 0;
	size_t startLineCount = 0; // add the startline
	size_t endLineCount = 0; //add the endline
	int complexity = 0; // the complexity of scope
	std::vector<element*> _children; // add the children
	std::string filename; //filename
	std::string filepathandname; //file full path 
	void addChild(element* elem) { _children.push_back(elem); } //add child

	std::string typeName;
	std::string fileName;
	std::string nameSpace;
	std::vector<std::string> aliases;
	std::string show()
	{
		std::ostringstream temp;
		temp << "(";
		temp << type;
		temp << ", ";
		temp << name;
		if (type != "Global-NameSpace")
		{
			temp << ", lin = ";
			temp << lineCount;
		}
		if (type == "function" || type == "class" || type == "namespace")
		{
			temp << ", cc = ";
			temp << complexity;
		}
		temp << ")";
		return temp.str();
	}
};
/* MertricAnalysis for analysising single node's complexity */
class MertricAnalysis {
public:
	/* analysis complexity of every node, fix the situation of cc = 1 */
	int analysisComplexity(element* elem, int& countComplexity) {
		auto iter = elem->_children.begin();
		countComplexity++;
		while (iter != elem->_children.end())
		{
			analysisComplexity(*iter, countComplexity);
			++iter;
		}
		return countComplexity;
	}

    /*--- display the AST ---*/
	void TreeWalk(element* pItem)
	{
		static size_t indentLevel = 0;
		std::cout << "\n  " << std::string(2 * indentLevel, '\t') << pItem->show();
		auto iter = pItem->_children.begin();
		++indentLevel;
		while (iter != pItem->_children.end())
		{
			TreeWalk(*iter);
			++iter;
		}
		--indentLevel;
	}
	//void displayStructureAndSummaryFuction(AST<element> abstractSyntaxtTree);
	/* diaplay lines and complexity of functions from specified file */
	void MertricAnalysis::displayStructureAndSummaryFuction(AST<element> abstractSyntaxtTree) {
		for (size_t treeindex = 0; treeindex < (abstractSyntaxtTree.getTree()).size(); treeindex++)
		{
			std::cout << "\n\n File full path name = " << abstractSyntaxtTree.getTree()[treeindex]->filepathandname << std::endl;
			//display the tree
			TreeWalk(abstractSyntaxtTree.getTree()[treeindex]);
			//display the filename for summary
			std::cout << " \n      The File: " << abstractSyntaxtTree.getFilefunction().at(treeindex)->filename;
			std::cout << " \n  =============================" << "\n  cyclo lines function name";
			//display the functions summary under the file
			for (size_t indexofFunction = 0; indexofFunction < abstractSyntaxtTree.getFilefunction().at(treeindex)->_children.size(); indexofFunction++)
			{
				auto temp_functionNode = abstractSyntaxtTree.getFilefunction().at(treeindex)->_children.at(indexofFunction);
				std::cout << " \n    " << temp_functionNode->complexity << "    " << temp_functionNode->lineCount << "    " << temp_functionNode->name;
				//abstractSyntaxtTree.getFilefunction().pop_back();
			}
			std::cout << "\n\n";
		}
	}
};


#endif
