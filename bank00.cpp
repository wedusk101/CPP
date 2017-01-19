//bank00.cpp -- Bank class implementation
//version 00

#include <iostream>
#include <cstdlib>
#include <ctime>
#include "bank00.h"

using namespace std;

void Bank::create_account()
{
	srand(time(NULL));
	cout<<"Please enter your name."<<endl;
	getline(cin, name);
	cout<<"Please enter the amount you would like to deposit."<<endl;
	cin>>balance;
	account_no = rand();
	cout<<"Please enter a four digit number for your PIN."<<endl;
	cin>>pin;
}

void Bank::change_pin()
{
	int new_pin = 0;
	cout<<"Please enter a four digit number as your PIN."<<endl;
	cin>>new_pin;
	if(new_pin == pin)
	{
		cout<<"New PIN cannot be same as old PIN."<<endl;
		cout<<"Please enter a different PIN."<<endl;
		cin>>new_pin;
		pin = new_pin;
	}
	else
		pin = new_pin;
}

void Bank::withdraw()
{
	double amount = 0.0;
	cout<<"Please enter the amount."<<endl;
	cin>>amount;
	if(amount > balance)
		cout<<"Withdrawal amount cannot be greater than the available balance."<<endl;
	else
		balance -= amount;
	cout<<"Available balance is $"<<balance<<endl;
	cout<<"Thank you."<<endl;
}

void Bank::display_balance()
{
	cout<<"Account Number: "<<account_no<<endl;
	cout<<"Balance $"<<balance<<endl;
	cout<<"Thank you."<<endl;
}

void Bank::authenticate()
{
	int input_pin = 0, choice = 0;
	cout<<"Please enter your PIN."<<endl;
	cin>>input_pin;
	if(input_pin == pin)
	{
		cout<<"Hello "<<name<<endl;
		cout<<"Enter 1 to change pin."<<endl;
		cout<<"Enter 2 to withdraw cash."<<endl;
		cout<<"Enter 3 to check balance."<<endl;
		cout<<"Enter 4 to quit."<<endl;
		cin>>choice;
		switch(choice)
		{
			case 1:
				change_pin();				
				break;
			
			case 2:
				withdraw();
				break;
			
			case 3:
				display_balance();
				break;
				
			default:
				exit(EXIT_SUCCESS);						
		}
	}
	else
	{
		cout<<"Incorrect pin."<<endl;
		exit(EXIT_SUCCESS);
	}	
}
