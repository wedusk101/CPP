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

void lookNsay(int*, int);
void display(int*);

int main()
{
	int terms = 0;
	std::cout << "This program generates the look-and-say sequence." << std::endl;
	std::cout << "Please enter the number of terms to display." << std::endl;
	std::cin >> terms;
	int seq[128] = {0};
	// seq[127] = -1;
	// display(seq);
	lookNsay(seq, terms);	
}

void display(int *arr)
{
	for(int i = 0; arr[i] != -1; i++)
		std::cout << arr[i];
	std::cout << "\n";
}

void lookNsay(int *arr, int num)
{
	int continuousSeqCount = 0, i = 0, j = 0, currentTerm = 0, terms = 0;
	arr[0] = 1;
	arr[1] = -1;
	while(terms != num)
	{
		// std::cout << " Here" <<"\n";
		display(arr);
		terms++;
		i = j = continuousSeqCount = 0;
		while(arr[i] != 0)
		{
			// display(arr);
			currentTerm = arr[i]; // infinite loop as arr[1] is initially -1
			continuousSeqCount = 1; 
			std::cout << " Current " << currentTerm <<"\n";
			while(arr[++i] == currentTerm) // currentTerm is -1 here - infinite loop
				++continuousSeqCount;
			arr[j++] = continuousSeqCount;
			arr[j++] = currentTerm;
			arr[j] = -1;
			// std::cout << " Here 59 i j " << i << " " << j <<"\n";
		}
		// std::cout << " Here 66" <<"\n";		
	}
}