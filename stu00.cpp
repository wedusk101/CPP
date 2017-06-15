//stu00.cpp --- Student implementation

#include <iostream>
#include "stu00.h"

using namespace std;

void Student::input()
{
	cout<<"Please enter your name."<<endl;
	getline(cin, name);
	cout<<"Please enter your age."<<endl;
	cin>>age;
	cout<<"Please enter your gender."<<endl;
	cin>>gender;
}

void Student::display()
{
	cout<<"\n\nYour details are:"<<endl;
	cout<<"-----------------"<<endl;
	cout<<"Name  :  "<<name<<endl;
	cout<<"Age   :  "<<age<<" Years"<<endl;
	cout<<"Gender:  "<<gender<<endl;
}