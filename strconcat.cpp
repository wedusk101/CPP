#include <iostream>
#include <string>

using namespace std;

string join(const string &, const string &);

int main()
{
	string first, second;
	cout<<"Please enter the first string."<<endl;
	getline(cin, first);
	cout<<"Please enter the second string."<<endl;
	getline(cin, second);
	cout<<"The joined string is: "<<join(first, second)<<endl;
	return 0;	
}

string join(const string & first, const string & second)
{
	return first + ", " + second; // first = "Hello"  second = "World!"
}