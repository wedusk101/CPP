/*The following code displays the nth term of the Fibonacci sequence, where user inputs n. It uses the OpenMP task system introduced with OpenMP 3.0 where
logical units of work called tasks are created by a single thread (and pushed into a queue), which are then executed in parallel by all the threads. Please note,
the algorithm used here to calculate the Fibonacci sequence is a naive brute force one involving recursion and is extremely inefficient. The performance of the OpenMP
task system seen here is *NOT* indicative of its usual behavior. It is significantly slower than the serial version of the same program.*/

#include <iostream>
#include <cstddef>
#include <chrono>
#include "omp.h"

size_t threadCount = 0;

size_t fib(size_t n)
{
	size_t x = 0, y = 0;
	if (n < 2)
		return n;

#pragma omp task shared(x) 
	x = fib(n-1);
#pragma omp task shared(y)
	y = fib(n-2);
#pragma omp taskwait
	return x + y;
}

int main()
{
	size_t term = 0, result = 0;
	std::cout << "Enter the desired term of the Fibonacci sequence to display." << std::endl;
	std::cin >> term;
	std::cout << "Enter the number of threads to spawn." << std::endl;
	std::cin >> threadCount;
	threadCount = threadCount < 1 ? 1 : threadCount;
	auto start = std::chrono::high_resolution_clock::now();
#pragma omp parallel num_threads(threadCount)
	{
#pragma omp single
		result = fib(term);
	}
	auto stop = std::chrono::high_resolution_clock::now();
	auto diff = std::chrono::duration_cast<std::chrono::seconds>(stop - start); 
	std::cout << "Term " << term << " of the Fibonacci sequence is " << result << "." << std::endl;
	std::cout << "Time taken is " << diff.count() << " seconds." << std::endl;
}