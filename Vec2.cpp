//Vec2.cpp
#include <iostream>
#include "Vec2.h"

double Vec2::dot(const Vec2 &v) const
{
	return x * v.x + y * v.y;
}

Vec2 Vec2::add(const Vec2 &v) const
{
	Vec2 result;
	result.x = x + v.x;
	result.y = y + v.y;
	return result;	
}

void Vec2::display() const
{
	std::cout << " i " << x << " j " << y << std::endl;
}