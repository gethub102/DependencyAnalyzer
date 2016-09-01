///////////////////////////////////////////////////////////////////////////////
// ScopeStack.cpp - implements template stack holding specified element type //
// ver 2.0                                                                   //
// Language:      Visual C++ 2015                                            //
// Platform:      Macbook, Win 10                                            //
// Application:   Project#2 OOD 2016                                         //
// Author:        Wenbin Li, Syracuse University                             //
//                wli102@syr.edu                                             //
///////////////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides a demonstration for AST node manipulation
* The main process is new an element, push to stack, add the elem
* to ointer top()'s child field. And get the line number of each
* scope token.
*
* Public Interface:
* =================
* TreeWalk(pTop)       // dispaly the structure
* testStack.push(pItem) //push element to stack
* 
* Build Process:
* ==============
* Required files
* - ScopeStack.h
* Build commands 
* - devenv Project3.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 2.0 : 11 Jun 2014
* - first release by Jim
*/
//----< test stub >------------------------------------------------------
#include <string>

#ifdef TEST_SCOPESTACK

#include "ScopeStack.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

struct element
{
	std::string type;
	std::string name;
	size_t startLineCount = 0;
	size_t endLineCount = 0;
	std::string show()
	{
		std::ostringstream temp;
		temp << "(";
		temp << type;
		temp << ", ";
		temp << name;
		temp << ", ";
		temp << startLineCount;
		temp << ", ";
		temp << endLineCount;
		temp << ")";
		return temp.str();
	}
	std::vector<element*> _children;
};

void TreeWalk(element* pItem)
{
	static size_t indentLevel = 0;
	std::cout << "\n  " << std::string(2 * indentLevel, ' ') << pItem->show();
	auto iter = pItem->_children.begin();
	++indentLevel;
	while (iter != pItem->_children.end())
	{
		TreeWalk(*iter);
		++iter;
	}
	--indentLevel;
}

/* print test title */
void testTile() {
	std::cout << "\n  Testing Scope Stack";
	std::cout << "\n =====================\n";

	std::cout << "\n  pushing items onto ScopeStack instance after adding startLineCount";
	std::cout << "\n --------------------------------------------------------------------";
}

void addElementAndChild(element* pItem, ScopeStack<element*>& testStack, std::string type, int line) {
	pItem->type = type;
	pItem->name = "";
	pItem->startLineCount = line;
	(testStack.top()->_children).push_back(pItem);
	testStack.push(pItem);
}

int main()
{
	testTile();
	ScopeStack<element*> testStack;
	element* pItem = new element;

	pItem->type = "function";
	pItem->name = "fun1";
	pItem->startLineCount = 33;
	testStack.push(pItem);

	pItem = new element;
	addElementAndChild(pItem, testStack, "if", 38);
	pItem = new element;
	addElementAndChild(pItem, testStack, "for", 40);

	showStack<element>(testStack, false);
	std::cout << std::endl;

	std::cout << "\n  Popping two items off ScopeStack after adding endLineCount";
	std::cout << "\n ------------------------------------------------------------";
	testStack.top()->endLineCount = 50;
	std::cout << "\n  " << testStack.pop()->show();
	testStack.top()->endLineCount = 55;
	std::cout << "\n  " << testStack.pop()->show();
	std::cout << "\n";
	std::cout << "\n  Pushing another item onto ScopeStack after adding startLineElement";
	std::cout << "\n --------------------------------------------------------------------";
	pItem = new element;
	addElementAndChild(pItem, testStack, "while", 60);
	std::cout << "\n  Stack now contains:";
	std::cout << "\n ---------------------";
	showStack<element>(testStack, false);
	std::cout << "\n";
	std::cout << "\n  Popping last elements off stack after adding endLineCount";
	std::cout << "\n -----------------------------------------------------------";
	testStack.top()->endLineCount = 70;
	element* pTop = testStack.pop();
	std::cout << "\n  " << pTop->show();
	testStack.top()->endLineCount = 75;
	pTop = testStack.pop();
	std::cout << "\n  " << pTop->show();
	std::cout << "\n";
	std::cout << "\n  Walking simulated AST tree:";
	std::cout << "\n -----------------------------";
	TreeWalk(pTop);
	std::cout << "\n\n";
}

#endif
