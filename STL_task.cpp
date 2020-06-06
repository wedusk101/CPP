/*This piece of code tests out std::pakcahed_task in the Standard Library. Use the flag -pthread to link the POSIX thread library.*/
#include <functional>
#include <thread>
#include <iostream>
#include <memory>

struct Foo
{
	int num;
	
	Foo(int n) : num(n) {}
	
	int fact()
	{
		if (num  <= 1)
			return 1;
		else
		{
			int p = 1;
			for (int i = 1; i <= num; i++)
				p *= i;
			return p;
		}
	}
	
	void printFib()
	{
		int current = 1, next = 1, fib = 0;
		
		if(num < 1)
		{
			std::cout << "Invalid input" << std::endl;
			return;
		}
		
		if (num <= 2)
		{
			std::cout << "Fibonacci term: " << num << std::endl;
			return;
		}
		
		for(int i = 3; i <= num; i++)
		{
			fib = current + next;
			current = next;
			next = fib;
		}
		std::cout << "Fibonacci term: " << fib << std::endl;
	}
};

struct ITask
{
	enum struct TaskState
	{
		Uninitialized,
		Waiting,
		Running,
		Paused,
		Finished
	};
};


template
<typename T>
struct Task : public ITask
{
	TaskState state;
	std::function<T()> job;
	
	Task(std::function<T()> &job_) : state(TaskState::Uninitialized), job(std::move(job_)) {}
	
	T run()
	{
		state = TaskState::Running;
		std::cout << "Starting execution..." << std::endl;
		return job();
	}
};

int main()
{
	Foo f1(15);
	std::function<int()> tempTask(std::bind(&Foo::fact, &f1));	
	Task<int> task(std::ref(tempTask));
	std::thread t1([&] ()
	{
		int result = task.run();
		std::cout << result << std::endl;
	});
	t1.join();
	
	Foo f2(20);
	std::function<void()> tempTask2(std::bind(&Foo::printFib, &f2));	
	Task<void> task2(std::ref(tempTask2));
	std::thread t2([&] () { task2.run(); });
	t2.join();	

	Foo f3(12);
	std::function<int()> tempTask3(std::bind(&Foo::fact, &f3));	
	ITask *tPtr = new Task<int>(std::ref(tempTask3));
	std::thread t3([&] ()
	{
		Task<int> *taskPtr = (Task<int>*)tPtr;
		int result = taskPtr->run();
		std::cout << result << std::endl;
	});
	t3.join();

}