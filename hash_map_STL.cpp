#include <iostream>
#include <string>
#include <unordered_map>

int main()
{
	std::unordered_map<std::string, std::string> map = {{"Red", "255 0 0"}, {"Green", "0 255 0"}};
	map.insert(std::pair<std::string, std::string>("Blue", "0 0 255"));
	for(const auto &i : map)
		std::cout << "Color: " << i.first << " -  RGB Value: " << i.second <<std::endl;
}