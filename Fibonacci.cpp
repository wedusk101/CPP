#include<iostream>

int main()
{	
	using std::cin;
	using std::cout;
	using std::endl;
	int i = 0, prev = 0, current = 1, next = 0, range = 0;
	cout<<"Enter the number of terms to display."<<endl;
	cout<<endl;
	cin>>range;
	cout<<""<<endl;
	if(range<1)
	{
		cout<<"Wrong input"<<endl;
		cout<<"Please enter an integer greater than zero."<<endl;
	}
	cout<<prev<<endl;
	cout<<current<<endl;
	for(i = 1; i < range; i++)
	{
		next = prev + current;
		prev = current;
		current = next;
		cout<<next<<endl;
	}
	return 0;
}