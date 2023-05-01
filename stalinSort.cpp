/*
The following code implements the Stalin sort algorithm. This algorithm takes in an unsorted list of numbers and discards all 
numbers which are out of order before returning the resultant list in linear time. Obviously, because of the very nature
of the algorithm, the sorted list will almost always be smaller than the input list.
*/

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>

int main(int argc, char *argv[])
{
	std::cout << "\nThe following program generates a list of random numbers and \n"
              << "sorts them in ascending order using the Stalin sort algorithm.\n\n";
	
	if (argc == 1)
	{
		std::cerr << "Invalid input.\n";
		std::cerr << "Usage:\n";
		std::cerr << "      compiled_binary_name <size of list of numbers to sort>\n";
		std::exit(EXIT_FAILURE);
	}
	
	size_t size = std::atoi(argv[1]);
	std::vector<uint16_t> inputList(size);	// reserve space for vector according to user supplied argument
	
	std::mt19937 generator(time(0));	
	std::generate(inputList.begin(), inputList.end(), [&generator]() {
		std::uniform_int_distribution<uint16_t> dist(0, std::numeric_limits<uint16_t>::max());
		return dist(generator);
	}); // fill up the vector using random positive integers
	
	std::cout << "Input list:" << std::endl;
	for (auto i : inputList)
		std::cout << i << " ";

	std::cout << "\n";
	
	int last = inputList[0];
	std::vector<uint16_t> outputList;
	outputList.push_back(last);
	
	for (int i = 1; i < inputList.size(); ++i) 
	{
		if (inputList[i] > last)
		{
			last = inputList[i];
			outputList.push_back(last);
		}
	}
	
	std::cout << "\nSorted list:" << std::endl;
	for (const auto i : outputList)
		std::cout << i << " ";

	std::cout << "\n";
}
