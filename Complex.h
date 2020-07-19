#ifndef COMPLEX_H__ // include guards - macros for the preprocessor
#define COMPLEX_H__

#include <string>

class Complex
{
	double real;
	double imaginary;
	
	int foo; // private
	
public:
	Complex(double r, double i)
	{
		real = r;
		imaginary = i;
	}
		
	int bar; // public
	
	double getReal();
	double getImaginary();
	void setReal(double r);
	void setImaginary(double i);
	
	Complex add(Complex c);
	Complex subtract(Complex c);
	double getModulus();
	double getArg();
	bool isReal();
	bool isImaginary();
};

#endif
