/*

The following program approximates Pi using the Chudnovsky algorithm. It is a very fast algorithm that has been used many times 
for setting the world record for calculating the most number of digits of Pi. 

For details on the algorithm please see: https://en.wikipedia.org/wiki/Chudnovsky_algorithm

*/


#include <cmath>
#include <iostream>
#include <iomanip>

#define PRECISION 30

constexpr long double CONST_PI =  3.141592653589793238462643383279;

// Chudnovsky algorithm
long double getPi(size_t max)
{
    constexpr long double C = 426880.0 * sqrt(10005.0);
    constexpr long double bigTerm = (640320.0 * 640320.0) * 640320.0; 

    long double Mq = 1.0, Lq = 13591409.0, Xq = 1.0, Kq = -6, sum = 0.0;
    for (size_t i = 0; i < max; ++i)
    {
        sum += (Mq * Lq) / Xq;
        Kq += 12;
        long double n = ((Kq * Kq) * Kq) - 16 * Kq;
        long double d = ((i + 1) * (i + 1)) * (i + 1);
        Mq *= (n / d);
        Lq += + 545140134.0;
        Xq *= -bigTerm;
    } 

    return C / sum;
}

int main()
{
    size_t numItr = 0;
    std::cout << "This program calculates the approximate value of Pi using the Chudnovsky algorithm." << std::endl;
    std::cout << "Please enter the desired number of iterations." << std::endl;
    std::cin >> numItr;  

    long double approx_pi = getPi(numItr);
    long double absErr = std::fabs(CONST_PI - approx_pi);
    long double absRelErr = (absErr / CONST_PI) * 100;

    std::cout << std::setprecision(PRECISION) << "Approx value of Pi is      : " << approx_pi << "\n" 
              << std::setprecision(8)         << "Absolute error is          : " << absErr << "\n"
              << std::setprecision(8)         << "Absolute relative error is : " << absRelErr << " %" << std::endl;
}