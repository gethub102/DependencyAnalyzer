#pragma once
#include "../1/1.h"
#include "../5/5.h"
#include <vector>
using namespace Scaner;

namespace Scaner {  //namespace
	class Parser
	{
	public:
		Parser(ITokCollection* pTokCollection);
		~Parser();
		void addRule(IRule* pRule);
		bool parse(AST<element>& abstractSyntaxtTree);
		bool next();
		void functionDeclaration();
		inline Parser::~Parser() {}
		inline void Parser::functionDeclaration()
		{
		}
		enum Color { red, green, blue };
		Color r = red;
		// simple typedef
		typedef unsigned long ulong;
	private:
		ITokCollection* pTokColl;
		std::vector<IRule*> rules;
	};
}