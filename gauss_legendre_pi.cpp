/*

The following program approximates Pi using the Gauss-Legendre algorithm. It is a very fast algorithm
with a quadratic rate of convergence. 

*/

#include <iostream>
#include <cmath>
#include <iomanip>

const long double PI = 3.14159265358979323846; // 3.14159265358979;
const long double EPS = 1e-14;
const long double sqrt2 = 1.41421356237309;


int main()
{
	long double a0 = 1.0, b0 = 1./sqrt2, t0 = 0.25,
	p0 = 1.0, error = 1, pi = 0.0;
	
	int itr = 0;
	
	while(error > EPS)
	{
		long double aTemp = a0;
		long double bTemp = b0;
		long double tTemp = t0;
		long double pTemp = p0;
		
		a0 = (aTemp + bTemp) * 0.5; 
		b0 = sqrt(aTemp * bTemp);
		
		t0 = tTemp - pTemp * (aTemp - a0) * (aTemp - a0);
		p0 = 2 * pTemp;
		
		pi = ((a0 + b0) * (a0 + b0) * 0.25) / t0;
		
		error = std::fabs(PI - pi);
		++itr;		
	}
	
	std::cout << std::setprecision(16) << "Approx. value of Pi is " << pi << std::endl;
	std::cout << "Iterations required: " << itr << std::endl;
}