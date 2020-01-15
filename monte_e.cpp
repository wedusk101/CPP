/*
The following program calculates the approximate value of the irrational mathematical constant e using Monte Carlo sampling.

The algorithm involved is as follows:

* Draw random variables S_r in the real open interval (0, 1).
* We then count the number of draws required N_i such that the Sum(S_r) > 1.
* We repeat this process K number of times where K is the total number of samples.
* approx_e = Sum(N_i(1,K)) / K

*/

#include <iostream>
#include <random>
#include <chrono>
#include <iomanip>

int getRandomDrawCount(std::default_random_engine &);

int main()
{
	unsigned samples = 0;
	long double sum = 0.0, approx_e = 0.0, error = 1.0, eps = 1e-12;
	const long double e_const = 2.718281828459045;
	std::cout << "This program estimates the value of the irrational constant e using Monte Carlo sampling." <<std::endl;
	std::default_random_engine generator;
	auto start = std::chrono::high_resolution_clock::now();
	while(error > eps)
	{
		sum += getRandomDrawCount(generator);
		samples++;
		approx_e = sum / samples;
		error = fabs(approx_e - e_const);
	}	
	std::cout << std::setprecision(14) << "The approximate value of e is " << approx_e << "." << std::endl;
	std::cout << "The absolute error is " << error << "." << std::endl;
	auto stop = std::chrono::high_resolution_clock::now();
	auto diff = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
	std::cout << "Time taken is " << diff.count() << " seconds." << std::endl;
}

int getRandomDrawCount(std::default_random_engine &generator)
{
	double s = 0.0;
	int count = 0;
	std::uniform_real_distribution<double> rnd(0.0, 1.0);
	while (s <= 1.0)
	{
		s += rnd(generator);
		count++;
	}
	return count;
}	