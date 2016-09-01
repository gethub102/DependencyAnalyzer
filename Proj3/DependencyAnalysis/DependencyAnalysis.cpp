#include "DependencyAnalysis.h"


#ifdef TEST_DEPENDENCYANALYSIS

int main(int argc, char* argv[]) {
	Util::Title("Testing Dependency Package", '=');
	Utilities::putline();
	TypeAnalysis typeAnalysis;
	DependencyAnalysis dependencyAnalysis;
	Task task;//return type is TypeAnlaysisResult 
	task.begin(1); 	//-----create threadpool------
	WorkItem workItemForTypeAnalysis;
	//file orgnizing 
	DataStore tmpPathDstore;
	std::string file1 = "../x64/Test1/2.cpp";
	tmpPathDstore.save(file1);
	std::string file2 = "../x64/Test1/2.h";
	tmpPathDstore.save(file2);
	//file path queue for type analysis
	BlockingQueue<std::string> filePathQueue;
	filePathQueue.enQ(file1);
	filePathQueue.enQ(file2);
	BlockingQueue<TypeQueue> typeQueue; // queue for hold partial type table

	BlockingQueue<std::string> filePathQueueforDepdncy; // file queue for analysis dependency
	filePathQueueforDepdncy.enQ(file1);
	filePathQueueforDepdncy.enQ(file2);

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

	for (auto fst : tmpPathDstore) //multiple threads handle type analysis
		task.doWork(&workItemForTypeAnalysis);

	auto wholeMap = typeQueue.deQ();
	auto tempMap = typeQueue.deQ();
	wholeMap.insert(tempMap.begin(), tempMap.end()); // merge the partial map table
	typeAnalysis.showPairs(wholeMap); 

	BlockingQueue<DependencyList*> dependencyListQeue; //container for dependency result
	WorkItem workItemForDependencyAnalysis; //lambda for dependency analysis
	workItemForDependencyAnalysis = [&]() {
		AST<element> abstractSyntaxtTree; //AST 
		std::string fs = filePathQueueforDepdncy.deQ();
		ConfigParseToConsole configure;
		Parser* pParser = configure.BuildDependencyAnalysis(abstractSyntaxtTree);
		configure.getRepository()->setRefTypeTableMap(wholeMap); //copy type table map to repository
		std::unordered_map<std::string, element*> type_table;
		dependencyAnalysis.dependencyAnalysis(fs, abstractSyntaxtTree, pParser, configure);
		auto denpendencylist = configure.getRepository()->getDependencyList();
		dependencyListQeue.enQ(denpendencylist); //store the dependency relationship in dependencyListQeue
	};
	///////START DEPENDENCY ANALYSIS///////
	task.doWork(&workItemForDependencyAnalysis); 
	auto numfilesfordependency = 1; //display
	while (numfilesfordependency > 0)
	{
		numfilesfordependency--;
		auto relationship = dependencyListQeue.deQ();
		for (auto mainlist : *relationship) {
			std::cout << "\t" << mainlist.first << "-------" << std::endl;
			for (auto secondList : mainlist.second) {
				if (mainlist.first != secondList.first)
					std::cout << "\t\t" << secondList.first << std::endl;
			}
		}
	}
	task.endThreadPool();//stop the thread pool
	task.wait();
	return 0;
}

#endif