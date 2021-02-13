#include <chrono>
#include <iostream>
#include <cstdlib>
#include <cmath>

struct Vertex
{
	float x;
	float y;
	float z;
};

struct Triangle
{
	Vertex va; // e1 = va->vb
	Vertex vb; // e2 = vb->vc
	Vertex vc; // e3 = vc->va
};

// Heron's formula
float calcArea(const Triangle &tri)
{
	float a = sqrt(pow((tri.vb.x - tri.va.x), 2) + pow((tri.vb.y - tri.va.y), 2) + pow((tri.vb.z - tri.va.z), 2));
	float b = sqrt(pow((tri.vc.x - tri.vb.x), 2) + pow((tri.vc.y - tri.vb.y), 2) + pow((tri.vc.z - tri.vb.z), 2));
	float c = sqrt(pow((tri.va.x - tri.vc.x), 2) + pow((tri.va.y - tri.vc.y), 2) + pow((tri.va.z - tri.vc.z), 2));
	float s = 0.5 * (a + b + c);
	return sqrt(s * (s - a) * (s - b) * (s - c));
}

int main()
{
	const int size = 100000000;
	Triangle *triList = new Triangle[size];
	// srand((unsigned)time(NULL));
	
	auto start = std::chrono::high_resolution_clock::now();
	
	for(int i = 0; i < size; i++)
	{
		triList[i].va.x = 123.123;
		triList[i].va.y = 123.123;
		triList[i].va.z = 123.123;
		
		triList[i].vb.x = 123.123;
		triList[i].vb.y = 123.123;
		triList[i].vb.z = 123.123;
		
		triList[i].vc.x = 123.123;
		triList[i].vc.y = 123.123;
		triList[i].vc.z = 123.123;		
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
	
	delete [] triList;
}