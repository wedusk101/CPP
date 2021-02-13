#include <chrono>
#include <iostream>
#include <cstdlib>
#include <cmath>

#include "omp.h"

struct TriangleList
{
	TriangleList(int size_) : size(size_)
	{
		vax = new float[size];
		vay = new float[size];
		vaz = new float[size];
		
		vbx = new float[size];
		vby = new float[size];
		vbz = new float[size];
		
		vcx = new float[size];
		vcy = new float[size];
		vcz = new float[size];
	}
	
	~TriangleList()
	{
		delete [] vax;
		delete [] vay;
		delete [] vaz;
		
		delete [] vbx;
		delete [] vby;
		delete [] vbz;
		
		delete [] vcx;
		delete [] vcy;
		delete [] vcz;
	}
	
	float *vax;
	float *vay;
	float *vaz;
	
	float *vbx;
	float *vby;
	float *vbz;
	
	float *vcx;
	float *vcy;
	float *vcz;
	
	int size;
};

/*

// Heron's formula
float calcArea(const Triangle &tri)
{
	float a = sqrt(pow((tri.vb.x - tri.va.x), 2) + pow((tri.vb.y - tri.va.y), 2) + pow((tri.vb.z - tri.va.z), 2));
	float b = sqrt(pow((tri.vc.x - tri.vb.x), 2) + pow((tri.vc.y - tri.vb.y), 2) + pow((tri.vc.z - tri.vb.z), 2));
	float c = sqrt(pow((tri.va.x - tri.vc.x), 2) + pow((tri.va.y - tri.vc.y), 2) + pow((tri.va.z - tri.vc.z), 2));
	float s = 0.5 * (a + b + c);
	return sqrt(s * (s - a) * (s - b) * (s - c));
}

*/

int main()
{
	const int size = 100000000;
	TriangleList triList(size);
	// srand((unsigned)time(NULL));
	
	auto start = std::chrono::high_resolution_clock::now();
	
	#pragma omp simd
	for(int i = 0; i < size; i++)
	{
		triList.vax[i] = 123.123;
		triList.vay[i] = 123.123;
		triList.vaz[i] = 123.123;
		
		triList.vbx[i] = 123.123;
		triList.vby[i] = 123.123;
		triList.vbz[i] = 123.123;
		
		triList.vcx[i] = 123.123;
		triList.vcy[i] = 123.123;
		triList.vcz[i] = 123.123;		
	}
	
	/*
	
	auto start = std::chrono::high_resolution_clock::now();

	for(int i = 0; i < size; i++)
		float result = calcArea(triList[i]);
	
	*/
	
	auto stop = std::chrono::high_resolution_clock::now(); 
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	
	std::cout << "Calculations finished successfully.\n";
    std::cout << "\nTotal time taken was " << diff.count() << " milliseconds." << std::endl; 
}