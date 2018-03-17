/*The following code calculates the approximate value of Pi using the Leibniz-Madhava formula. The accuracy depends on the value of epsilon.
The smaller the value, the greater the precision.*/ 
#include <iostream>
#include <cmath>
#include <iomanip>

int main() 
{
	const double PI = 3.1415926;
	double approx_pi = 0.0, i = 0.0, epsilon = 0.0000002, sum = 0.0;
	while(fabs(PI - approx_pi) > epsilon)
	{
		sum = sum + (1 / ((2 * i) + 1)) * pow(-1, i);
		approx_pi = 4 * sum;
		++i;
		// std::cout << std::setprecision(12) << "The approximate value of pi is " << approx_pi << "." << std::endl;
	}
	std::cout << std::setprecision(12) << "\nThe approximate value of pi is " << approx_pi << "." << std::endl;
	return 0;
}	