/*The following code implements the Knuth-Fisher-Yates linear time shuffling algorithm.*/
#include <iostream>
#include <random>
#include <ctime>

void swap(int &, int &);
int* shuffle(int*, int);

int main()
{
	int size = 0;
	std::cout << "Enter the size of the list." << std::endl;
	std::cin >> size;
	int *input = new int[size];
	std::cout << "Enter the numbers." << std::endl;
	for(int i = 0; i < size; i++)
		std::cin >> input[i];
	int *result = shuffle(input, size);
    for(int i = 0; i < size; i++)
		std::cout << result[i] << " ";
	std::cout << "\n";
	delete [] input;
	delete [] result;
}

void swap(int &a, int &b)
{
	int tmp = a;
	a = b;
	b = tmp;
}

int* shuffle(int *input, int size)
{
	int range = size - 1, i = 0, j = size - 1, index = 0;
	int *output = new int[size];
	std::mt19937 rand(time(0));
	while(i != size)
	{
		std::uniform_int_distribution<int> roll(0, range);
		index = roll(rand);
		output[i] = input[index];
		swap(input[index], input[j]);
		i++;
		range--;
		j--;
	}
	return output;
}