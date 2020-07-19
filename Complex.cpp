// Class Complex implementation


/*
	Complex c1(2, 3);
	Complex c2(3, 5);
	
	Complex c3 = c1.add(c2);


	double x = 1.33;
	double y = 1.33; // x + y = 2.66
	double z = x + y; // check if z equals 2.66 within a given tolerance
	
	double epsilon = 1e-3; // 0.001; // tolerance
	
	if (fabs(z - 2.66) < epsilon)
		print hello;
	else
		print bye;	
	
	
	for (double i = 0; i != 1; i += 0.1)
		print i;

*/

#include <cmath>
#include "Complex.h"

double Complex::getReal()
{
	return this->real;
}

double Complex::getImaginary()
{
	return this->imaginary;
}

void Complex::setReal(double r)
{
	this->real = r;
}

void Complex::setImaginary(double i)
{
	this->imaginary = i;
}	

Complex Complex::add(Complex c)
{
	double r = this->real + c.real;
	double i = this->imaginary + c.imaginary;
	return Complex(r, i);
}

Complex Complex::subtract(Complex c)
{
	double r = this->real - c.real;
	double i = this->imaginary - c.imaginary;
	return Complex(r, i);
}

double Complex::getModulus()
{
	return sqrt(real * real + imaginary * imaginary);
}

double Complex::getArg()
{
	return atan2(imaginary, real);
}

bool Complex::isReal()
{
	double epsilon = 1e-4; // 0.0001; // approximately 0
	return fabs(real) < epsilon;
}

bool Complex::isImaginary()
{
	double epsilon = 1e-4; // 0.0001; // approximately 0
	return fabs(imaginary) < epsilon;
}	