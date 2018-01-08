//Vector.h
#ifndef VECTOR_H_
#define VECTOR_H_

class Vector
{
	double x, y, z;
	
	private:
		void setCompX(double x);
		void setCompY(double y);
		void setCompZ(double z);
	
	public:
		Vector();
		Vector(double x, double y, double z);
		
		void initVector(double x, double y, double z);
		
		double getCompX() const { return x; }
		double getCompY() const { return y; }
		double getCompZ() const { return z; }
		double magnitude() const;
		double dotProduct(Vector v) const;
		void display() const;
};
#endif