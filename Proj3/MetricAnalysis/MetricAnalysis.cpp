/////////////////////////////////////////////////////////////////////
//  MetricAnalysis.cpp - Display and Analysis AST                  //
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
* This package provides a test stub for MetricAnalysis class
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
#include "MetricAnalysis.h"
#include "../ScopeStack/ScopeStack.h"


#ifdef TEST_METRICANALYSIS


void addElementAndChild(element* pItem, ScopeStack<element*>& testStack, std::string type, int line) {
	pItem->type = type;
	pItem->name = "";
	pItem->startLineCount = line;
	(testStack.top()->_children).push_back(pItem);
	testStack.push(pItem);
}

void testTile() {
	std::cout << "\n  Testing MetricAnalysis";
	std::cout << "\n =====================\n";
}

int main() {

	ScopeStack<element*> testStack;
	element* elem = new element;
	element* pItem = new element;

	pItem->type = "function";
	pItem->name = "fun1";
	pItem->startLineCount = 33;
	testStack.push(pItem);

	pItem = new element;
	addElementAndChild(pItem, testStack, "if", 38);
	pItem = new element;
	addElementAndChild(pItem, testStack, "for", 40);

	MertricAnalysis analysis;
	int  complexity = 1;
	analysis.analysisComplexity(testStack.top(), complexity);
	std::cout << "complexity is  " << complexity << std::endl;

	testStack.pop();
	testStack.pop();
	analysis.TreeWalk(testStack.top());
}
#endif