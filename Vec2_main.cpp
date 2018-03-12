//Vec2_main.cpp
#include <iostream>
#include "Vec2.h"

int main()
{
	int x = 5, y = 6, a = 2, b = 3;
	Vec2 v1(5, 6);
	Vec2 v2(2, 3);
	Vec2 result = v1.add(v2);
	double dotVal = v1.dot(v2);
	Vec2 sum = v1 + v2; // overloaded
	std::cout << "Resultant Vector i j : " << result.x << " " << result.y << std::endl;
	std::cout << "Dot product is " << dotVal << "." << std::endl; 
	std::cout << "Overloaded Dot Product: " << v1 % v2 << std::endl;
	std::cout << "Overloaded Resultant Vector i j : " << sum.x << " " << sum.y << std::endl;
	result.display();
	(v1 + v2).display();
	v1.display();
	v2.display();
}