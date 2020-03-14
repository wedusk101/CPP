/*The following code calculates the approximate value of Pi using Monte Carlo Sampling and utilizes C++ 11 multithreading for performance. Use the
flag -lpthread during compilation to link the Posix thread library pthreads. This code suffers from false sharing problems.*/ 
#include <iostream>
#include <random>
#include <limits>
#include <iomanip>
#include <thread>
#include <chrono>
#include <functional>
#include <unistd.h>

long long getRandom(long long min, long long max, std::mt19937 &generator)
{	
	std::uniform_int_distribution<long long> distribution(min, max);
	return distribution(generator);
}

void calcPi(long long iterations, long long samples, uint32_t numThreads, long long &count)
{
	thread_local std::mt19937 generator;
	long long maxItr = iterations / numThreads;
	for(long long i = 0; i < maxItr ; i++) // sampling
	{	
		long long x = getRandom(0, std::numeric_limits<unsigned int>::max(), generator) % samples + 1;
		long long y = getRandom(0, std::numeric_limits<unsigned int>::max(), generator) % samples + 1;
			if(x * x + y * y <= iterations) // x^2 + y^2 <= r^2 where r is the radius
				count++;		
	}
}

long double getPi(uint32_t numThreads, long long samples)
{
	long long max = samples * samples, sum = 0;
	long long count[numThreads] = {0};
	std::thread *piThreads = new std::thread[numThreads];
	
	for (uint32_t i = 0; i < numThreads; i++)
		piThreads[i] = std::thread(calcPi, max, samples, numThreads, std::ref(count[i]));			
	for(uint32_t i = 0; i < numThreads; i++) 
		piThreads[i].join();
	for(uint32_t i = 0; i < numThreads; i++) 
		sum += count[i];
	
	delete [] piThreads;	
	return (4 * sum)/(long double)max;
}

int main() 
{
	long long samples = 0; // the value of samples controls the precision of the approximation
	long double pi = 0.0;
	uint32_t numThreads = 0;
	
	uint32_t cache_line_size = sysconf(_SC_LEVEL1_DCACHE_LINESIZE); // works for linux - comment out otherwise
	
	std::cout << "Size of long long: " << sizeof(long long) << std::endl;
	std::cout << "CPU cache line size: " << cache_line_size << std::endl; 
	
	std::cout<<"Please enter the sample size. Doubling the sample size quadruples the computation time."<<std::endl;
	std::cin>>samples;
	std::cout<<"Enter the number of threads to spawn." << std::endl;
	std::cin>>numThreads;
		
	auto start = std::chrono::high_resolution_clock::now();
	pi = getPi(numThreads, samples);
	auto stop = std::chrono::high_resolution_clock::now();
	
	std::cout<<"Sample size is "<<samples<<"."<<std::endl;
	std::cout<<std::setprecision(12)<<"Approximate value of Pi is "<<pi<<"." << std::endl;
	auto diff = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
	std::cout << "\nTime taken is " << diff.count() << " seconds." << std::endl;
}