/*This piece of code tests out std::pakcahed_task in the Standard Library. Use the flag -pthread to link the POSIX thread library.*/
#include <functional>
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
};

int main()
{
	Foo f(7);
	std::packaged_task<int()> task(std::bind(&Foo::fact, &f));
	std::cout << "Getting future" << std::endl;
	auto fut = task.get_future();
	std::cout << "Starting..." << std::endl;
	task();
	std::cout << "Execution finished" << std::endl;
	int res = fut.get();
	std::cout << res <<std::endl;
}
