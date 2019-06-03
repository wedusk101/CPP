#include <iostream>
#include <iomanip>
#include <omp.h>
#include <chrono>

#define NUM_THREADS 4

int main()
{
	const long num_steps = 10000000;
	double step = 0, x = 0, pi = 0, sum[NUM_THREADS] = {0}, total = 0;
		
	step = 1/(double)num_steps;
	
	auto start = omp_get_wtime();
	#pragma omp parallel
	{
		int id = omp_get_thread_num();
		for(unsigned int i = 0; i < num_steps; i += NUM_THREADS)
		{
			x = (i + 0.5) * step;
			sum[id] += 4/(double)(1 + x * x);
		}
	}
	for(int i = 0; i < NUM_THREADS; i++)
		total += sum[i];
	pi = step * total;
	auto stop = omp_get_wtime();
	std::cout << std::setprecision(16) << pi << std::endl;
	std::cout << std::setprecision(12) << "Time taken is " << stop - start << " seconds." << std::endl;
}