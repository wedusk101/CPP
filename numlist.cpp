#include <iostream>
using namespace std;

int main()
{
	int numlist[5] = {0}, i = 0;
	cout<<"Please enter five integers."<<endl;
	for(i = 0;i < 5;i++)
	{
		cin>>numlist[i];
	}
	cout<<"\n";
	cout<<"Your list is:"<<endl;
	for(i = 0;i < 5;i++)
	{
		cout<<numlist[i]<<endl;
	}
	return 0;
}
