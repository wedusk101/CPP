#include <iostream>

struct Grades
{
	int math;
	int physics;
	int cs;
	int total;
};

void sortReport(Grades*, const int&);

int main()
{
	int i = 0, math = 0, physics = 0, cs = 0;
	const int size = 3;
	
	Grades *list = new Grades[size]; // allocate memory
	
	for(i = 0; i < size; ++i)
	{
		std::cout << "Please enter Math marks: " << std::endl;
		std::cin >> math;
		std::cout << "Please enter Physics marks: " << std::endl;
		std::cin >> physics;
		std::cout << "Please enter CS marks: " << std::endl;
		std::cin >> cs;
		
		list[i].math = math;
		list[i].physics = physics;
		list[i].cs = cs;
		list[i].total = math + physics + cs;
	}
	sortReport(list, size);
	
	std::cout << "The ranked reports are: " << std::endl; // display results
	for(i = 0; i < size; i++)
	{
		std::cout << "Rank   : "<< i + 1 << std::endl;
		std::cout << "Math   : "<< list[i].math << std::endl;
		std::cout << "Physics: "<< list[i].physics << std::endl;
		std::cout << "CompSci: "<< list[i].cs << std::endl;
		std::cout << "Total  : "<< list[i].total << std::endl;
	}
	
	delete [] list; // cleanup
}

void sortReport(Grades *record, const int &size) // selection sort
{
	int i = 0, j = 0, max = 0;
	Grades tmp;
	for(i = 0; i < size - 1; i++)
	{
		max = i;
		for (j = i + 1; j < size; j++)
			if(record[j].total > record[max].total)
				max = j;
		tmp = record[i];
		record[i] = record[max];
		record[max] = tmp;
	}
}	