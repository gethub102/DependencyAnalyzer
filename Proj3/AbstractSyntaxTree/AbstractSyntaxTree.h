/////////////////////////////////////////////////////////////////////
//  AbstractSyntaxTree.h - Store Trees for every single file     //
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
AbstractSyntaxTree.h is a template class build for 
store trees of every single file to be analysis. 
Their trees are sotre here for the further analysis 
or modify, even if the reposity scope goes out. 
Moreover the if the function's cc is morethan 10, 
it will be stored in this tree class.

Public Interface:
=================
void pushToTree(element* elem);         // store the tree
void pushToNoticedNode(element* elem);  // notice not proper cc node
void pushToFilefunction(element* elem); // push every function node

Build Process:
==============
Required files
- AbstractSyntaxTree.h
Build commands 
- devenv Project3.sln /rebuild debug

Maintenance History:
====================
ver 1.0 : 29 Mar 06
- modify data structure for storing type analysis unordered_map
ver 1.0 : 10 Mar 06
- first release by LI

*/
#ifndef ABSTRACTSYNTAXTREE_H
#define ABSTRACTSYNTAXTREE_H

#include <vector>
#include <unordered_map>
#include "../BlockingQueue/BlockingQueue.h"
/* class for store every single tree of every file */


template <typename element>
class AST {
public:
	AST();
	AST(const AST<element>& ast);
	std::vector<element*> getTree();
	std::vector<element*> buildTree();
	std::vector<element*> getNoticedNode();
	std::vector<element*> getFilefunction();
	std::unordered_map<std::string, std::unordered_map<std::string, element*>*>* getFileTable();
	std::unordered_map<std::string, element*>* getTypeTable(std::string fileName);
	void pushToTree(element* elem);
	void pushToNoticedNode(element* elem);
	void pushToFilefunction(element* elem);
private:
	std::vector<element*> aSTree;
	std::vector<element*> noticedNode;
	std::vector<element*> filefunction; //element is file name and its children are functions
	//BlockingQueue<element*>* _fileTpyeTable;
	std::unordered_map<std::string, std::unordered_map<std::string, element*>*> fileTable;
};
/* void constructor */
template<typename element>
inline AST<element>::AST(){}

/* AST copy construtor */
template<typename element>
inline AST<element>::AST(const AST<element>& ast) :
	aSTree(ast.aSTree), noticedNode(ast.noticedNode), filefunction(ast.filefunction)
{
	//_fileTpyeTable = ast._fileTpyeTable;
	fileTable = ast.fileTable;
}

/* get the tree data */
template<typename element>
std::vector<element*> AST<element>::getTree() {
	return aSTree;
}

/* get the tree data */
template<typename element>
std::vector<element*> AST<element>::buildTree() {
	element* ele = new element;
	aSTree.push_back(ele);
	return aSTree;
}

/* get the noticed tree node */
template<typename element>
inline std::vector<element*> AST<element>::getNoticedNode()
{
	return noticedNode;
}
/* get the collection of specify file's functions */
template<typename element>
inline std::vector<element*> AST<element>::getFilefunction()
{
	return filefunction;
}

template<typename element>
inline std::unordered_map<std::string, std::unordered_map<std::string, element*>*>* AST<element>::getFileTable()
{
	return &fileTable;
}
/* return the type table for storing orgnized element */
template<typename element>
inline std::unordered_map<std::string, element*>* AST<element>::getTypeTable(std::string fileName)
{
	return fileTable[fileName];
}

/* push element to tree data node */
template<typename element>
inline void AST<element>::pushToTree(element* elem) {
	aSTree.push_back(elem);
}

/* push noticed node to tree data -- noticedNode */
template<typename element>
inline void AST<element>::pushToNoticedNode(element* elem)
{
	noticedNode.push_back(elem);
}
/* push functions to the collections */
template<typename element>
inline void AST<element>::pushToFilefunction(element * elem)
{
	filefunction.push_back(elem);
}


#endif
