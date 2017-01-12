//main01.cpp -- client program

#include <iostream>
#include "bank00.h"

using namespace std;

int main()
{
	Bank user;
	user.create_account();
	cout<<"Welcome!"<<endl;
	user.authenticate();
	return 0;	
}