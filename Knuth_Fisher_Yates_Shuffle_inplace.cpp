/*The following code implements the in-place version of the Knuth-Fisher-Yates linear time shuffling algorithm.*/
#include <iostream>
#include <random>
#include <ctime>

void swap(int &, int &);
void shuffle(int*, int);

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
	std::cout<<"Enter s to shuffle the list, q to quit."<< std::endl;
	while(1)
	{
		std::cin >> ch;
		if(ch != 's') // shuffles the list every time the user enters s
			break;
		shuffle(input, size);
		for(int i = 0; i < size; i++)
			std::cout << input[i] << " ";
		std::cout << "\n";
	}
	std::cout << "Thank you." << std::endl;
	delete [] input;
}

void swap(int &a, int &b)
{
	int tmp = a;
	a = b;
	b = tmp;
}

void shuffle(int *input, int size)
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