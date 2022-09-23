/* 

The following code uses the fundamental theorem of arithmetic to check if two pieces of 
text are anagrams of each other. Anagrams are strings made of the same set of characters.
For example, "Dirty room" and "Dormitory" are anagrams. Please note that the program is
NOT case sensitive to input and ignores spaces in the strings. 

Please visit https://wordsmith.org/anagram/hof.html for a list of anagrams.

The algorithm works by generating a lookup table (LUT) of the letters of the alphabet and
their corresponding prime number values. For example, a = 2, b = 3, c = 5, d = 7 etc.
The LUT we generate spans the basic ASCII space from 0 through 127. This LUT is then used
to read the prime number values of the corresponding letters in the input string. 
The resulting numbers are then multiplied and stored in a variable. The same operation is
performed for the other input string. The resulting products are compared. This comparison
will return true if and only if the two input strings are anagrams. This is due to the 
properties of the fundamental tehorem of arithmetic which states that any composite
number can be factored into one one set of prime factors.

NOTE: This program relies on unsigned integer overflow which is defined behavior per the 
C++ standard.

*/

#include <iostream>
#include <cmath>
#include <cstddef>
#include <algorithm>

void initLUT(size_t*, const size_t &);
bool isAnagram(const std::string &, const std::string &, size_t*);
bool isPrime(size_t);
size_t getNextPrime(size_t &);

int main()
{
	std::string first, second;
	std::cout << "This program checks if two strings are anagrams of each other." << std::endl;
	std::cout << "Please enter the first string." << std::endl;
	getline(std::cin, first);
	std::cout << "Please enter the second string." << std::endl;
	getline(std::cin, second);
	std::cout << "\nThe input strings are: \n" << first << " - " << second << std::endl;
	size_t *map = new size_t[128];
	
	// convert strings to lower case
	std::transform(first.begin(), first.end(), first.begin(),
    [](unsigned char c){ return std::tolower(c); });
	
	std::transform(second.begin(), second.end(), second.begin(),
    [](unsigned char c){ return std::tolower(c); });
	
	initLUT(map, 128);
	if(isAnagram(first, second, map))
		std::cout << "The two strings are anagrams." << std::endl;
	else
		std::cout << "The two strings are not anagrams." << std::endl;
	delete [] map;
}

bool isPrime(size_t n)
{
	size_t i = 0;
	if(n <= 1)
		return false;
	else
		for(i = 2; i <= sqrt(n); i++)
			if(n % i == 0)
				return false;
	return true;
}

size_t getNextPrime(size_t &p) 
{
	for( p += 1; ; p++)
		if(isPrime(p))
			return p;
}

// initializes the lookup table with consecutive primes
void initLUT(size_t *map, const size_t &size) 
{
	size_t j = 1;
	for(int i = 0; i < size; i++)
		map[i] = getNextPrime(j);
	map[32] = 1; // we ignore spaces
}

bool isAnagram(const std::string &first, const std::string &second, size_t *lookup)
{
	size_t firstProduct = 1, secondProduct = 1;
	for(size_t i = 0; i < first.size(); i++)
		firstProduct *= lookup[(int)first[i]];
	for(size_t i = 0; i < second.size(); i++)
		secondProduct *= lookup[(size_t)second[i]];
	return (firstProduct == secondProduct);
}