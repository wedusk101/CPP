#include <iostream>
#include <vector>
#include <string>
#include <climits>

struct Data
{
	std::string description;
	int item;
};

int main()
{
	bool flag = false;
	int pos = 0;
	std::string key;
	Data obj;
	std::vector<Data> list;
	
	std::cout << "Please enter the name of each item followed by the number of such item." << std::endl;
	while(getline(std::cin, obj.description) && std::cin>>obj.item)
	{
		std::cin.clear();
		std::cin.ignore(INT_MAX, '\n');
		list.push_back(obj);
	}
	
	std::cout<<"Enter the description of the item you want to delete."<<std::endl;
	std::cin.clear();
	// std::cin.ignore(INT_MAX, '\n');
	getline(std::cin, key);
	
	// std::cout << key << std::endl;
	
	for(auto start = list.begin() ; start != list.end() ; ++start)
	{
		if((*start).description == key)
		{
			flag = true;
			list.erase(list.begin() + pos);			
		}
		++pos;
	}
	if(flag == false)
		std::cout <<"Item not found!"<<std::endl;
	for(auto start = list.begin() ; start != list.end() ; ++start)
		std::cout << "Item Name: " << (*start).description << " Count: " << (*start).item <<  std::endl;
}