/*
	Integrate x^3 + 3x dx over [3, 8] using Monte-Carlo sampling
	
	Given Int{f(x) dx} over [a,b] is approx. = (b - a) * Sum{f(x) evaluated at N random points distributed over [a,b]} / N, where N is the number of samples
*/

#include <iostream>
#include <random>

int main()
{
	uint32_t samples = 0;
	double sum = 0;
	double range = 8 - 3;
	std::default_random_engine seed;
	std::uniform_real_distribution<double> rnd(0, 1.0);
	std::cout << "Please enter the number of samples." << std::endl;
	std::cin >> samples;
	for (int i = 0; i < samples; i++)
	{	
		double x = 5 * rnd(seed) + 3; // rand between 3 and 8
		sum += x * x * x + 3 * x;
	}	
	std::cout << "Result: " << (range * sum) / (double)samples << std::endl;
}