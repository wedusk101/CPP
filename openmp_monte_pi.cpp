/*The following code calculates the approximate value of Pi using Monte Carlo Sampling.*/ 
#include <iostream>
#include <iomanip>
#include <omp.h>
#include <random>

int main() 
{
	std::default_random_engine generator;
	std::uniform_real_distribution<double> rnd(0, 1);
	long double count = 0, samples = 0; // the value of samples controls the precision of the approximation
	long double pi = 0.0, start = 0.0, stop = 0.0;
	int num_of_threads;
	int nthreads;
	std::cout<<"Please enter the sample size. Doubling the sample size quadruples the time taken."<<std::endl;
	std::cin>>samples;
	std::cout<<"Please enter the number of threads to spawn."<<std::endl;
	std::cin>>num_of_threads;
	long double max = samples * samples; // square of the radius = total number of samples
	omp_set_num_threads(num_of_threads);
	start = omp_get_wtime();
	#pragma omp parallel reduction(+:count)
	{	
		#pragma omp for 
		for(long long i = 0; i < (long long)max; i += 1) // sampling
		{	
			long double x = rnd(generator) * samples;
			long double y = rnd(generator) * samples;
			long double dist = x * x + y * y;
			if(dist <= max) // x^2 + y^2 <= r^2 where r is the radius
				count++;		
		}
	}
	stop = omp_get_wtime();
	pi = (4 * count)/(long double)max;
	std::cout<<"Sample size is "<<samples<<"."<<std::endl;
	std::cout<<std::setprecision(12)<<"Approximate value of Pi is "<<pi<<"."<<std::endl;
	std::cout<<"Time taken is "<<stop - start<<" seconds."<<std::endl;
}