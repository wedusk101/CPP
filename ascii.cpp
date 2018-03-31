#include <iostream>
#include <cmath>

#define SIZE 

int main()
{
	for(double x = 0; x < 20; x++)
	{
		for(double y = 0; y < 20; y++)
		{
			if( y == round(sin(x)) + 4 || y == 4 - round(sin(x))) // sin curve
				std::cout << "*";
			else
				std::cout << " ";
		}
		std::cout << "\n";
	}
}