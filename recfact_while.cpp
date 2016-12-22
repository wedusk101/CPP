#include <iostream>
int recfact(int);
using namespace std;

int main()
{
	int input = 0;
	cout<<"Please enter a positive number."<<endl;
	cin>>input;
	if(input > 0)
	{
		cout<<"The factorial of "<<input<<" is "<<recfact(input)<<"."<<endl;
	}
	else
	{
		cout<<"The factorial of "<<input<<" is "<<1<<"."<<endl;
	}	
	return 0;
}

int recfact(int n)
{
	int fact = 1;
	while(n > 0)
	{	
		fact = n * recfact(n-1);
		return fact;
	}
	return fact;
}
