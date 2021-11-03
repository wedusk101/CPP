/*The following code implements the sleep sort algorithm to sort a list of integers in ascending order.*/
#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm>

void sleepSort(int *, int);
void printElement(int);

int minVal = 0;

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
	std::cout<<"Beginning sleep sort of the list..."<< std::endl;
	auto start = std::chrono::high_resolution_clock::now();
	std::cout << "The sorted list is:" << std::endl;
	sleepSort(input, size);
	std::cout << "\n";
	auto stop = std::chrono::high_resolution_clock::now();
	auto diff = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
	std::cout << "\nTime taken is " << diff.count() << " seconds." << std::endl;
	std::cout << "Thank you." << std::endl;
	delete [] input;
}

void sleepSort(int *input, int size) // for each element, a new thread is created and put to sleep for the duration equivalent to the value of each element
{
	std::thread sortThreads[size];
	for(int i = 0; i < size; i++)
		minVal = std::min(input[i], minVal);
	minVal = std::abs(minVal); // for handling negative integers - used to find the absolute value of the least negative integer (if any) 
	for(int i = 0; i < size; i++)
		sortThreads[i] = std::thread(printElement, input[i] + minVal);	
	for(int i = 0; i < size; i++)
		sortThreads[i].join();
}	

void printElement(int val)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(val));
	std::cout << val - minVal << " ";
}