//Virtual
#include <iostream>

#ifndef VIRTUAL__H_
#define VIRTUAL__H_

class Animal
{
	public:
	virtual void call()
	{
		std::cout << "I'm generic calling." << std::endl;
	}	
};

class Cat : public Animal
{
	public:
	void call()
	{
		std::cout << "Meow!" << std::endl;
	}
};
#endif