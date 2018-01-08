//time01.cpp ---- The following code creates three objects of class Time and adds them using operator overloading
#include <iostream>
#include "time00.h"

using namespace std;

int main()
{
	int h = 0, m = 0, s = 0;
	cout<<"Please enter the time as hh:mm:ss."<<endl;
	cout<<"Hours:"<<endl;
	cin>>h;
	cout<<"Minutes:"<<endl;
	cin>>m;
	cout<<"Seconds:"<<endl;
	cin>>s;
	Time t1(h, m, s);
	t1.display();
	cout<<"First time object created successfully. Please enter the second set of details."<<endl;
	cout<<"Hours:"<<endl;
	cin>>h;
	cout<<"Minutes:"<<endl;
	cin>>m;
	cout<<"Seconds:"<<endl;
	cin>>s;
	Time t2(h, m, s);
	t2.display();
	cout<<"Second time object created successfully. Please enter the third and final set of details."<<endl;
	cout<<"Hours:"<<endl;
	cin>>h;
	cout<<"Minutes:"<<endl;
	cin>>m;
	cout<<"Seconds:"<<endl;
	cin>>s;
	Time t3(h, m, s);
	t3.display();
	Time result = t1 + t2 + t3;
	cout<<"Total time:"<<endl;
	result.display();
	return 0;
}