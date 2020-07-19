#include <iostream>
#include "Complex.h"


int main()
{
	double r = 0, im = 0;
	std::cout << "Please enter real and im part" << std::endl;
	std::cin >> r >> im;
	Complex c1(r, im);
	std::cout << "Please enter real and im part" << std::endl;
	std::cin >> r >> im;
	Complex c2(r, im);
	Complex c3 = c1.add(c2);
	Complex c4 = c1.subtract(c2);
	double modC1 = c1.getModulus();
	
	std::cout << c3.getReal() << " " << c3.getImaginary() << std::endl;
	std::cout << c4.getReal() << " " << c4.getImaginary() << std::endl;
	std::cout << "c1 Mod " << modC1 << std::endl;
}
