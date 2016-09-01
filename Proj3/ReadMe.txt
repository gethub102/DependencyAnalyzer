 Test files and debug.exe are in x64 directory.

 Build command: devenv Project3.sln /rebuild debug
 Run command: 
	cd x64
	cd Debug
	ParallelDependencyExecutive.exe -p *.cpp -p *.h -d ../Test1
	cd ..
	cd ..

 Requriement 1, 2: Visual Studio 2015 and STL is used in this project.

 Requirement 3: Thread pool package can execute callable object, Task package is a wrap for thread pool. 
	  Task package could execute specified callable object. TypeAnalysis is a package for analyzing type 
	  and global function, and create a table for dependency analysis. DependencyAnalysis is the package 
	  that build dependency relationship for files with the help of type table. ParallelDependencyAnalysis 
	  package is used to commbine analsis type and dependence. ParallelDependencyExecutive is the entrance 
	  for the whole project. It only gets files and thread number and then run the program to paralle 
	  analyzing type and dependence.

 Requirement 4: ThreadPool package is provided to run the enqueued callable object asynchronously.      

 Requirement 5: Task class is provided for executing callable object in ThreadPool.

 Requirement 6: TypeAnalysis package is provided to creating type table refered all the specified c++ files. 
	 This analysis' rules and actions is build in ActionsAndRules package. Analysis is configured by ConfigureParser 
	 package. These two package is used in project 2. And some other packages are used are also some packages 
	 we used in project 1 and 2. This TypeAnalysis package would collect global funciton and type definitions: 
	 classes, structs, enums, typedefs, and aliases, and capture their fully qualified names and files where 
	 they are defined.
         
 Requirement 7: After the TypeAnalysis, and partial type tables are merged, DependencyAnalysis package would 
     analyze the specified files to build their dependence relationship.

 Requirement 8: ParallelDependencyAnalysis package would run the program asynchronously to analyze dependencies 
     of specified files. Analysis process is runned by different threads asynchronously. The threads are provided 
	 by ThreadPool package.
     
 Requirement 9: ParallelDependencyExecutive package is build for the entrance of whole project.
      The commandline in run.bat is:
      ParallelDependencyExecutive.exe -p *.cpp -p *.h -d ../x64/Test1

 Requirement 10: Every package has its own test stub, and ParallelDependencyExecutive combines them together to 
	  fulfil multiple thread dependency analysis.


	Special dependency analysis case:
		1, if cpp file implement the funcion declared by header file. cpp file could depend on header file.

		2, typedef int def_in[]; this situtation can be checked. First type table can get the name def_in 
			and then analysis dependency.

		3, namespace::class::function(); can be hanlded to build dependency relationship for this file and 
			namespace::class defined file.

		4, namespace::class object; this delcaration could be handled to find dependency file.

		5, using namespace Scaner; this situation could be handled for file to build dependency.

		6, this program only create one thread pool, without stop any thread until the end of analysis.