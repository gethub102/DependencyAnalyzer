#ifndef FILEMGR_H
#define FILEMGR_H
/////////////////////////////////////////////////////////////////////
//  FileMgr.h - Manage the regular files                           //
//  ver 1.4                                                        //
//  Language:      Visual C++ 2015, SP1                            //
//  Platform:      Macbook, Win10                                  //
//  Author:        Wenbin Li,      Syracuse University             //
//                 wli102@syr.edu                                  //
//              Source from Dr. Jim                                //
/////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================
This module defines DataStore and FileMgr class. They will be used
for searching files, and managing files.

Public Interface:
=================
Search()
Find()
recursionFind(const std::string& path, const std::string& patt)
addPattern()

Build Process:
==============
Required files
FileSystem.h, BlockingQueue.h
Build command:
- devenv Project3.sln /rebuild debug

Maintenance History:
====================
ver 1.1 : 16 Mar 08
- added recursionFind(const std::string& path, const std::string& patt)
ver 1.0 : 
- first release by Jim

*/

#include "FileSystem.h"
#include "../BlockingQueue/BlockingQueue.h"
#include <vector>
/* class for store the file name and path and their pattern for searching  */
class DataStore
{
public:
	using iterator = std::vector<std::string>::iterator;
	void save(const std::string& filespec) { store.push_back(filespec); }
	void save(const std::string& filename, const std::string& path) {}
	iterator begin() { return store.begin(); }
	iterator end() { return store.end(); }
	std::vector<std::string> getStore() { return store; }
private:
	std::vector<std::string> store;
};
/* class for searching and orgnize file */
class FileMgr
{
public:
	using iterator = DataStore::iterator;
	using patterns = std::vector<std::string>;
	/*-- construto --*/
	FileMgr(const std::string& path, DataStore& ds) : path_(path), store_(ds)
	{
		patterns_.push_back("*.*");
	}
	/* add specified pattern for searching */
	void addPattern(const std::string& patt)
	{
		if (patterns_.size() == 1 && patterns_[0] == "*.*")
			patterns_.pop_back();
		patterns_.push_back(patt);
	}
	/* searching the file */
	void search()
	{
		find(path_);
	}
	/* recursively find the file for specified folder and pattern */
	void recursionFind(const std::string& path, const std::string& patt) {
		std::vector<std::string> files = FileSystem::Directory::getFiles(path, patt);
		for (auto f : files)
		{
			f = FileSystem::Path::fileSpec(path, f);
			store_.save(f);
		}
		std::vector<std::string> dirs = FileSystem::Directory::getDirectories(path);
		for (auto d : dirs)
		{
			if (d == "." || d == "..")
				continue;
			auto dirPath = path + "/" + d; //modified
			recursionFind(dirPath, patt);
		}
	}
	
	/* recursively find file with more than one patterns */
	void find(const std::string& path)
	{
		for (auto patt : patterns_)
			recursionFind(path, patt);
	}
private:
	std::string path_;
	DataStore& store_;
	patterns patterns_;
	BlockingQueue<std::string> fileQueue;
};

#endif
