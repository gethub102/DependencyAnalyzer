/////////////////////////////////////////////////////////////////////
//  AbstractSyntaxTree.cpp - Store Trees for every single file     //
//  ver 1.0                                                        //
//                                                                 //
//  Lanaguage:     Visual C++ 2015                                 //
//  Platform:      Macbook, Win10                                  //
//  Application:   Prototype for CSE687 Pr3                        //
//  Author:        Wenbin LI, Syracuse University                  //
//                 wli102@syr.edu                                  //
/////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================
This is the test file for AbstractSyntaxTree.h
AbstractSyntaxTree.h is build for store trees 
of every single file to be analysis. Their trees
are sotre here for the further analysis or modify,
even if the reposity scope goes out.

Public Interface:
=================
void pushToTree(element* elem);         // store the tree
void pushToNoticedNode(element* elem);  // notice not proper cc node
void pushToFilefunction(element* elem); // push every function node

Build Process:
==============
Required files
- MetricAnalysis.h, AbstractSyntaxTree.h
Build commands
- devenv Project3.sln /rebuild debug

Maintenance History:
====================
ver 1.0 : 10 Mar 06
- first release by LI

*/
#include "../AbstractSyntaxTree/AbstractSyntaxTree.h"
#include "../MetricAnalysis/MetricAnalysis.h"

#ifdef TEST_ABSTRACTSYNTAXTREE

int main() {
	AST<element> tree;
	element* elem = new element;
	elem->name = "treename";
	elem->type = "type";
	tree.pushToTree(elem);

	element* elemNode = new element;
	elemNode->name = "Node";
	elemNode->type = "NoticedNode";
	tree.pushToNoticedNode(elemNode);

	std::cout << "The first elem name is " << tree.getTree().front()->name << std::endl;
	std::cout << "Tree noticed node's name is " << tree.getNoticedNode().front()->name << std::endl;
	std::cout << "\n\n";
}
#endif
