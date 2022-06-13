/*The following code generates Mersenne Prime numbers using trial division method. 
The user inputs the number of Mersenne Primes to display.A Mersenne prime is of 
the form M = (2^N) - 1 where M is a prime number and N is a positive integer and 
is prime. Please note that due to the very nature of Mersenne Primes, which are 
exceedingly rare, this program is very slow for anything beyond very small input 
sizes.Handling large numbers like Mersenne Primes require special methods not 
implemented here. The prime generator used here can be replaced by something like
the Lucas_lehmer test for greater efficiency. This implementation uses multithreading
for better performance.*/

#include <iostream>
#include <cmath>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>
#include <mutex>

std::vector<size_t> mersennes;
std::mutex vecMutex;

bool isMersenne(size_t m) 
{
	size_t var = 0, exp = 1, base = 2;
    while(var <= (m + 1)) // M = (2^exp) - 1 => 2<<exp = (M + 1)
    {
        var = base << exp;
        if(var == (m + 1)) // follows from the definition of a Mersenne prime
			return true;
        exp++;
    }
    return false;
}

bool isPrime(size_t n) 
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

void generateMersennePrimes(size_t start, size_t range)
{
	for (size_t i = start; i < range; i++)
	{
		if(isPrime(i) && isMersenne(i))
		{
			std::unique_lock<std::mutex> lock(vecMutex);
			mersennes.push_back(i);
		}
	}
}


int main()
{
	size_t max, ch = 0;
	std::cout << "Please enter the max number till which to search. e.g. If you enter 1000, the program will search for Mersenne primes till 1000.\n";
	std::cin >> max; 
	
	std::cout << "Use multithreading? (1/0).\n";
	std::cin >> ch;
	
	size_t numThreads = ch ? std::thread::hardware_concurrency() : 1;
	std::thread *primeThreads = new std::thread[numThreads];
	
	std::cout << "Generating...\n";
	auto start = std::chrono::high_resolution_clock::now();	
	
	size_t stride = max / numThreads;
	size_t low = 3, high = stride;
	for (uint32_t i = 0; i < numThreads; i++)
	{		
		primeThreads[i] = std::thread(generateMersennePrimes, low, high);
		low = high, high += stride;
	}		
	for(uint32_t i = 0; i < numThreads; i++) 
		primeThreads[i].join();
	
	auto stop = std::chrono::high_resolution_clock::now(); 
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	
	std::cout << "Calculations finished successfully.\n";
    std::cout << "\nTotal time taken was " << diff.count() << " milliseconds." << std::endl; 	
	
	std::sort(mersennes.begin(), mersennes.end());
	std::cout << "The list is:\n";
	for (auto i : mersennes)
		std::cout << i << std::endl;
	
	delete[] primeThreads;
} // end

