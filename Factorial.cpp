#include <iostream>
int factorial(int);

int main()
{
	using std::cin;
	using std::cout;
	using std::endl;
	int num;
	cout<<"Enter a number."<<endl;
	cin>>num;
	cout<<"The factorial of "<<num<<" is "<<factorial(num)<<"."<<endl;
	return 0;
}

int factorial(int n)
{
	int fact=1,i=1;
	for(i=1;i<=n;i++)
	{
		fact=fact*i;
	}
	return fact;
}
