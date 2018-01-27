/*The following code calculates the approximate value of Pi using Monte Carlo Sampling.*/ 
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <iomanip>

int main() 
{
	srand(time(NULL));
	long long count = 0, max = 0, samples = 0; // the value of samples controls the precision of the approximation
	long double pi = 0.0;
	std::clock_t start, stop;
	std::cout<<"Please enter the sample size. Doubling the sample size quadruples the computation time."<<std::endl;
	std::cin>>samples;
	max = samples * samples; // square of the radius = total number of samples
	start = std::clock();
	for(long long i = 0; i < max; i++) // sampling
	{	
		long long x = rand() % samples + 1;
		long long y = rand() % samples + 1;
			if(x * x + y * y <= max) // x^2 + y^2 <= r^2 where r is the radius
				count++;		
	}
	pi = (4 * count)/(long double)max;
	stop = std::clock();
	double time_elapsed = ((double)stop - (double)start)/CLOCKS_PER_SEC;
	std::cout<<"Sample size is "<<samples<<"."<<std::endl;
	std::cout<<std::setprecision(12)<<"Approximate value of Pi is "<<pi<<"."<<std::endl;
	std::cout<<"Time taken is "<<time_elapsed<<" seconds."<<std::endl;
}