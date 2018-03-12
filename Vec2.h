//Vec2.h
#ifndef VECTOR__H_
#define VECTOR__H_

class Vec2
{
	public:
	
	double x;
	double y;
	
	
		Vec2() : x(0), y(0) {}
		Vec2(const double &x_, const double &y_) : x(x_), y(y_) {}
		
		double dot(const Vec2 &v) const;
		Vec2 add(const Vec2 &v) const;
		void display() const;
		
		double operator%(const Vec2 &v) const // dot product
		{
			return x * v.x + y * v.y;
		}
		
		Vec2 operator+(const Vec2 &v) const // add two vectors
		{
			return Vec2(x + v.x, y + v.y);
		}		
}; 
#endif