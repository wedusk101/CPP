#include <iostream>
#include <vector>

using namespace std;

int main()
{
	int s = 0, input = 0;
	vector<int> val;
	vector<int>::iterator ptr;
	cout<<"Please enter as many integers as you like.Enter ESC followed by Return when you're done."<<endl;
	while(cin>>input)
		val.push_back(input);
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