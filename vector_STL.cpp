#include <iostream>
#include <vector>

using namespace std;

int main()
{
	int s = 0, input = 0;
	vector<int> val;
	vector<int>::iterator ptr;
	cout<<"Please enter as many integers as you like."<<endl;
	do{
		cin>>input;
		val.push_back(input);
		cout<<"Continue? (1 = Yes / 0 = No)"<<endl;
		cin>>input;
	}while(input != 0);
	cout<<"You have entered "<<val.size()<<" items."<<endl;
	cout<<"The elements entered are:"<<endl;
	for(ptr = val.begin(); ptr != val.end(); ptr++)
	{
		s = s + (*ptr);
		cout<<*ptr<<endl;
	}
	cout<<"The sum is "<<s<<"."<<endl;	 
	return 0;
}