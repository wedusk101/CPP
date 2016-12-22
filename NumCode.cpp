#include<iostream>
int numReverse(int);

using namespace std;

int main()
{
	int input = 0, digit = 0, number = 0;
	cout<<"Enter a number."<<endl;
	cin>>input;
	number = numReverse(input);
	while(number != 0)
	{
		digit = number % 10;
		number = number / 10;
		if(digit % 2 == 0)
		{
			cout<<1;
		}
		else
		{
			cout<<0;
		}
	}
cout<<endl;
return 0;
}

int numReverse(int num)
{
	int x = 0, revSum = 0;
	while(num != 0)
	{
		x = num % 10;
		num = num / 10;
		revSum = (revSum * 10) + x;
	}
return revSum;
}
