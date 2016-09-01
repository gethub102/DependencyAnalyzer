#ifdef TEST_TASK

#include "Task.h"
#include "../Utilities/Utilities.h"

using Util = Utilities::StringHelper;
using WorkResult = std::string;

int main(int argc, char* argv[]){
	Util::Title("Enqueued Work Items");

	std::cout << "\n  main thread id = " << std::this_thread::get_id();
	Task task;
	task.begin(3);

	WorkItem wi1 = []() {
		std::cout << "\n  working on thread " << std::this_thread::get_id();
		//return "Hello from wi1";
	};

	task.doWork(&wi1);


	WorkItem wi2 = []()
	{
		std::cout << "\n  working on thread " << std::this_thread::get_id();
		size_t sum = 0;
		for (size_t i = 0; i < 100000; ++i)
			sum += i;
		//return "wi2 result = " + Utilities::Converter<size_t>::toString(sum);
	};

	task.doWork(&wi2);

	std::function<void()> voidfunciton = []() {
		std::cout << "\n void function to check lambda" << std::endl;
	};

	task.doWork(&voidfunciton);

	task.endThreadPool();
	task.wait();

	return 0;
}


#endif