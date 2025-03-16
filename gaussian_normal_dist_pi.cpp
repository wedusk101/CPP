/*

The following program approximates Pi using the Gaussian distribution, also known as the Normal distribution. We can integrate this
function and use the result to calculate Pi. We are using Reimann sum to compute the integral of the following function:

	F = e^{-(x * x)} and
	
	I = Integration from -inf to +inf [ F(x) dx ] = sqrt(Pi) + erf

For details on the Gaussian distribution please see: 

	- https://en.wikipedia.org/wiki/Normal_distribution
	- https://www.wolframalpha.com/input?i2d=true&i=Power%5Be%2C-Power%5Bx%2C2%5D%5D
	- https://kconrad.math.uconn.edu/blurbs/analysis/gaussianintegral.pdf

*/

#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>

#define PRECISION 30

constexpr long double CONST_PI =  3.141592653589793238462643383279;
constexpr long double CONST_E  =  2.718281828459045235360287471352;

int main(int argc, char *argv[])
{
	uint64_t steps = 0;
	
	std::cout << "This program calculates the approximate value of Pi using the Gaussian distribution." << std::endl;
	std::cout << "Please enter the desired number of iterations." << std::endl;
	std::cin >> steps;	
	
	const long double min = -1e+5f;
	const long double max = 1e+5f;
	const long double dx = (max - min) / steps;
	
	long double x = min;
	long double val = 0;
	
	auto start = std::chrono::high_resolution_clock::now();
		
	for (uint64_t i = 0; i < steps; ++i)
	{
		long double x2 = -(x * x);
		val += std::pow(CONST_E, x2) * dx;
		x += dx;
	}
	
	long double approx_pi = val * val;
	long double absErr = std::fabs(CONST_PI - approx_pi);
	long double absRelErr = (absErr / CONST_PI) * 100;
	
	auto stop = std::chrono::high_resolution_clock::now();

	std::cout << std::setprecision(PRECISION) << "\nApprox value of Pi is      : " << approx_pi << "\n" 
				<< std::setprecision(8)         << "Absolute error is          : " << absErr << "\n"
				<< std::setprecision(8)         << "Absolute relative error is : " << absRelErr << " %" << std::endl;
			  
	auto diff = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
	std::cout << "\nTime taken is " << diff.count() << " seconds." << std::endl;
			  
	return 0;
}