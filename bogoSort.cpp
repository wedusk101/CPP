/*The following code implements the exceedingly inefficient bogo sort (also known as slow sort or
stupid sort) algorithm to sort a list of integers in ascending order. It has a factorial class
worst-case complexity. Since this is the non-deterministic version of the algorithm, there is a
chance that it may never finish. This is not a bug.*/

#include <iostream>
#include <random>
#include <ctime>
#include <chrono>

void swap(int&, int&);
void shuffle(int*, int);
void bogoSort(int*, int);
bool isSorted(int*, int);

int main()
{
	int size = 0;
	char ch;
	std::cout << "Enter the size of the list." << std::endl;
	std::cin >> size;
	int *input = new int[size];
	std::cout << "Enter the numbers." << std::endl;
	for(int i = 0; i < size; i++)
		std::cin >> input[i];
	std::cout<<"Beginning bogo sort of the list..."<< std::endl;
	auto start = std::chrono::high_resolution_clock::now();
	bogoSort(input, size);
	std::cout << "The sorted list is:" << std::endl;
	for(int i = 0; i < size; i++)
		std::cout << input[i] << " ";		
	std::cout << "\n";	
	auto stop = std::chrono::high_resolution_clock::now();
	auto diff = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
	std::cout << "\nTime taken is " << diff.count() << " seconds." << std::endl;
	std::cout << "Thank you." << std::endl;
	delete [] input;
}

void swap(int &a, int &b)
{
	int tmp = a;
	a = b;
	b = tmp;
}

void shuffle(int *input, int size) // Knuth-Fisher-Yates shuffle
{
	int range = size - 1, i = 0, index = 0;
	std::mt19937 rand(time(0));
	while(i != size)
	{
		std::uniform_int_distribution<int> roll(0, range);
		index = roll(rand);
		swap(input[index], input[range]);
		i++;
		range--;
	}
}

void bogoSort(int *input, int size) // picks a random permutation of the list and checks if it is in sorted order
{
	while(!isSorted(input, size))
		shuffle(input, size);		
}

bool isSorted(int *input, int size)
{
	if (size == 2)
		return input[0] > input[1] ? false : true; // when there are only two elements in the list
	for(int i = 1; i < size - 1; i++)
		if(!(input[i] >= input[i - 1] && input[i] <= input[i + 1]))
				return false;
	return true;
}