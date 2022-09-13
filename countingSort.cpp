/*

The following code implements the counting sort algorithm for a given list of positive integers.

Running the program requires passing the size of the list of numbers to sort as a command line 
argument. Please note, speedups due to parallel algorithms will only be noticeable for larger
sizes of input.

Compilation requires setting the flag "-std=c++17" to enable C++17
support along with the flag "-ltbb" to link the TBB library. Parallel generation of random numbers
requires compiler support for C++17 Standard Library parallel algorithms.

NOTE: Depending on the size of the list to be sorted, this program can consume a significant amount
of memory. Although I have set the vector to use uint16_t to conserve memory, this is still something
to keep in mind. Since the program is making a copy of the list for the serial and parallel executions,
if it runs out of memory, the program will segfault.

*/

#include <vector>
#include <algorithm>
#include <cstdint>
#include <chrono>
#include <iostream>
#include <random>
#include <execution>

std::vector<uint16_t> countingSort(const std::vector<uint16_t>& in, size_t max)
{
	std::vector<uint16_t> out;
	out.reserve(in.size());
	
	std::vector<int> freqList(max + 1);			
	for (size_t i = 0; i < max; ++i)
		freqList[i] = 0;
	
	// calc frequency
	for (size_t i = 0; i < in.size(); ++i)
		freqList[in[i]]++;
	
	// write out sorted elements
	for (size_t i = 0; i < freqList.size(); ++i)
	{
		if (freqList[i] == 0)
			continue;
		else
		{
			for (int j = 0; j < freqList[i]; ++j)
				out.push_back(i);
		}
	}
			
	return out;
}

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		std::cerr << "Invalid input.\n";
		std::cerr << "Usage:\n";
		std::cerr << "      compiled_binary_name <size of list of numbers to sort>\n";
		std::exit(EXIT_FAILURE);
	}
	
	size_t size = std::atoi(argv[1]);
	std::vector<uint16_t> input(size);
	
	std::cout << "\nGenerating the list of random numbers...\n\n";	
	std::random_device r;
	std::seed_seq seed{r()};
	std::mt19937 generator(seed);	
	std::generate(std::execution::par_unseq, input.begin(), input.end(), [&generator]() {
		std::uniform_int_distribution<size_t> dist(0, std::numeric_limits<uint16_t>::max());
		return dist(generator);
	}); // fill up the vector using random positive integers
	
	size_t max = 0;
	for (const auto& i : input)
		max = (i >= max) ? i : max;
	
	std::cout << "Executing counting sort...\n";
	auto start = std::chrono::high_resolution_clock::now();
	
	std::vector<uint16_t> out = countingSort(input, max);
	
	auto stop = std::chrono::high_resolution_clock::now();
	auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	
	std::cout << "Time taken for sort: " << diff.count() << " milliseconds.\n" <<std::endl;
	
	/*
	
	// for testing
	for (const auto& i : out)
		std::cout << i << std::endl;	
	
	*/
}	
	