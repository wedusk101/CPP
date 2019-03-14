/*This program calculates Pi by using the Nilakantha method, which he introduced in the 15th century.
Pi = 3 + 4/(2 * 3 * 4) - 4/(4 * 5 * 6) + 4/(6 * 7 * 8) - 4/...
*/

#include <iostream>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <thread>

int main()
{
	const double PI = 3.141592653589793;
	double calc_PI = 0, sum = 0, epsilon = 1e-10, erf = 1234;
	int x = 2, y = 3, z = 4, i = 0;
	auto start = std::chrono::high_resolution_clock::now();
	while(fabs(erf) > epsilon)
	{
		sum += pow(-1, i) / (x * y * z);
		calc_PI = 3 + (4 * sum);
		erf = PI - calc_PI;
		x += 2;
		y += 2;
		z += 2;
		i++;
	}
	// std::this_thread::sleep_for(std::chrono::seconds(2)); // for testing the timer
	auto stop = std::chrono::high_resolution_clock::now();
	auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
	std::cout << std::setprecision(16) << "Pi is " << PI << "." << std::endl;
	std::cout << std::setprecision(16) << "Calculated value of Pi is " << calc_PI << "." << std::endl;
	std::cout << std::setprecision(16) << "The error is " << erf << "." << std::endl;	
	std::cout << std::setprecision(12) << "Time taken is " << diff.count()/1e-9 << " seconds." << std::endl;
}