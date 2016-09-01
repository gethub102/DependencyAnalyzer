#ifndef DEPENDENCYANALYSIS_H
#define DEPENDENCYANALYSIS_H
/////////////////////////////////////////////////////////////////////
//  DependencyAnalysis.h - Analysis dependency of c++ files        //
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
* This package provides DependencyAnalysis class and 
* dependencyAnalysis function for dependence analysis.
*
* Public Interface:
* --------------
* dependencyAnalysis(std::string fs, AST<element>& abstractSyntaxtTree,
*	Parser* pParser, ConfigParseToConsole& configure)
*
* Build Process:
* --------------
* Required Files
* TypeAnalysis.h
* Build commands
* - devenv Project3.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.0 : 07 Apr 2016
* - first release by Wenbin
*/
#include "../TypeAnalysis/TypeAnalysis.h"
class DependencyAnalysis {
public:
	void dependencyAnalysis(std::string fs, AST<element>& abstractSyntaxtTree,
		Parser* pParser, ConfigParseToConsole& configure);
};

/* main procss for dependency analysis */
void DependencyAnalysis::dependencyAnalysis(std::string fs, AST<element>& abstractSyntaxtTree,//there is a template argument
	Parser* pParser, ConfigParseToConsole& configure) {
	std::string fileName = FileSystem::Path::getName(fs);
	try {
		//fs = filePathQueue.deQ();
		if (pParser) {
			if (!configure.Attach(fs)) {
				std::cout << "\n  could not open file " << fs << std::endl;
				return;
			}
		}
		else {
			return;
		}
		while (pParser->next())
			pParser->parse(abstractSyntaxtTree, fileName);
		std::cout << "\n";
	}
	catch (std::exception& ex) {
		std::cout << "\n\n    " << ex.what() << "\n\n";
	}
}
#endif
