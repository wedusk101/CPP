#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <climits>

struct Dictionary
{
	std::string key;
	int value;
};

bool sortByKey(const Dictionary &a, const Dictionary &b)
{
	return a.value <= b. value;
}

int main()
{
	Dictionary pair;
	std::vector<Dictionary> list;
	while(getline(std::cin, pair.key) && std::cin >> pair.value)
	{
		std::cin.clear();
		std::cin.ignore(INT_MAX,'\n');
		list.push_back(pair);
	}
	for(auto i : list)
		std::cout << "Key: " << i.key << " Value: " << i.value << std::endl;
	std::sort(list.begin(), list.end(), sortByKey);
	std::cout << "Sorted result is: " << std::endl;
	for(auto i : list)
		std::cout << "Key: " << i.key << " Value: " << i.value << std::endl;
	return 0;
}