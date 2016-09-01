#ifndef TASK_H
#define TASK_H
/////////////////////////////////////////////////////////////////////
//  Task.h - Process tasks in threadpool                           //
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
* This package is a wraper for threadpool. It could start a threadpool
* and process different work item. The number of thread in pool could be 
* specified. 
*
* Public Interface:
* --------------
* void begin(int numThreadinPool); //start threadpool with a thread number
* void doWork(WorkItem* pWi);      //enQ workitem to process
* void endThreadPool();            //end and wait threads in pool
*
* Build Process:
* --------------
* Required Files
* ThreadPool.h
* Build commands
* - devenv Project3.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.0 : 07 Apr 2016
* - first release by Wenbin
*/
#include "../ThreadPool/ThreadPool.h"

class Task {
public:
	void begin(int numThreadinPool);
	void doWork(WorkItem* pWi);
	void wait();
	void endThreadPool();
private:
	ProcessWorkItem process;
	int _numThreadinPool;
};

#endif
//start threadpool with a thread number
inline void Task::begin(int numThreadinPool)
{
	process.start(numThreadinPool);
	_numThreadinPool = numThreadinPool;
}

//enQ workitem to process
inline void Task::doWork(WorkItem* pWi)
{
	process.doWork(pWi);
}

//wait the thread
inline void Task::wait()
{
	process.wait();
}
//end and wait threads in pool
inline void Task::endThreadPool()
{
	while (_numThreadinPool > 0)
	{
		_numThreadinPool--;
		process.doWork(nullptr);
	}
}


