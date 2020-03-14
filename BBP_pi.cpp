/*
	The following program estimates Pi using the Bailey-Borwein-Plouffe (BBP) formula.
	
	Pi = Sum(k = 0, inf)[(1/(16^k)) * [(4/(8k + 1)) - (2/(8k + 4)) - (1/(8k + 5)) - (1/(8k + 6))]
*/
#include <iostream>
#include <chrono>
#include <iomanip>
#include <cmath>

long double getPiDigit(long long iteration)
{
	long double baseSixteenMultipler = 1./pow(16, iteration);  
	long double firstTerm = 4./(8 * iteration + 1);
	long double secondTerm = 2./(8 * iteration + 4);
	long double thirdTerm = 1./(8 * iteration + 5);
	long double fourthTerm = 1./(8 * iteration + 6);
	return baseSixteenMultipler * (firstTerm - secondTerm - thirdTerm - fourthTerm);
}

int main()
{
	long long iterations = 0;
	long double pi = 0;
	std::cout << "Please enter the desired number of iterations for estimating Pi." << std::endl;
	std::cin >> iterations;
	
	auto start = std::chrono::high_resolution_clock::now();
	for (long long i = 0; i < iterations; i++)
		pi += getPiDigit(i);
	auto stop = std::chrono::high_resolution_clock::now();
	
	std::cout << "Number of iterations: " << iterations << "." << std::endl;
	std::cout << std::setprecision(16) << "Approximate value of Pi is " << pi << "." << std::endl;
	auto diff = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
	std::cout << "\nTime taken is " << diff.count() << " seconds." << std::endl;
}