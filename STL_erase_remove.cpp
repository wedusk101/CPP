#include <iostream>
#include <algorithm>
#include <random>

int main()
{
	std::random_device r;
	std::seed_seq seed{r()};
	std::mt19937 engine(seed); // uses the seed sequence from above
	std::vector<int> numList(10);
	std::generate(numList.begin(), numList.end(), [&engine]() 
	{
		std::uniform_int_distribution<uint16_t> dist(0, 100);
		return dist(engine);
	}); // we generate 10 random integers between 0 and 100
	
	std::cout << "Original list:" << std::endl;
	for (auto i : numList)
		std::cout << i << " ";

	std::cout << "\n";
	
	// Erase-remove idiom using STL algorithms
	numList.erase(std::remove_if(numList.begin(),
								 numList.end(),
								 [](int num)
								 {
									 return (num % 2 == 0);
								 }), numList.end()); // we remove all even numbers
	
	std::cout << "Modified list:" << std::endl;
	for (auto i : numList)
		std::cout << i << " ";

	std::cout << "\n";
}