//bank00.h -- Bank class interface
//version 00

#ifndef BANK00_H_
#define BANK00_H_

class Bank
{
	private:
		std::string name;
		long account_no;
		double balance;
		int pin;
	
	public:
		void create_account();
		void authenticate();
		void change_pin();
		void withdraw();
		void display_balance();
};

#endif