#include <iostream>
#include <fstream>
#include <string>
#include <climits>

struct Data
{
	int x;
	std::string word; 
	double val;
	
	Data() : x(0), word("Empty"), val(0.0) {}
	Data(int x_, std::string word_, double val_) : x(x_), word(word_), val(val_) {}
};

int main()
{
	Data d, load;
	std::cout << "Please enter an integer." << std::endl;
	std::cin >> d.x;
	
	std::cin.clear();
	std::cin.ignore(INT_MAX,'\n');
		
	std::cout << "Please enter a word of your choice." << std::endl;
	std::getline(std::cin, d.word);
	std::cout << "Please enter a real number." << std::endl;
	std::cin >> d.val;
	
	std::ofstream out("Data.txt");
	out << d.x << "#";
	out << d.word << "#";
	out << d.val << "#";
	
	out.close();
	std::cout << "\nData saved!" << std::endl;
	
	std::ifstream in("Data.txt");
	in >> load.x;
	in.close();	
	std::cout << "Data loaded!\n" << std::endl;
	
	std::cout << "The integer entered was " << load.x << "." << std::endl;
	// check for the # delimiter here
	std::cout << "The string entered was " << load.word << "." << std::endl;
	std::cout << "The real no. entered was " << load.val << "." << std::endl;
	std::cout << "Check!" << std::endl;
	return 0;	
}