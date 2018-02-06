#include <iostream>
#include <vector>
#include <stdexcept>

int main()
{
	int s = 0, input = 0;
	char ch;
	std::vector<int> val;
	std::vector<int>::iterator ptr;
	std::cout<<"Please enter as many natural numbers as you like.Enter ESC followed by Return when you're done."<<std::endl;
	while(std::cin>>input)
	{
		try
		{
			if(input < 0)
				throw std::out_of_range("Natural numbers cannot be less than zero!");
			val.push_back(input);
		}
		catch(std::out_of_range err)
		{
			std::cout<<err.what()<<std::endl;
			std::cout<<"Try again? (Y/N)"<<std::endl;
			std::cin>>ch;
			if(ch == 'N' || ch == 'n')
				break;
		}
	}
	std::cout<<"You have entered "<<val.size()<<" items."<<std::endl;
	std::cout<<"The elements entered are:"<<std::endl;
	for(ptr = val.begin(); ptr != val.end(); ptr++)
	{
		s = s + (*ptr);
		std::cout<<*ptr<<std::endl;
	}
	std::cout<<"The sum is "<<s<<"."<<std::endl;	 
	return 0;
}