/*The following code calculates the approximate value of Pi using Monte Carlo Sampling and utilizes C++ 11 multithreading for performance. Use the
flag -pthread during compilation to link the POSIX thread library pthreads. This code utilizes padding of the shared array to avoid false sharing.*/ 
#include <iostream>
#include <random>
#include <limits>
#include <iomanip>
#include <thread>
#include <chrono>
#include <functional>
#include <unistd.h>
#include <cstdint>

#define CACHE_LINE_SIZE 64 // 64 bytes 
#define PADDING 8 // each row will take up 64 bytes - 8 elements each of 8 bytes [sizeof(size_t)]

size_t getRandom(size_t min, size_t max, std::mt19937 &generator)
{	
	std::uniform_int_distribution<size_t> distribution(min, max);
	return distribution(generator);
}

void calcPi(size_t iterations, size_t samples, uint32_t numThreads, size_t &count)
{
	thread_local std::mt19937 generator;
	size_t maxItr = iterations / numThreads;
	for(size_t i = 0; i < maxItr ; i++) // sampling
	{	
		size_t x = getRandom(0, std::numeric_limits<unsigned int>::max(), generator) % samples + 1;
		size_t y = getRandom(0, std::numeric_limits<unsigned int>::max(), generator) % samples + 1;
			if(x * x + y * y <= iterations) // x^2 + y^2 <= r^2 where r is the radius
				count++;		
	}
}

long double getPi(uint32_t numThreads, size_t samples)
{
	size_t max = samples * samples, sum = 0;
	size_t count[numThreads][PADDING] = {0}; // padding to avoid false sharing
	std::thread *piThreads = new std::thread[numThreads];
	
	for (uint32_t i = 0; i < numThreads; i++)
		piThreads[i] = std::thread(calcPi, max, samples, numThreads, std::ref(count[i][0]));			
	for(uint32_t i = 0; i < numThreads; i++) 
		piThreads[i].join();
	for(uint32_t i = 0; i < numThreads; i++) 
		sum += count[i][0];
	
	delete [] piThreads;	
	return (4 * sum)/(long double)max;
}

int main() 
{
	size_t samples = 0; // the value of samples controls the precision of the approximation
	long double pi = 0.0;
	uint32_t numThreads = 0;
	
	uint32_t cache_line_size = sysconf(_SC_LEVEL1_DCACHE_LINESIZE); // works for linux - comment out otherwise
	
	std::cout << "Size of size_t: " << sizeof(size_t) << std::endl;
	std::cout << "CPU cache line size: " << cache_line_size << std::endl; 
	
	std::cout << "Please enter the sample size. Doubling the sample size quadruples the computation time."<<std::endl;
	std::cin >> samples;
	std::cout << "Enter the number of threads to spawn." << std::endl;
	std::cin >> numThreads;
	
	std::cout << "Calculating..." << std::endl;
	auto start = std::chrono::high_resolution_clock::now();
	pi = getPi(numThreads, samples);
	auto stop = std::chrono::high_resolution_clock::now();
	
	std::cout << "Sample size is " << samples << "." << std::endl;
	std::cout << std::setprecision(12) << "Approximate value of Pi is " << pi << "." << std::endl;
	auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	std::cout << "\nTime taken is " << diff.count() << " milliseconds." << std::endl;
}