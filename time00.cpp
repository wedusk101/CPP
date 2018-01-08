//time00.cpp
#include <iostream>
#include "time00.h"

using namespace std;

Time::Time()
{
	hours = 0;
	minutes = 0;
	seconds = 0;
}

Time::Time(int h, int m, int s)
{
	hours = h;
	minutes = m;
	seconds = s;
}

void Time::display() const
{
	cout<<hours<<":"<<minutes<<":"<<seconds<<endl;
}

Time Time::operator+(const Time & t) const
{
	Time sum;
	sum.seconds = (seconds + t.seconds) % 60;
	sum.minutes = (seconds + t.seconds) / 60;
	sum.minutes += minutes + t.minutes;
	sum.hours = sum.minutes / 60;
	sum.minutes %= 60;
	sum.hours += hours + t.hours;
	return sum;
}