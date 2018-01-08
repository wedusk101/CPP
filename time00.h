//time00.h
#ifndef TIME_00_H
#define TIME_00_H

class Time
{
	private:
		int hours, minutes, seconds;
		
	public:
		Time();
		Time(int h, int m, int s);
		Time operator+(const Time &) const;
		void display() const;
};

#endif