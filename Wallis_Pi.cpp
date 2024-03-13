/*
    The following program approximates Pi using the Wallis product method.

    For details please see: https://en.wikipedia.org/wiki/Wallis_product
*/

#include <iostream>
#include <cmath>
#include <iomanip>
#include <chrono>

#define PRECISION 32

constexpr long double CONST_PI =  3.141592653589793238462643383279;

long double getWallisPi(size_t numItr)
{
    long double product = 1.0;

    for (size_t i = 1; i <= numItr; ++i)
    {
        long double t = 4 * i * i;
        product *= (t / (t - 1));
    }

    return (long double)(2 * product);
}

int main()
{
    size_t itrCount = 0;
    std::cout << "Enter the desired number of iterations." << std::endl;
    std::cin >> itrCount;

    auto start = std::chrono::high_resolution_clock::now();
    long double approx_pi = getWallisPi(itrCount);
    auto stop = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	std::cout << "\nTime taken is " << diff.count() << " milliseconds.\n" << std::endl;

    long double absErr = std::fabs(CONST_PI - approx_pi);
    long double absRelErr = (absErr / CONST_PI) * 100;

    std::cout << std::setprecision(PRECISION) << "Approx value of Pi is      : " << approx_pi << "\n" 
              << std::setprecision(8)         << "Absolute error is          : " << absErr << "\n"
              << std::setprecision(8)         << "Absolute relative error is : " << absRelErr << " %" << std::endl;
}