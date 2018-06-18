/* The following code uses the fundamental theorem of arithmetic to check if two pieces of texts
are anagrams of each other. Please note that the program is case sensitive.*/
#include <iostream>
#include <cmath>

void initLUT(int *loopkup, const int &sizeLUT);
bool isAnagram(const std::string &a, const std::string &b, int *lookup);
bool isPrime(int);
int getNextPrime(int &);

int main()
{
	std::string first, second;
	std::cout << "This program checks if two strings are anagrams of each other." << std::endl;
	std::cout << "Please enter the first string." << std::endl;
	getline(std::cin, first);
	std::cout << "Please enter the second string." << std::endl;
	getline(std::cin, second);
	std::cout << "The input strings are: \n" << first << " - " << second << std::endl;
	int *map = new int[128];
	initLUT(map, 128);
	if(isAnagram(first, second, map))
		std::cout << "The two strings are anagrams." << std::endl;
	else
		std::cout << "The two strings are not anagrams." << std::endl;
	delete [] map;
}

bool isPrime(int n)
{
	int i = 0;
	if(n <= 1)
		return false;
	else
		for(i = 2; i <= sqrt(n); i++)
			if(n % i == 0)
				return false;
	return true;
}

int getNextPrime(int &p)
{
	for( p += 1; ; p++)
		if(isPrime(p))
			return p;
}

void initLUT(int *map, const int &size)
{
	int j = 1;
	for(int i = 0; i <= 127; i++)
		map[i] = getNextPrime(j);
	map[32] = 1;
}

bool isAnagram(const std::string &first, const std::string &second, int *lookup)
{
	int firstProduct = 1, secondProduct = 1;
	for(int i = 0; i < first.size(); i++)
		firstProduct *= lookup[(int)first[i]];
	for(int i = 0; i < second.size(); i++)
		secondProduct *= lookup[(int)second[i]];
	// std::cout << firstProduct << " " << secondProduct << " " << (int)second[3] << " " << lookup[(int)second[3]] << " " << std::endl;
	if(firstProduct == secondProduct)
		return true;
	else
		return false;
}