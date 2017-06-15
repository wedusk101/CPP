//stu00.h --- Student interface

#ifndef STU00_H_
#define STU00_H_

class Student
{
	private:
		std::string name;
		int age;
		std::string gender;
		
	public:
		void input();
		void display();	
};

#endif