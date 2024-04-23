/*
The following piece of code tests the parallel sort algorithm offered by C++17 Standard Library.
This *requires* compiler support for the feature which is still pretty nascent as of early 2021.
I have tested it with G++ 9.3.0 on Ubuntu 20.04 LTS running under WSL2 on Windows 10 Pro x64,
build 19042 with libtbb2 (= 2020.1-2). Older versions of Ubuntu don't seem to have Intel oneAPI
TBB 2018 or greater which is required for the PSTL back-end used for the Parallel STL
implementation of GCC. 

Compilation requires setting the flag "-std=c++17" to enable C++17 support along with the
flag "-ltbb" to link the TBB library.

Running the program requires passing the size of the list of numbers to sort as a command line 
argument. Please note, speedups due to parallel algorithms will only be noticeable for larger
sizes of input.

NOTE: Depending on the size of the list to be sorted, this program can consume a significant amount
of memory. Although I have set the vector to use KEY_TYPE to conserve memory, this is still something
to keep in mind. Since the program is making a copy of the list for the serial and parallel executions,
if it runs out of memory, the program will segfault.
*/

#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <execution>
#include <random>
#include <limits>
#include <cstddef>
#include <thread>
#include <iomanip>

#define UINT8 uint8_t
#define UINT16 uint16_t
#define UINT32 uint32_t

#define KEY_TYPE UINT8

int main(int argc, char *argv[])
{
	std::cout << "\nThe following program generates a list of random numbers and sorts them.\n";
	std::cout << "It measures the time taken to sort using a single thread and then compares\n"
				 "that with the time taken when sorting using parallel algorithms in C++17.\n"
				 "Finally the achieved speedup is displayed to the user.\n\n";
	if (argc == 1)
	{
		std::cerr << "Invalid input.\n";
		std::cerr << "Usage:\n";
		std::cerr << "      compiled_binary_name <size of list of numbers to sort>\n";
		std::exit(EXIT_FAILURE);
	}
	
	size_t size = std::atoi(argv[1]);
	std::vector<KEY_TYPE> vecSerial(size);	// reserve space for vector according to user supplied argument
	
	
	std::cout << "Number of logical processors detected: " << std::thread::hardware_concurrency() << std::endl;
	std::cout << "\nGenerating the list of random numbers...\n\n";
	
	std::mt19937 generator;	
	std::generate(std::execution::par_unseq, vecSerial.begin(), vecSerial.end(), [&generator]() {
		std::uniform_int_distribution<KEY_TYPE> dist(0, std::numeric_limits<KEY_TYPE>::max());
		return dist(generator);
	}); // fill up the vector using random positive integers
	
	std::vector<KEY_TYPE> vecParallel(vecSerial);	// make a copy of the vector for the parallel pass
	
	std::cout << "Executing serial sort...\n";
	auto start = std::chrono::high_resolution_clock::now();
	std::sort(vecSerial.begin(), vecSerial.end()); // sort the vector serially
	auto stop = std::chrono::high_resolution_clock::now();
	auto serial = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	
	std::cout << "Time taken for serial sort: " << serial.count() << " milliseconds.\n" << std::endl;	
	
	std::cout << "Executing parallel sort...\n";
	start = std::chrono::high_resolution_clock::now();
	std::sort(std::execution::par_unseq, vecParallel.begin(), vecParallel.end()); // sort the vector in parallel
	stop = std::chrono::high_resolution_clock::now();
	auto parallel = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	
	std::cout << "Time taken for parallel sort: " << parallel.count() << " milliseconds.\n" <<std::endl;
	std::cout << std::setprecision(4) << "Speedup factor: " << (double)serial.count() / (double)parallel.count() << "x\n";
}