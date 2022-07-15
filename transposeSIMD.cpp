/*
	This piece of code tests the performance of SIMD accelerated matrix transpose operation of a 4x4 matrix.
	
	Compile with the following flags on GCC: -std=c++11 -O3 -msse2
	
*/

#include <iostream>
#include <chrono>
#include "immintrin.h"

struct Matrix4x4
{
	float mat[4][4];
	
	Matrix4x4() = default;
	
	Matrix4x4(float m00,
		          float m01,
		          float m02,
		          float m03,
		          float m10,
		          float m11,
		          float m12,
		          float m13,
		          float m20,
		          float m21,
		          float m22,
		          float m23,
		          float m30,
		          float m31,
		          float m32,
		          float m33)
		{
			mat[0][0] = m00;
			mat[0][1] = m01;
			mat[0][2] = m02;
			mat[0][3] = m03;
			mat[1][0] = m10;
			mat[1][1] = m11;
			mat[1][2] = m12;
			mat[1][3] = m13;
			mat[2][0] = m20;
			mat[2][1] = m21;
			mat[2][2] = m22;
			mat[2][3] = m23;
			mat[3][0] = m30;
			mat[3][1] = m31;
			mat[3][2] = m32;
			mat[3][3] = m33;
		}
		
	void transpose()
	{
		Matrix4x4 out(mat[0][0], mat[1][0], mat[2][0], mat[3][0],
					  mat[0][1], mat[1][1], mat[2][1], mat[3][1],
					  mat[0][2], mat[1][2], mat[2][2], mat[3][2],
					  mat[0][3], mat[1][3], mat[2][3], mat[3][3]);
	}
};

void fastTranspose(const Matrix4x4& input)
{
	const float* r0 = &input.mat[0][0];
	const float* r1 = &input.mat[1][0];
	const float* r2 = &input.mat[2][0];
	const float* r3 = &input.mat[3][0];

	__m128 row0 = _mm_loadu_ps(r0);
	__m128 row1 = _mm_loadu_ps(r1);
	__m128 row2 = _mm_loadu_ps(r2);
	__m128 row3 = _mm_loadu_ps(r3);

	_MM_TRANSPOSE4_PS(row0, row1, row2, row3);	

	Matrix4x4 out;

	_mm_storeu_ps(&out.mat[0][0], row0);
	_mm_storeu_ps(&out.mat[1][0], row1);
	_mm_storeu_ps(&out.mat[2][0], row2);
	_mm_storeu_ps(&out.mat[3][0], row3);
}

int main(int argc, char** argv)
{
	Matrix4x4 mat(1.000001, 2, 3, 3.1,
					4, 1, 5.1, 7.100001,
					17.6999999, 8, 1, 9,
					8, 6.600001, 4.30000001, 1);
					
	
	if (argc < 2)
	{
		std::cerr << "Invalid number of arguments passed." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	
	std::cout << "Running normal transpose..." << std::endl;

	auto start1 = std::chrono::high_resolution_clock::now();
	
	for (int i = 0; i < atoi(argv[1]); ++i)
		mat.transpose();
	
	auto stop1 = std::chrono::high_resolution_clock::now();
	
	std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop1 - start1).count() << " milliseconds." << std::endl;
	
	
	
	std::cout << "Running SIMD transpose..." << std::endl;

	auto start2 = std::chrono::high_resolution_clock::now();
	
	for (int i = 0; i < atoi(argv[1]); ++i)
		fastTranspose(mat);	
	
	auto stop2 = std::chrono::high_resolution_clock::now();
	
	std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop2 - start2).count() << " milliseconds." << std::endl;

		
	return 0;
}

