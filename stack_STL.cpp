#include <iostream>
#include <stack>

int main()
{
	std::stack<int> s;
	int val = 0;
	while(std::cin >> val)
		s.push(val);
	std::cout << "popping..." << std::endl;
	while(!s.empty())
	{
		std::cout << s.top() << std::endl;
		s.pop();
	}
}