/*This piece of code tests out std::pakcahed_task in the Standard Library. Use the flag -pthread to link the POSIX thread library.*/
#include <functional>
#include <thread>
#include <future>
#include <iostream>

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
	std::packaged_task<T()> job;
	
	Task(std::packaged_task<T()> &job_) : state(TaskState::Uninitialized), job(std::move(job_)) {}
	
	T run()
	{
		state = TaskState::Running;
		std::cout << "Getting future" << std::endl;
		auto fut = job.get_future();
		std::cout << "Starting execution..." << std::endl;
		job();
		std::cout << "Execution finished" << std::endl;
		return fut.get();
	}
};

int main()
{
	Foo f1(7);
	std::packaged_task<int()> tempTask(std::bind(&Foo::fact, &f1));	
	Task<int> task(std::ref(tempTask));
	std::thread t1([&] ()
	{
		int result = task.run();
		std::cout << result << std::endl;
	});
	t1.join();
	
	Foo f2(6);
	std::packaged_task<void()> tempTask2(std::bind(&Foo::printFib, &f2));	
	Task<void> task2(std::ref(tempTask2));
	std::thread t2([&] () { task2.run(); });
	t2.join();
}