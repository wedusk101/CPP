#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

int main()
{
	int i = 0, n = 0, j = 0, k = 0, tmp = 0;
	char ch;
	srand(time(NULL)); // initalizes seed value 
	cout<<"How many numbers would you like to have on your list?"<<endl;
	cin>>n;
	int *list = new int [n];
	cout<<"Please enter the integers."<<endl;
	for(i = 0; i < n; i++)
	{
		cin>>list[i];
 	}
	cout<<"The list you have entered is ";
	for(i = 0; i < n; i++)
	{
		cout<<list[i]<<" ";
 	}
	cout<<".\n";
	cout<<"Enter s to shuffle the list."<<endl;
	while(1)
	{
		cin>>ch;
		if(ch != 's') // shuffles the list every time the user enters s
			break;
		for(k = 0; k < n; k++) // shuffles the list n times
		{			
			i = rand() % n; // selects two random positions
			j = rand() % n; 
			tmp = list[i];
			list[i] = list[j];
			list[j] = tmp;
		}
		cout<<"The shuffled list is ";
		for(i = 0; i < n; i++)
		{
			cout<<list[i]<<" ";
		}
		cout<<".\n";
		cout<<"Enter s to shuffle again. Enter q to quit."<<endl;		
	}
	cout<<"Thank you."<<endl;
	delete list;
	return 0;	 
} // end