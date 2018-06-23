/*The following code generates the terms of the look-and-say sequence upto 128 digits long.
The first few terms of the sequence are :

1
11
21
1211
111221
312211
13112221
1113213211
31131211131221
13211311123113112211
*/

#include <iostream>

void lookNsay(int);
void display(int*);
void swap(int**, int**);

int main()
{
	int terms = 0;
	std::cout << "This program generates the look-and-say sequence." << std::endl;
	std::cout << "Please enter the number of terms to display." << std::endl;
	std::cin >> terms;
	std::cout << "\n\nThe sequence is: \n\n" << std::endl;
	lookNsay(terms);	
}

void display(int *arr)
{
	for(int i = 0; arr[i] != 0; i++)
		std::cout << arr[i];
	std::cout << "\n";
}

void swap(int **a, int **b)
{
	int *tmp = *a;
	*a = *b;
	*b = tmp;
}

void lookNsay(int num)
{
	int continuousSeqCount = 0, i = 0, j = 0, k = 0, currentTerm = 0, terms = 0;
	bool flag = false;
	int *arr = new int[128]();
	int *seq = new int[128]();
	arr[0] = 1;
	while(terms != num)
	{
		// swap(arr, seq);
		display(arr);
		terms++;
		i = j = k = continuousSeqCount = 0;
		while(arr[i] != 0)
		{
			currentTerm = arr[i]; 
			continuousSeqCount = 1; 
			while(arr[++i] == currentTerm) 
				++continuousSeqCount;
			seq[j++] = continuousSeqCount;
			seq[j++] = currentTerm; 			
		}
		swap(&arr, &seq);
	}
	delete [] arr;
	delete [] seq;
}