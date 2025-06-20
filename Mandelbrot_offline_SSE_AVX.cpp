/*

The following program renders the Mandelbrot set. This set consists of a set of complex numbers with
a specific property. A complex number Z is a number that has two parts; one real, and one imaginary.
It is usually written in the form Z = a + bi, where a is the real part, b is the imaginary part and
i is the square root of -1. The Mandelbrot set consists of all such complex numbers C, such that
Z = Z^2 + C [ where C is a given complex number ] and Z is bounded under iteration with Z (a complex
number itself) starting from 0 (every real number is a complex number with the imaginary part being zero).

Therefore,

		Z_n+1 = Z^2 + C [ let C = a + bi ]
		
		=> Z_0 = 0 + (a + bi) = a + bi

		=> Z_1 = Z_0^2 + C = (a + bi)^2 + (a + bi) = (a^2 - b^2 + 2abi) + (a + bi) = (a^2 - b^2 + a) + i(2ab + b)

		=> Z_2 = Z_1^2 + C and so on ...


The modulus of a complex number Z = (a + bi) is defined as sqrt(a^2 + b^2). This is geometrically the
Euclidean distance between the origin of the complex plane and Z. If this distance is always less than
some value for a given complex number C , under the above iteration, then C is set to be bounded. If it
is bounded within the Mandelbrot set bounds, then the given complex number C is said to be a part of
the Mandelbrot set.

The Mandelbrot set is what is known as a fractal pattern. It has repeating patterns visible upon zooming
and is self-similar. It is named after the mathematician Benoit Mandelbrot. It is the most well-known
fractal. Computing the Mandelbrot used to be a very compute intensive task to perform once. Nowadays, it
can be trivially computed by even processors in smart watches. There are multithreaded implementations 
provided here using OpenMP and C++ Standard Library threads. There are also both single and multithreaded
implementations using SIMD processing for significantly better performance. This is an offline variant of
this algorithm. The multithreading should show speed ups at very high resolutions only. Otherwise, the
overhead of creating the threads might be too high. The bottleneck in this program is the disk access for
writing out the rendered buffer to file, especially at lower resolutions. It is recommended to use the
application in benchmark mode to disable file I/O for performance measurement.

NOTE: 
----

[1] Running the application at high enough resolutions can be memory intensive. 

[2] To compile the program make sure to set the appropriate compiler flags to generate vector code for your 
processor specific architecture The SIMD instructions used here support both SSE and AVX, depending on
the platform of choice. Since AVX uses 8 wide registers whereas SSE uses 4 wide ones, the AVX code
should be roughly twice as fast when tested on the same system. 

[3] If this program is being run on a system with more than 64 logical cores under WSL please
see https://github.com/sirredbeard/WSL2-more-cores. If this program is run under Windows on 
a system that has more than 64 logial cores, special changes need to made concering processor
groups. Such features have not been implemented yet.

COMPILATION DETAILS:
-------------------

I have tested on GCC 9.4.0 with the following flags: 

SSE: "-fopenmp -pthread -O3 -std=c++17 -msse4.1 -DISA_SSE" 
----------------------------------------------------------

to enable and/or link OpenMP, pthreads, full optimizations, C++17,
SSE4.1 instruction set respectively.

AVX: "-fopenmp -pthread -O3 -std=c++17 -mavx2 -mfma -DISA_AVX" 
--------------------------------------------------------------

to enable and/or link OpenMP, pthreads, full optimizations, C++17,
AVX2 instruction set and Fused Multiply Add respectively.

AVX-512: "-fopenmp -pthread -O3 -std=c++17 -mavx512f -mfma -DISA_AVX512"
-----------------------------------------------------------------------

to enable and/or link OpenMP, pthreads, full optimizations, C++17,
AVX-512F instruction set and Fused Multiply Add respectively.

The compiler definitions ISA_SSE, ISA_AVX, ISA_AVX512 enable/disable platform specific intrinsics code.
Note that the SSE code will run on a system that supports AVX but the AVX code will *not* run
on a system that *only* supports upto SSE. Also, SSE and AVX *cannot* be enabled at the same time.
This is by design. In case one needs to test out both the implementations, two binaries need to be
compiled for SSE and AVX using the flags -DISA_SSE and -DISA_AVX respectively. Same applies for
AVX-512 instructions.

TODO: The AVX implementation has slight rendering differences compared to the non-SIMD OpenMP and
the single core versions. This is probably due to subtle precision errors in the AVX implementation.
Needs further investigation for a fix.

To know more about the Mandelbrot set, please refer to https://en.wikipedia.org/wiki/Mandelbrot_set

Resources on SSE/AVX intrinsics:
-------------------------------

[1]  https://www.cs.virginia.edu/~cr4bd/3330/S2018/AVXref.html
[2]  https://docs.microsoft.com/en-us/cpp/intrinsics/compiler-intrinsics?view=msvc-160
[3]  https://gcc.gnu.org/onlinedocs/gcc-4.9.2/gcc/X86-Built-in-Functions.html
[4]  https://software.intel.com/sites/landingpage/IntrinsicsGuide/#
[5]  https://www.linuxjournal.com/content/introduction-gcc-compiler-intrinsics-vector-processing
[6]  https://www.youtube.com/watch?v=x9Scb5Mku1g
[7]  https://www.youtube.com/watch?v=QghC6G8TyQ0
[8]  https://www.codingame.com/playgrounds/283/sse-avx-vectorization/what-is-sse-and-avx
[9]  https://software.intel.com/content/www/us/en/develop/articles/introduction-to-intel-advanced-vector-extensions.html
[10] https://www.cs.virginia.edu/~cr4bd/3330/S2018/simdref.html
[11] https://en.wikichip.org/wiki/x86/avx-512

*/

#include <iostream>
#include <fstream>
#include <cmath>
#include <chrono>
#include <thread>
#include <omp.h>
#include <immintrin.h>

struct Color3f
{
	float r; // red 0-1
	float g; // green 0-1
	float b; // blue 0-1
	
	Color3f() : r(0), g(0), b(0) {}
	Color3f(const float &r_, const float &g_, const float &b_) : r(r_), g(g_), b(b_) {}

	inline Color3f operator&(const Color3f &col) const // color modulate
	{
		return Color3f(r * col.r, g * col.g, b * col.b);
	}

	inline Color3f operator*(const float &c) const // scalar multiplication
	{
		return Color3f(c * r, c * g, c * b);
	}

	inline Color3f operator+(const Color3f &c) const // addition
	{
		return Color3f(r + c.r, g + c.g, b + c.b);
	}

	inline Color3f operator/(const float &c) const // scalar division
	{
		float inv = 1. / c;
		return Color3f(r * inv, g * inv, b * inv);
	}

	inline Color3f& operator*=(const float &c) // scalar division
	{
		r *= c;
		g *= c;
		b *= c;
		return *this;
	}

	inline Color3f& operator/=(const float &c) // scalar division
	{
		float inv = 1. / c;
		r *= inv;
		g *= inv;
		b *= inv;
		return *this;
	}

	inline Color3f& operator+=(const Color3f &c) // addition
	{
		r += c.r;
		g += c.g;
		b += c.b;
		return *this;
	}

	void clamp()
	{
		r = (r > 1) ? 1 : (r < 0) ? 0 : r;
		g = (g > 1) ? 1 : (g < 0) ? 0 : g;
		b = (b > 1) ? 1 : (b < 0) ? 0 : b;
	}
};

struct Complex // class to support Complex numbers
{
	float a;
	float b;

	Complex() : a(0), b(0) {}
	Complex(float a_, float b_) : a(a_), b(b_) {}

	float real() const
	{
		return a;
	}

	float imaginary() const
	{
		return b;
	}

	float getModulus()
	{
		return sqrt(a * a + b * b);
	}
};

/*============================ COLORS ================================*/

Color3f BLACK(0, 0, 0);
Color3f WHITE(1, 1, 1);
Color3f RED(1, 0, 0);
Color3f GREEN(0, 1, 0);
Color3f BLUE(0, 0, 1);
Color3f CYAN(0, 1, 1);
Color3f MAGENTA(1, 0, 1);
Color3f YELLOW(1, 1, 0);

/*=====================================================================*/

uint32_t MAX_ITR = 0;

void saveImg(Color3f *frameBuffer, int width, int height)
{
	int bufferSize = width * height;
	std::cout << "Writing out frame buffer to file..." << std::endl;

	const char *outputPath = "Mandelbrot.ppm";

	std::ofstream out(outputPath); // creates a PPM image file for saving the rendered output
	out << "P3\n" << width << " " << height << "\n255\n";

	for (uint32_t i = 0; i < bufferSize; i++)
		out << static_cast<int>(frameBuffer[i].r * 255.99) << " " << static_cast<int>(frameBuffer[i].g * 255.99) << " " << static_cast<int>(frameBuffer[i].b * 255.99) << "\n";
}

// maps pixel values in the X direction of screen space between (-2.5, 1)
float getMappedScaleX(const int &x, const int &xMax)
{
	return ((x / (float) xMax) * 3.5) - 2.5;
}

// maps pixel values in the Y direction of screen space between (-1, 1)
float getMappedScaleY(const int &y, const int &yMax)
{
	return ((y / (float) yMax) * 2) - 1;
}

void evalMandel(Complex &z, const Complex &c)
{
	float zReal = z.a;
	float zImaginary = z.b;
	z.a = zReal * zReal - zImaginary * zImaginary + c.a;
	z.b = 2 * zReal * zImaginary + c.b;
}

/********************************************INTRINSICS BEGIN*****************************************/

/////////////////////////////////////////////// SSE BEGIN /////////////////////////////////////////////

#ifdef ISA_SSE

void drawMandelbrotOMPSSE(const int &width, const int &height, int isBenchmark, uint32_t nThreads)
{
	uint32_t bufferSize = width * height;
	Color3f *frameBuffer = new Color3f[bufferSize];
	float invW = (1./ width) * 3.5, invH = (1./ height) * 2;
#pragma omp parallel num_threads(nThreads) shared(frameBuffer)
	{
	
		// 32-bit float registers
		__m128 _zr, _zi, _cr, _ci, _a, _b, _zr2, _zi2, _const2, _invw, _invh, _const2p5,
				_const1, _mod, _const4, _maskwhile, _xf, _yf; 
		
		// 32-bit signed int registers
		__m128i _masknumitr, _itr, _constmaxitr, _inc1i, _const1i;
		
		// initialize floating point registers
		_const1 = _mm_set1_ps(1.0);
		_const2 = _mm_set1_ps(2.0);
		_const4 = _mm_set1_ps(4.0);
		_const2p5 = _mm_set1_ps(2.5);
		
		_invw = _mm_set1_ps(invW);
		_invh = _mm_set1_ps(invH);	
		
		// initialize integer registers
		_constmaxitr = _mm_set1_epi32(MAX_ITR);	
		_const1i = _mm_set1_epi32(1);	
		
#pragma omp for schedule(dynamic, 1) 
		for (int y = 0; y < height; y++) // y axis of the image	
		{
			int yw = y * width;
			_yf = _mm_set1_ps((float)y);
			
			for (int x = 0; x < width; x += 4) // x axis of the image
			{			
				_xf = _mm_setr_ps((float)x + 3, (float)x + 2, (float)x + 1, (float)x);							
				
				// int index = y * width + x;			
				int index0 = yw + x; // y * width + x
				int index1 = yw + x + 1; // y * width + (x + 1)
				int index2 = yw + x + 2; // y * width + (x + 2)
				int index3 = yw + x + 3; // y * width + (x + 3)			
				
				// int itr = 0;	
				_itr = _mm_set1_epi32(0); // initialize iteration counter for each pixel
				
				// float zr = 0, zi = 0, cr = 0, ci = 0; [ Complex z, c ]				
				_zr = _mm_set1_ps(0);
				_zi = _mm_set1_ps(0);
				_cr = _mm_set1_ps(0);
				_ci = _mm_set1_ps(0);
				
				// getMappedScaleX(const int &x, const int &xMax)
				
				// cr = (x * invW) - 2.5;			
				// _cr =  _mm_fmadd_ps(_xf, _invw, _const2p5neg); // No FMA on my Nehalem CPU			 
				_cr = _mm_mul_ps(_xf, _invw);
				_cr = _mm_sub_ps(_cr, _const2p5);	

				// getMappedScaleY(const int &y, const int &yMax)
				
				// ci = (y * invH) - 1;			
				// _ci =  _mm_fmadd_ps(_yf, _invh, _const1neg);  // No FMA on my Nehalem CPU
				_ci = _mm_mul_ps(_yf, _invh);
				_ci = _mm_sub_ps(_ci, _const1);		

				///////////////////////////// while (zr * zr + zi * zi <= 2 * 2 && itr < MAX_ITR) ///////////////////////////////
				
				loop: // while (...)
					
				_zr2 = _mm_mul_ps(_zr, _zr); // zr * zr
				_zi2 = _mm_mul_ps(_zi, _zi); // zi * zi
				_mod = _mm_add_ps(_zr2, _zi2); // zr * zr + zi * zi			
					
				_masknumitr = _mm_cmplt_epi32(_itr, _constmaxitr); // itr < MAX_ITR	
				_maskwhile = _mm_cmple_ps(_mod, _const4); // zr * zr + zi * zi <= 4.0					
				_maskwhile = _mm_and_ps(_maskwhile, _mm_castsi128_ps(_masknumitr)); // (zr * zr + zi * zi <= 4.0 && itr < MAX_ITR)
				
				
				//////////////////////// evalMandel(z, c)//////////////////////////////////
				
				_a = _zr;
				_b = _zi;
				
				_zr = _mm_sub_ps(_zr2, _zi2); // zr = zr * zr - zi * zi
				_zr = _mm_add_ps(_zr, _cr); // zr += cr
				
				_zi = _mm_mul_ps(_a, _b); // zi = a * b
				
				// zi = zi * 2 + ci
				// _zi = _mm_fmadd_ps(_zi, _const2, _ci); // No FMA on my Nehalem CPU
				_zi = _mm_mul_ps(_zi, _const2);
				_zi = _mm_add_ps(_zi, _ci);
				
				//////////////////////// evalMandel(z, c)//////////////////////////////////
				
				
				// itr++;
				_inc1i = _mm_and_si128(_mm_castps_si128(_maskwhile), _const1i);
				_itr = _mm_add_epi32(_itr, _inc1i);				
				
				// if (any one register satisfies while condition) goto loop;
				if (_mm_movemask_ps(_maskwhile) > 0)
					goto loop;
				
				///////////////////////////// while (zr * zr + zi * zi <= 2 * 2 && itr < MAX_ITR) ///////////////////////////////
				
				// if (itr < MAX_ITR) frameBuffer[index] = BLACK;
				// else frameBuffer[index] = CYAN;
				
				// pixel masks read in correct endianness |3, 2, 1, 0| instead of |0, 1, 2, 3| 
				int pixel0 = _mm_extract_epi32(_masknumitr, 3);
				int pixel1 = _mm_extract_epi32(_masknumitr, 2);
				int pixel2 = _mm_extract_epi32(_masknumitr, 1);
				int pixel3 = _mm_extract_epi32(_masknumitr, 0);
				
				frameBuffer[index0] = pixel0 ? BLACK : CYAN;
				frameBuffer[index1] = pixel1 ? BLACK : CYAN;
				frameBuffer[index2] = pixel2 ? BLACK : CYAN;
				frameBuffer[index3] = pixel3 ? BLACK : CYAN;		
			}
		}
	}
	
	if (!isBenchmark)
		saveImg(frameBuffer, width, height);
	
	delete [] frameBuffer;
}

#endif // ISA_SSE

/////////////////////////////////////////////// SSE END ///////////////////////////////////////////////



/////////////////////////////////////////////// AVX BEGIN /////////////////////////////////////////////

#ifdef ISA_AVX

void drawMandelbrotOMPAVX(const int &width, const int &height, int isBenchmark, uint32_t nThreads)
{
	uint32_t bufferSize = width * height;
	Color3f *frameBuffer = new Color3f[bufferSize];
	float invW = (1. / width) * 3.5, invH = (1. / height) * 2;	
	
#pragma omp parallel num_threads(nThreads) shared(frameBuffer)
	{	
		// 32-bit float registers
		__m256 _zr, _zi, _cr, _ci, _a, _b, _zr2, _zi2, _const2, _invw, _invh, _const2p5neg,
			_const1neg, _mod, _const4, _maskwhile, _xf, _yf;

		// 32-bit signed int registers
		__m256i _masknumitr, _itr, _constmaxitr, _inc1i, _const1i;


		// initialize floating point registers
		_const1neg = _mm256_set1_ps(-1.0);
		_const2 = _mm256_set1_ps(2.0);
		_const4 = _mm256_set1_ps(4.0);
		_const2p5neg = _mm256_set1_ps(-2.5);

		_invw = _mm256_set1_ps(invW);
		_invh = _mm256_set1_ps(invH);

		// initialize integer registers
		_constmaxitr = _mm256_set1_epi32(MAX_ITR);
		_const1i = _mm256_set1_epi32(1);	
		
#pragma omp for schedule(dynamic, 1) 
		for (int y = 0; y < height; y++) // y axis of the image	
		{
			int yw = y * width;
			_yf = _mm256_set1_ps((float) y);
			
			for (int x = 0; x < width; x += 8) // x axis of the image
			{			
				_xf = _mm256_setr_ps((float) x + 7, (float) x + 6, (float) x + 5, (float) x + 4,
				(float) x + 3, (float) x + 2, (float) x + 1, (float) x);

				// int index = y * width + x;			
				int index0 = yw + x; // y * width + x
				int index1 = yw + x + 1; // y * width + (x + 1)
				int index2 = yw + x + 2; // y * width + (x + 2)
				int index3 = yw + x + 3; // y * width + (x + 3)		
				int index4 = yw + x + 4;
				int index5 = yw + x + 5;
				int index6 = yw + x + 6;
				int index7 = yw + x + 7;

				// int itr = 0;	
				_itr = _mm256_set1_epi32(0); // initialize iteration counter for each pixel

				// float zr = 0, zi = 0, cr = 0, ci = 0; [ Complex z, c ]				
				_zr = _mm256_set1_ps(0);
				_zi = _mm256_set1_ps(0);
				_cr = _mm256_set1_ps(0);
				_ci = _mm256_set1_ps(0);

				// getMappedScaleX(const int &x, const int &xMax)

				// cr = (x * invW) - 2.5;			
				_cr = _mm256_fmadd_ps(_xf, _invw, _const2p5neg);


				// getMappedScaleY(const int &y, const int &yMax)

				// ci = (y * invH) - 1;			
				_ci = _mm256_fmadd_ps(_yf, _invh, _const1neg);


				///////////////////////////// while (zr * zr + zi * zi <= 2 * 2 && itr < MAX_ITR) ///////////////////////////////

			loop: // while (...)

				_zr2 = _mm256_mul_ps(_zr, _zr); // zr * zr
				_zi2 = _mm256_mul_ps(_zi, _zi); // zi * zi
				_mod = _mm256_add_ps(_zr2, _zi2); // zr * zr + zi * zi			

				_masknumitr = _mm256_cmpgt_epi32(_constmaxitr, _itr); // MAX_ITR > itr	
				_maskwhile = _mm256_cmp_ps(_mod, _const4, _CMP_LT_OQ); // zr * zr + zi * zi <= 4.0
				_maskwhile = _mm256_and_ps(_maskwhile, _mm256_castsi256_ps(_masknumitr)); // (zr * zr + zi * zi <= 4.0 && itr < MAX_ITR)


				//////////////////////// evalMandel(z, c)//////////////////////////////////

				_a = _zr;
				_b = _zi;

				_zr = _mm256_sub_ps(_zr2, _zi2); // zr = zr * zr - zi * zi
				_zr = _mm256_add_ps(_zr, _cr); // zr += cr

				_zi = _mm256_mul_ps(_a, _b); // zi = a * b

				// zi = zi * 2 + ci
				_zi = _mm256_fmadd_ps(_zi, _const2, _ci);

				//////////////////////// evalMandel(z, c)//////////////////////////////////


				// itr++;
				_inc1i = _mm256_and_si256(_mm256_castps_si256(_maskwhile), _const1i);
				_itr = _mm256_add_epi32(_itr, _inc1i);

				// if (any one register satisfies while condition) goto loop;
				if (_mm256_movemask_ps(_maskwhile) > 0)
					goto loop;

				///////////////////////////// while (zr * zr + zi * zi <= 2 * 2 && itr < MAX_ITR) ///////////////////////////////

				// if (itr < MAX_ITR) frameBuffer[index] = BLACK;
				// else frameBuffer[index] = CYAN;


				// pixel masks read in correct endianness |7, 6, 5, 4, 3, 2, 1, 0| instead of |0, 1, 2, 3, 4, 5, 6, 7| 			
				int pixel0 = _mm256_extract_epi32(_masknumitr, 7);
				int pixel1 = _mm256_extract_epi32(_masknumitr, 6);
				int pixel2 = _mm256_extract_epi32(_masknumitr, 5);
				int pixel3 = _mm256_extract_epi32(_masknumitr, 4);
				int pixel4 = _mm256_extract_epi32(_masknumitr, 3);
				int pixel5 = _mm256_extract_epi32(_masknumitr, 2);
				int pixel6 = _mm256_extract_epi32(_masknumitr, 1);
				int pixel7 = _mm256_extract_epi32(_masknumitr, 0);


				frameBuffer[index0] = pixel0 ? BLACK : CYAN;
				frameBuffer[index1] = pixel1 ? BLACK : CYAN;
				frameBuffer[index2] = pixel2 ? BLACK : CYAN;
				frameBuffer[index3] = pixel3 ? BLACK : CYAN;
				frameBuffer[index4] = pixel4 ? BLACK : CYAN;
				frameBuffer[index5] = pixel5 ? BLACK : CYAN;
				frameBuffer[index6] = pixel6 ? BLACK : CYAN;
				frameBuffer[index7] = pixel7 ? BLACK : CYAN;
			}
		}
	}
	
	if (!isBenchmark)
		saveImg(frameBuffer, width, height);
	
	delete [] frameBuffer;
}

#endif // ISA_AVX

/////////////////////////////////////////////// AVX END ///////////////////////////////////////////////



/////////////////////////////////////////////// AVX-512 BEGIN /////////////////////////////////////////////

#ifdef ISA_AVX512

void drawMandelbrotOMPAVX512(const int &width, const int &height, int isBenchmark, uint32_t nThreads)
{
	uint32_t bufferSize = width * height;
	Color3f *frameBuffer = new Color3f[bufferSize];
	float invW = (1. / width) * 3.5, invH = (1. / height) * 2;	
	
#pragma omp parallel num_threads(nThreads) shared(frameBuffer)
	{	
		// 32-bit float registers
		__m512 _zr, _zi, _cr, _ci, _a, _b, _zr2, _zi2, _const2, _invw, _invh, _const2p5neg,
			_const1neg, _mod, _const4, _xf, _yf;

		// 32-bit signed int registers
		__m512i _itr, _constmaxitr, _inc1i, _const1i;

		// initialize floating point registers
		_const1neg = _mm512_set1_ps(-1.0);
		_const2 = _mm512_set1_ps(2.0);
		_const4 = _mm512_set1_ps(4.0);
		_const2p5neg = _mm512_set1_ps(-2.5);

		_invw = _mm512_set1_ps(invW);
		_invh = _mm512_set1_ps(invH);

		// initialize integer registers
		_constmaxitr = _mm512_set1_epi32(MAX_ITR);
		_const1i = _mm512_set1_epi32(1);	
		
#pragma omp for schedule(dynamic, 1) 
		for (int y = 0; y < height; y++) // y axis of the image	
		{
			int yw = y * width;
			_yf = _mm512_set1_ps((float) y);
			
			for (int x = 0; x < width; x += 16) // x axis of the image
			{
				_xf = _mm512_setr_ps((float)x + 15, (float)x + 14, (float)x + 13, (float)x + 12,
				                    (float)x + 11, (float)x + 10, (float)x + 9, (float)x + 8, 
									(float)x + 7, (float)x + 6, (float)x + 5, (float)x + 4,
									(float)x + 3, (float)x + 2, (float)x + 1, (float)x);

				// int index = y * width + x;			
				int index0 = yw + x;
				int index1 = yw + x + 1;
				int index2 = yw + x + 2;
				int index3 = yw + x + 3;		
				int index4 = yw + x + 4;
				int index5 = yw + x + 5;
				int index6 = yw + x + 6;
				int index7 = yw + x + 7;
				int index8 = yw + x + 8;
				int index9 = yw + x + 9;
				int index10 = yw + x + 10;
				int index11 = yw + x + 11;
				int index12 = yw + x + 12;
				int index13 = yw + x + 13;
				int index14 = yw + x + 14;
				int index15 = yw + x + 15;

				// int itr = 0;	
				_itr = _mm512_set1_epi32(0); // initialize iteration counter for each pixel

				// float zr = 0, zi = 0, cr = 0, ci = 0; [ Complex z, c ]				
				_zr = _mm512_set1_ps(0);
				_zi = _mm512_set1_ps(0);
				_cr = _mm512_set1_ps(0);
				_ci = _mm512_set1_ps(0);

				// getMappedScaleX(const int &x, const int &xMax)

				// cr = (x * invW) - 2.5;			
				_cr = _mm512_fmadd_ps(_xf, _invw, _const2p5neg);


				// getMappedScaleY(const int &y, const int &yMax)

				// ci = (y * invH) - 1;			
				_ci = _mm512_fmadd_ps(_yf, _invh, _const1neg);


				///////////////////////////// while (zr * zr + zi * zi <= 2 * 2 && itr < MAX_ITR) ///////////////////////////////

			loop: // while (...)

				_zr2 = _mm512_mul_ps(_zr, _zr); // zr * zr
				_zi2 = _mm512_mul_ps(_zi, _zi); // zi * zi
				_mod = _mm512_add_ps(_zr2, _zi2); // zr * zr + zi * zi			

				__mmask16 _masknumitr = _mm512_cmplt_epi32_mask(_itr, _constmaxitr);   // itr < MAX_ITR
				__mmask16 _maskradius = _mm512_cmple_ps_mask(_mod, _const4); // zr * zr + zi * zi <= 4.0
				
				__mmask16 _whileTrue = _maskradius & _masknumitr; // (zr * zr + zi * zi <= 4.0 && itr < MAX_ITR)

				//////////////////////// evalMandel(z, c)//////////////////////////////////

				_a = _zr;
				_b = _zi;

				_zr = _mm512_sub_ps(_zr2, _zi2); // zr = zr * zr - zi * zi
				_zr = _mm512_add_ps(_zr, _cr); // zr += cr

				_zi = _mm512_mul_ps(_a, _b); // zi = a * b

				// zi = zi * 2 + ci
				_zi = _mm512_fmadd_ps(_zi, _const2, _ci);

				//////////////////////// evalMandel(z, c)//////////////////////////////////


				// itr++;

				__m512i _zero = _mm512_setzero_epi32();
				__m512i _onlytrue = _mm512_mask_set1_epi32(_zero, _whileTrue, 1);

				_inc1i = _mm512_and_si512(_onlytrue, _const1i);
				_itr = _mm512_add_epi32(_itr, _inc1i);

				// if (any one register satisfies while condition) goto loop;
				if (_mm512_mask2int(_whileTrue))
					goto loop;

				///////////////////////////// while (zr * zr + zi * zi <= 2 * 2 && itr < MAX_ITR) ///////////////////////////////

				// if (itr < MAX_ITR) frameBuffer[index] = BLACK;
				// else frameBuffer[index] = CYAN;


				// pixel masks read in correct endianness |...7, 6, 5, 4, 3, 2, 1, 0| instead of |0, 1, 2, 3, 4, 5, 6, 7...| 			
				int pixel0 = _masknumitr & (1 << 15);
				int pixel1 = _masknumitr & (1 << 14);
				int pixel2 = _masknumitr & (1 << 13);
				int pixel3 = _masknumitr & (1 << 12);
				int pixel4 = _masknumitr & (1 << 11);
				int pixel5 = _masknumitr & (1 << 10);
				int pixel6 = _masknumitr & (1 << 9);
				int pixel7 = _masknumitr & (1 << 8);
				int pixel8 = _masknumitr & (1 << 7);
				int pixel9 = _masknumitr & (1 << 6);
				int pixel10 = _masknumitr & (1 << 5);
				int pixel11 = _masknumitr & (1 << 4);
				int pixel12 = _masknumitr & (1 << 3);
				int pixel13 = _masknumitr & (1 << 2);
				int pixel14 = _masknumitr & (1 << 1);
				int pixel15 = _masknumitr & 1;

				frameBuffer[index0] = pixel0 ? BLACK : CYAN;
				frameBuffer[index1] = pixel1 ? BLACK : CYAN;
				frameBuffer[index2] = pixel2 ? BLACK : CYAN;
				frameBuffer[index3] = pixel3 ? BLACK : CYAN;
				frameBuffer[index4] = pixel4 ? BLACK : CYAN;
				frameBuffer[index5] = pixel5 ? BLACK : CYAN;
				frameBuffer[index6] = pixel6 ? BLACK : CYAN;
				frameBuffer[index7] = pixel7 ? BLACK : CYAN;
				frameBuffer[index8] = pixel8 ? BLACK : CYAN;
				frameBuffer[index9] = pixel9 ? BLACK : CYAN;
				frameBuffer[index10] = pixel10 ? BLACK : CYAN;
				frameBuffer[index11] = pixel11 ? BLACK : CYAN;
				frameBuffer[index12] = pixel12 ? BLACK : CYAN;
				frameBuffer[index13] = pixel13 ? BLACK : CYAN;
				frameBuffer[index14] = pixel14 ? BLACK : CYAN;
				frameBuffer[index15] = pixel15 ? BLACK : CYAN;
			}
		}
	}
	
	if (!isBenchmark)
		saveImg(frameBuffer, width, height);
	
	delete [] frameBuffer;
}

#endif // ISA_AVX512

/////////////////////////////////////////////// AVX-512 END /////////////////////////////////////////////

/********************************************INTRINSICS END*******************************************/

void drawMandelbrot(const int &width, const int &height, int isBenchmark)
{
	uint32_t bufferSize = width * height;
	Color3f *frameBuffer = new Color3f[bufferSize];
	uint32_t index = 0;
	for (int y = 0; y < height; y++) // y axis of the image	
	{
		for (int x = 0; x < width; x++) // x axis of the image
		{
			index = y * width + x;
			int itr = 0;
			Complex z, c;
			c.a = getMappedScaleX((float)x, width);
			c.b = getMappedScaleY((float)y, height);
			while (z.real() * z.real() + z.imaginary() * z.imaginary() <= 2 * 2 && itr < MAX_ITR)
			{
				evalMandel(z, c);
				itr++;
			}
			if (itr < MAX_ITR)
				frameBuffer[index] = BLACK;
			else
				frameBuffer[index] = CYAN;
		}
	}
	
	if (!isBenchmark)
		saveImg(frameBuffer, width, height);
	
	delete [] frameBuffer;
}

void drawMandelbrotOMP(const int &width, const int &height, int isBenchmark)
{
	uint32_t bufferSize = width * height;
	Color3f *frameBuffer = new Color3f[bufferSize];	
	size_t nThreads = std::thread::hardware_concurrency();
#pragma omp parallel num_threads(nThreads) shared(frameBuffer)
	{
#pragma omp for schedule(dynamic, 1) 
		for (int y = 0; y < height; y++) // y axis of the image	
		{
			for (int x = 0; x < width; x++) // x axis of the image
			{
				uint32_t index = y * width + x;
				int itr = 0;
				Complex z, c;
				c.a = getMappedScaleX((float)x, width);
				c.b = getMappedScaleY((float)y, height);
				while (z.real() * z.real() + z.imaginary() * z.imaginary() <= 2 * 2 && itr < MAX_ITR)
				{
					evalMandel(z, c);
					itr++;
				}
				if (itr < MAX_ITR)
					frameBuffer[index] = BLACK;
				else
					frameBuffer[index] = CYAN;
			}
		}
	}
	
	if (!isBenchmark)
		saveImg(frameBuffer, width, height);
	
	delete [] frameBuffer;
}

void drawMandelbrotThread(int renderWidth, int renderHeight, int tileWidth, int tileHeight, int startX, int startY, Color3f *frameBuffer)
{
	for (int y = 0; y < tileHeight; y++) // y axis of the image	
	{
		for (int x = startX; x < tileWidth; x++) // x axis of the image
		{
			uint32_t index = (startY + y) * tileWidth + x;
			int itr = 0;
			Complex z, c;
			c.a = getMappedScaleX((float)x, renderWidth);
			c.b = getMappedScaleY((float)(startY + y), renderHeight);
			while (z.real() * z.real() + z.imaginary() * z.imaginary() <= 2 * 2 && itr < MAX_ITR)
			{
				evalMandel(z, c);
				itr++;
			}
			if (itr < MAX_ITR)
				frameBuffer[index] = BLACK;
			else
				frameBuffer[index] = CYAN;
		}
	}
}

void drawMandelbrotMT(const int &width, const int &height, int isBenchmark)
{
	uint32_t bufferSize = width * height, startX = 0, startY = 0; 
	size_t nThreads = std::thread::hardware_concurrency();
	Color3f *frameBuffer = new Color3f[bufferSize];	
	
	size_t tileWidth = width, tileHeight = height / nThreads;
	
	std::thread *mandelThreads = new std::thread[nThreads];
	for (int i = 0; i < nThreads - 1; ++i)
	{
		mandelThreads[i] = std::thread(drawMandelbrotThread, width, height, tileWidth, tileHeight, startX, startY, frameBuffer);
		startY += tileHeight;
	}
	mandelThreads[nThreads - 1] = std::thread(drawMandelbrotThread, width, height, tileWidth, height - startY, startX, startY, frameBuffer);
	
	for (int i = 0; i < nThreads; ++i)
		mandelThreads[i].join();
	
	if (!isBenchmark)
		saveImg(frameBuffer, width, height);	
	
	delete [] frameBuffer;
	delete [] mandelThreads;
}

bool isValidISA()
{

#if defined(ISA_SSE) && defined(ISA_AVX) && defined(ISA_AVX512)
	return false;
#endif

	uint32_t flag = 0;

#ifdef ISA_SSE
	flag ^= 1;
#endif

#ifdef ISA_AVX
	flag ^= 1;
#endif

#ifdef ISA_AVX512
	flag ^= 1;
#endif

	return (flag & 1);
}

int main()
{	
	if (!isValidISA())
	{
		std::cerr << "Invalid compilation flags detected in binary. Aborting..." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	
	int width = 0, height = 0, isBenchmark = 0, useSIMD = 0;
	char ch = ' ';	
	std::cout << "This program renders the Mandelbrot set in a non real-time context.\n";
	std::cout << "Please enter the desired resolution in pixels. Width (then press return), followed by height (press return).\n";
	std::cin >> width;
	std::cin >> height;
	std::cout << "Please enter the desired number of iterations to calculate the Mandelbrot set.\n";
	std::cin >> MAX_ITR;
	std::cout << "Number of logical processors detected: " << std::thread::hardware_concurrency() << std::endl;
	std::cout << "Enable multithreading? (Y/N)\n";
	std::cin >> ch;
	std::cout << "Enable benchmark mode? Disables file output for more accurate performance measurement. (1 = Yes / 0 = No[default])\n";
	std::cin >> isBenchmark;		
	
	std::chrono::time_point<std::chrono::high_resolution_clock> start, stop;
	
	if (ch == 'y' || ch == 'Y')
	{		
		std::cout << "Use OpenMP? (Y/N)\n";
		std::cin >> ch;
		if (ch == 'y' || ch == 'Y')
		{
			std::cout << "Use SIMD for better performance? (1 = Yes / 0 = No[default])\n";
			std::cin >> useSIMD;
			std::cout << "Using OpenMP for parallelism.\n";
			std::cout << "Generating the Mandelbrot set...\n";
			start = std::chrono::high_resolution_clock::now();
			
			if (useSIMD)
			{
#if ISA_SSE
				drawMandelbrotOMPSSE(width, height, isBenchmark, std::thread::hardware_concurrency());
#elif ISA_AVX
				drawMandelbrotOMPAVX(width, height, isBenchmark, std::thread::hardware_concurrency());
#elif ISA_AVX512
				drawMandelbrotOMPAVX512(width, height, isBenchmark, std::thread::hardware_concurrency());
#else
				std::cerr << "Compilation error. Please specify the correct instruction set architecture "
                             "during compilation. Please see the documentation in the source file. Aborting..." << std::endl;
				std::exit(1);
#endif
			}
			else
				drawMandelbrotOMP(width, height, isBenchmark);
			
			stop = std::chrono::high_resolution_clock::now();
		}
		else
		{
			std::cout << "Using STL threads for parallelism.\n";
			std::cout << "Generating the Mandelbrot set...\n";
			start = std::chrono::high_resolution_clock::now();
			drawMandelbrotMT(width, height, isBenchmark);	
			stop = std::chrono::high_resolution_clock::now();			
		}
	}		
	else if ((ch == 'n' || ch == 'N'))
	{
		std::cout << "Use SIMD for better performance? (1 = Yes / 0 = No[default])\n";
		std::cin >> useSIMD;
		std::cout << "Generating the Mandelbrot set...\n";
		start = std::chrono::high_resolution_clock::now();
		
		if (useSIMD)
		{	
#if ISA_SSE
				drawMandelbrotOMPSSE(width, height, isBenchmark, 1);
#elif ISA_AVX
				drawMandelbrotOMPAVX(width, height, isBenchmark, 1);
#elif ISA_AVX512
				drawMandelbrotOMPAVX512(width, height, isBenchmark, 1);
#else
				std::cerr << "Compilation error. Please specify the correct instruction set architecture "
                             "during compilation. Please see the documentation in the source file. Aborting..." << std::endl;
				std::exit(1);
#endif
		}
		else
			drawMandelbrot(width, height, isBenchmark);
		
		stop = std::chrono::high_resolution_clock::now();
	}
	else
	{
		std::cout << "Invalid choice! Aborting...\n";
		std::exit(EXIT_FAILURE);
	}
	
	auto diffMSec = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	auto diffSec = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
	
	if ((int)diffSec.count() < 1)
		std::cout << "Time taken is " << (int)diffMSec.count() << " milliseconds.\n";
	else
		std::cout << "Time taken is " << (int)diffSec.count() << " seconds.\n";
	
	return 0;
}