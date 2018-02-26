#include <iostream>
#include <ctime>
#include <cstdlib>
#include <omp.h>

int main()
{
	srand(time(NULL));
	unsigned long long x= 0, sum = 0;
	std::clock_t start, stop;
	std::cout<<"Please enter the number of terms to add."<<std::endl;
	std::cin>>x;
	//omp_set_num_threads(2);
	start = std::clock();
	//#pragma omp parallel for
	for(int i = 0; i <= x; i++)
	{
		unsigned long long r = rand() % 100; 
		sum += r * r;
	}
	stop = std::clock();
	std::cout<<"The sum is "<<sum<<"."<<std::endl;
	std::cout<<"Time taken is "<<((float)stop - (float)start)/CLOCKS_PER_SEC<<" seconds."<<std::endl;
}