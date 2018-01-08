//Vector00.cpp
#include <iostream>
#include <cmath>
#include "Vector.h"

using namespace std;

Vector::Vector()
{
	x = 0;
	y = 0;
	z = 0;
}

Vector::Vector(double i, double j, double k)
{
	x = i;
	y = j;
	z = k;
}

void Vector::initVector(double i, double j, double k)
{
	setCompX(i);
	setCompY(j);
	setCompZ(k);
}

void Vector::setCompX(double i)
{
	x = i;
}

void Vector::setCompY(double j)
{
	y = j;
}

void Vector::setCompZ(double k)
{
	z = k;
}

double Vector::magnitude() const
{
	return sqrt(x*x + y*y + z*z);
}

double Vector::dotProduct(Vector v) const
{
	return x*v.getCompX() + y*v.getCompY() + z*v.getCompZ();
}

void Vector::display() const
{
	cout<<x<<"i + "<<y<<"j + "<<z<<"k"<<endl;
}