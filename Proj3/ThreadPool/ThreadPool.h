#ifndef THREADPOOL_H
#define THREADPOOL_H
///////////////////////////////////////////////////////////////////////
// QueuedWorkItems.h - child thread processes enqueued work items    //
//                                                                   //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2016         //
///////////////////////////////////////////////////////////////////////
/*
* A single child thread processes work items equeued by main thread
*/


//delete the reusult;

#include <thread>
#include <functional>
#include "../BlockingQueue/BlockingQueue.h"


using WorkItem = std::function<void()>;


/////////////////////////////////////////////////////////////////////// 
// class to process work items
// - each work item is processed sequentially on a single child thread

class ProcessWorkItem
{
public:
	void start(int numThreadinPool);
	void doWork(WorkItem* pWi); // doWork(std::function<Result()> <Result>* pWi)? class and function using template should specify T
	void wait();
	~ProcessWorkItem();
private:
	std::thread* _pThread;
	BlockingQueue<WorkItem*> _workItemQueue;
	//BlockingQueue<Result> _resultsQueue;
};
//----< wait for child thread to terminate >---------------------------

void ProcessWorkItem::wait()
{
	_pThread->join();
}
//----< enqueue work item >--------------------------------------------


void ProcessWorkItem::doWork(WorkItem* pWi)
{
	_workItemQueue.enQ(pWi);
}

//----< start child thread that dequeus work items >-------------------

void ProcessWorkItem::start(int threadNum)
{
	std::function<void()> threadProc =
		[&]() { //why is it a reference?
		while (true)
		{
			WorkItem* pWi = _workItemQueue.deQ();
			if (pWi == nullptr)
			{
				//doWork(nullptr);
				std::cout << "\n  shutting down work item processing\n";
				return;
			}
			(*pWi)(); //? dereference and run ()// (_workItemQueue.deQ())()
		}
	};
	while (threadNum > 0)
	{
		threadNum--;
		_pThread = new std::thread(threadProc);
	}
}
//----< clean up heap >------------------------------------------------

ProcessWorkItem::~ProcessWorkItem()
{
	delete _pThread;
}


#endif