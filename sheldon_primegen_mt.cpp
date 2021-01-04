/*This piece of code generates the Sheldon Prime 73, in base 10 and utilizes multithreading for performance.*/

#include <iostream>
#include <unordered_map>
#include <utility>
#include <chrono>
#include <cstddef>
#include <cmath>
#include <vector>
#include <thread>

struct SheldonPrime
{
	SheldonPrime() {}
	SheldonPrime(size_t e, size_t p, size_t rE, size_t rP) : enumeration(e), prime(p), revEnumeration(rE), revPrime(rP) {}
	
	size_t enumeration;
	size_t prime;
	
	size_t revEnumeration;
	size_t revPrime;
	
	void display() const
	{
		std::cout << "Index of prime number: 					" << enumeration << std::endl;
		std::cout << "Value of prime number: 					" << prime << std::endl;
		std::cout << "Reverse index: 						" << revEnumeration << std::endl;
		std::cout << "Value of prime number at reversed index:  		" << revPrime << std::endl;
		std::cout << "\n\n";
	}
}; 

size_t **primeLUT = NULL;
constexpr size_t PADDING = 8;

bool isPrime(size_t n) // checks if a number is prime or not
{
	size_t i = 0;
	if(n <= 1)
		return false;
	else
		for(i = 2; i * i <= n; i++)
			if(n % i == 0)
				return false;
			
	return true;
}

size_t getReverse(size_t num)
{
	return num % 10 == 0 ? num / pow(10, (size_t)log10(num)) : num > 10 ? (num % 10) * pow(10, (size_t)log10(num)) + getReverse(num / 10) : num;
}

size_t getProdDigits(size_t input)
{
	size_t prod = 1;
	while (input)
	{
		prod *= input % 10;
		input /= 10;
	}
	return prod;
}

bool isSheldonPrime(const size_t index)
{
	if (index % 11 == 0 || index < 10 || primeLUT[index][0] < 10 || getProdDigits(primeLUT[index][0]) != index) return false;
	
	size_t revIndex = getReverse(index);	
	return primeLUT[revIndex][0] == getReverse(primeLUT[index][0]);	
}

void initLUT(size_t max)
{
	for (size_t i = 2, term = 1; term < max; i++)
	{
		if (isPrime(i))
		{
			primeLUT[term][0] = i;
			++term;
		}
	}
}

int main()
{
	std::vector<SheldonPrime> sheldonPrimes;
	
	size_t max = 0, term = 0, ch = 0;
	std::cout << "Please enter the max no. of terms to search for.\n";
	std::cin >> max;
	size_t terms = max * 10;
	
	std::cout << "Use multithreading? (1/0).\n";
	std::cin >> ch;
	
	primeLUT = new size_t*[terms];
	for (int i = 0; i < terms; i++)
		primeLUT[i] = new size_t[PADDING]();
	
	
	size_t numThreads = ch ? std::thread::hardware_concurrency() : 1;
	std::thread *primeThreads = new std::thread[numThreads];
	
	
	std::cout << "Generating...\n";
	auto start = std::chrono::high_resolution_clock::now();
	
	for (uint32_t i = 0; i < numThreads; i++)
		primeThreads[i] = std::thread(initLUT, terms/numThreads);			
	for(uint32_t i = 0; i < numThreads; i++) 
		primeThreads[i].join();
	
	// initLUT(terms);
	
	for (size_t i = 0; i < max; ++i)
	{
		if (isSheldonPrime(i))
			sheldonPrimes.push_back(SheldonPrime(i, primeLUT[i][0], getReverse(i), getReverse(primeLUT[i][0])));

	}
	
	auto stop = std::chrono::high_resolution_clock::now(); 
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	
	std::cout << "Calculations finished successfully.\n";
    std::cout << "\nTotal time taken was " << diff.count() << " milliseconds.\n\n" << std::endl; 
	
	if (sheldonPrimes.empty())
		std::cout << "No Sheldon primes found in the given range.\n";
	else
	{
		std::cout << "The Sheldon primes found are:\n";
		
		for (const auto &p : sheldonPrimes)
			p.display();   
	}
	
	/*
	
	// debugging
	for (int i = 0; i < max; i++)
		std::cout << primeLUT[i][0] << std::endl;
	
	*/

	delete[] primeLUT;
	delete[] primeThreads;
}