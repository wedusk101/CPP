#include <iostream>
#include <string>

template <typename T>
int sign(const T &t)
{
	return t.num < 0 ? -1 : t.num > 0 ? 1 : 0 ;
}

class Number
{	
	public:
		Number(const int &n) : num(n) {}
		int num;
};

class Value : public Number
{
	public:
		Value(const int &n) : num(n) {}
		int num;
};


int main()
{
	Number n(5);
	Value v(-5);
	std::cout << sign(n) << " " << sign(v) << std::endl;
}