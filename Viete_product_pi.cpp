/*
    The following code approximates Pi using Viète's formula which is:

    2/Pi = sqrt(2)/2 + sqrt(2 + sqrt(2))/2 + sqrt(2 + sqrt(2 + sqrt(2)))/2 ....

    For details, please see: https://en.wikipedia.org/wiki/Vi%C3%A8te%27s_formula

*/

#include <iostream>
#include <cmath>
#include <iomanip>

double getVietePi(size_t nItr)
{
    double rootVal = sqrt(2);
    double product = 1.0f;

    for (int i = 0; i < nItr; ++i)
    {
        product *= rootVal * 0.5;
        rootVal = sqrt(2 + rootVal);
    }

    return 2.0 / product;
}

int main()
{
    size_t itrCount = 0;
    std::cout << "Enter the desired number of iterations." << std::endl;
    std::cin >> itrCount;
    std::cout << std::setprecision(16) << "Approx Pi: " << getVietePi(itrCount) << std::endl;
}