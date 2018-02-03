/*The following code calculates the approximate value of Pi using Monte Carlo Sampling.*/ 
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <omp.h>

int main() 
{
	srand(time(NULL));
	long long count = 0, max = 0, samples = 0, x = 0, y = 0; // the value of samples controls the precision of the approximation
	long double pi = 0.0, start = 0.0, stop = 0.0;
	int num_of_threads;
	int nthreads;
	std::cout<<"Please enter the sample size. Doubling the sample size quadruples the time taken."<<std::endl;
	std::cin>>samples;
	std::cout<<"Please enter the number of threads to spawn."<<std::endl;
	std::cin>>num_of_threads;
	max = samples * samples; // square of the radius = total number of samples
	omp_set_num_threads(num_of_threads);
	start = omp_get_wtime();
	#pragma omp parallel for private(x,y) reduction(+:count)
	for(long long i = 0; i < max; i++) // sampling
	{	
		x = rand() % samples + 1;
		y = rand() % samples + 1;
			if(x * x + y * y <= max) // x^2 + y^2 <= r^2 where r is the radius
				count++;		
	}
	//nthreads = omp_get_num_threads();
	pi = (4 * count)/(long double)max;
	stop = omp_get_wtime();
	std::cout<<"Sample size is "<<samples<<"."<<std::endl;
	std::cout<<std::setprecision(12)<<"Approximate value of Pi is "<<pi<<"."<<std::endl;
	std::cout<<"Time taken is "<<stop - start<<" seconds."<<std::endl;
}