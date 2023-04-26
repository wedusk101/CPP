/*
	A number is considered a beautiful number if its binary representation has at least one zero surrounded  by 1s 
	and at most three consecutive zeroes surrounded by ones. 
	
	e.g. Binary(5) = 101 => Beautiful but Binary(67) = 1000011 => Ugly
*/

#include <iostream>
#include <bitset>
#include <string>
#include <algorithm>

bool isBeautiful(int n)
{
	std::string binary = std::bitset<32>(n).to_string();
	std::cout<< "Binary: " << binary << std::endl;
	
	size_t idx = binary.find_first_of('1');
	int numZeroes = std::count_if(binary.begin() + idx, binary.end(),
	[](char c) { return c == '0'; });
	
	return (numZeroes >= 1 && numZeroes <= 3);
}

int main()
{
	int n;
	std::cout << "Please enter a number." << std::endl;
	std::cin >> n;
	
	if (isBeautiful(n))
		std::cout << "Beautiful :*" << std::endl;
	else
		std::cout << "Ugly! D:<" << std::endl;
	
	return 0;
}