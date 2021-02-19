/*

The following program renders the Mandelbrot set. This set consists of a set of complex numbers with a specific property. A complex number Z is
a number that has two parts; one real, and one imaginary. It is usually written in the form Z = a + bi, where a is the real part, b is the imaginary part
and i is the square root of -1. The Mandelbrot set consists of all such complex numbers C, such that Z = Z^2 + C [ where C is a given complex number ] and Z 
is bounded under iteration with Z (a complex number itself) starting from 0 (every real number is a complex number with the imaginary part being zero).

Therefore,

		Z_n+1 = Z^2 + C [ let C = a + bi ]
		
		=> Z_0 = 0 + (a + bi) = a + bi

		=> Z_1 = Z_0^2 + C = (a + bi)^2 + (a + bi) = (a^2 - b^2 + 2abi) + (a + bi) = (a^2 - b^2 + a) + i(2ab + b)

		=> Z_2 = Z_1^2 + C and so on ...


The modulus of a complex number Z = (a + bi) is defined as sqrt(a^2 + b^2). This is geometrically the Euclidean distance between the origin of the complex plane 
and Z. If this distance is always less than some value for a given complex number C , under the above iteration, then C is set to be bounded. If it is bounded within
the Mandelbrot set bounds, then the given complex number C is said to be a part of the Mandelbrot set.

The Mandelbrot set is what is known as a fractal pattern. It has repeating patterns visible upon zooming and is self-similar. It is named after the mathematician
Benoit Mandelbrot. It is the most well-known fractal. Computing the Mandelbrot used to be a very compute intensive task to perform once. Nowadays, it can be trivially 
computed by even processors in smart watches. For fun, there are simple multithreaded implementations provided here using OpenMP and C++ Stand Library threads.
This is an offline variant of this algorithm. The multithreading should show speed ups at very high resolutions only. Otherwise, the overhead
of creating the threads might be too high. The bottleneck in this program is the disk access for writing out the rendered buffer to file,
especially at lower resolutions.


To know more about the Mandelbrot set please refer to https://en.wikipedia.org/wiki/Mandelbrot_set

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
	double r; // red 0-1
	double g; // green 0-1
	double b; // blue 0-1
	
	Color3f() : r(0), g(0), b(0) {}
	Color3f(const double &r_, const double &g_, const double &b_) : r(r_), g(g_), b(b_) {}

	inline Color3f operator&(const Color3f &col) const // color modulate
	{
		return Color3f(r * col.r, g * col.g, b * col.b);
	}

	inline Color3f operator*(const double &c) const // scalar multiplication
	{
		return Color3f(c * r, c * g, c * b);
	}

	inline Color3f operator+(const Color3f &c) const // addition
	{
		return Color3f(r + c.r, g + c.g, b + c.b);
	}

	inline Color3f operator/(const double &c) const // scalar division
	{
		double inv = 1. / c;
		return Color3f(r * inv, g * inv, b * inv);
	}

	inline Color3f& operator*=(const double &c) // scalar division
	{
		r *= c;
		g *= c;
		b *= c;
		return *this;
	}

	inline Color3f& operator/=(const double &c) // scalar division
	{
		double inv = 1. / c;
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
	double a;
	double b;

	Complex() : a(0), b(0) {}
	Complex(double a_, double b_) : a(a_), b(b_) {}

	double real() const
	{
		return a;
	}

	double imaginary() const
	{
		return b;
	}

	double getModulus()
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
double getMappedScaleX(const int &x, const int &xMax)
{
	return ((x / (double) xMax) * 3.5) - 2.5;
}

// maps pixel values in the Y direction of screen space between (-1, 1)
double getMappedScaleY(const int &y, const int &yMax)
{
	return ((y / (double) yMax) * 2) - 1;
}

void evalMandel(Complex &z, const Complex &c)
{
	double zReal = z.a;
	double zImaginary = z.b;
	z.a = zReal * zReal - zImaginary * zImaginary + c.a;
	z.b = 2 * zReal * zImaginary + c.b;
}

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
			c.a = getMappedScaleX((double)x, width);
			c.b = getMappedScaleY((double)y, height);
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
	
	delete[] frameBuffer;
}

void drawMandelbrotSIMD(const int &width, const int &height, int isBenchmark)
{
	uint32_t bufferSize = width * height;
	Color3f *frameBuffer = new Color3f[bufferSize];
	float invW = (1./ width) * 3.5, invH = (1./ height) * 2;
	
	// 32-bit float registers
	__m128 _zr, _zi, _cr, _ci, _a, _b, _zr2, _zi2, _const2, _invw, _invh, _const2p5neg,
			_const1neg, _mod, _const4, _maskwhile; 
	
	// 32-bit signed int registers
	__m128i _maskwhileitr, _itr, _constmaxitr, _indexi, _inc1i, _const1i;
	// __m128i _widthi = _mm_setr_epi32(width, width, width, width);
	
	// initialize floating point registers
	_const1neg = _mm_set1_ps(-1.0);
	_const2 = _mm_set1_ps(2.0);
	_const4 = _mm_set1_ps(4.0);
	_const2p5neg = _mm_set1_ps(-2.5);
	
	_invw = _mm_set1_ps(invW);
	_invh = _mm_set1_ps(invH);	
	
	// initialize integer registers
	 _constmaxitr = _mm_set1_epi32(MAX_ITR);
	_itr = _mm_set1_epi32(0);
	_const1i = _mm_set1_epi32(1);	
	
	
	for (int y = 0; y < height; y++) // y axis of the image	
	{
		int yw = y * width;
		
		for (int x = 0; x < width; x += 4) // x axis of the image
		{
			_inc1i = _const1i; // while loop iteration condition
				
			__m128i _xi = _mm_setr_epi32(x + 3, x + 2, x + 1, x);
			__m128 _xf = _mm_setr_ps((float)x + 3, (float)x + 2, (float)x + 1, (float)x);
			
			__m128i _yi = _mm_set1_epi32(y);
			__m128 _yf = _mm_set1_ps((float)y);			
			
			
			int index = y * width + x;
			
			int index0 = yw + x; // y * width + x
			int index1 = yw + x + 1; // y * width + (x + 1)
			int index2 = yw + x + 2; // y * width + (x + 2)
			int index3 = yw + x + 3; // y * width + (x + 3)
			
			_indexi = _mm_setr_epi32(index3, index2, index1, index0); // store set of 4 packed consecutive indices
			
			int itr = 0;	
			
			float zr = 0, zi = 0, cr = 0, ci = 0;				
			_zr = _mm_set1_ps(0);
			_zi = _mm_set1_ps(0);
			_cr = _mm_set1_ps(0);
			_ci = _mm_set1_ps(0);
			
			// cr = (x * invW) - 2.5;			
			// _cr =  _mm_fmadd_ps(_xf, _invw, _const2p5neg); // No FMA on my Nehalem CPU			 
			_cr = _mm_mul_ps(_xf, _invw);
			_cr = _mm_add_ps(_cr, _const2p5neg);
			
			
			// ci = (y * invH) - 1;			
			// _ci =  _mm_fmadd_ps(_yf, _invh, _const1neg);  // No FMA on my Nehalem CPU
			_ci = _mm_mul_ps(_yf, _invh);
			_ci = _mm_add_ps(_ci, _const1neg);			
			
			_zr2 = _mm_mul_ps(_zr, _zr); // zr * zr
			_zi2 = _mm_mul_ps(_zi, _zi); // zi * zi
			_mod = _mm_add_ps(_zr2, _zi2); // zr * zr + zi * zi
			
			loop: // while(...)
			
			_maskwhile = _mm_cmple_ps(_mod, _const4); // zr * zr + zi * zi <= 4.0
			_maskwhileitr = _mm_cmplt_epi32(_itr, _constmaxitr); // itr < MAX_ITR			
			_maskwhile = _mm_and_ps(_maskwhile, _mm_castsi128_ps(_maskwhileitr)); // (zr * zr + zi * zi <= 4.0 && itr < MAX_ITR)
			
			while (zr * zr + zi * zi <= 2 * 2 && itr < MAX_ITR)
			{
				_a = _zr;
				_b = _zi;
				
				_zr = _mm_sub_ps(_zr2, _zi2); // zr = zr * zr - zi * zi
				_zr = _mm_add_ps(_zr, _cr); // zr += cr
				
				_zi = _mm_mul_ps(_a, _b); // zi = a * b
				
				// zi = zi * 2 + ci
				// _zi = _mm_fmadd_ps(_zi, _const2, _ci); // No FMA on my Nehalem CPU
				_zi = _mm_mul_ps(_zi, _const2);
				_zi = _mm_add_ps(_zi, _ci);
				

				_inc1i = _mm_and_ps(_maskwhile, _inc1i)
				// _itr
			}
			
			// if (any one register satisfies while condition)
				// goto loop;
			
			
			
			if (itr < MAX_ITR)
				frameBuffer[index] = BLACK;
			else
				frameBuffer[index] = CYAN;
		}
	}
	
	if (!isBenchmark)
		saveImg(frameBuffer, width, height);
	
	delete[] frameBuffer;
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
				c.a = getMappedScaleX((double)x, width);
				c.b = getMappedScaleY((double)y, height);
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
	
	delete[] frameBuffer;
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
			c.a = getMappedScaleX((double)x, renderWidth);
			c.b = getMappedScaleY((double)(startY + y), renderHeight);
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
	for (int i = 0; i < nThreads; ++i)
	{
		mandelThreads[i] = std::thread(drawMandelbrotThread, width, height, tileWidth, tileHeight, startX, startY, frameBuffer);
		startY += tileHeight;
	}
	
	for (int i = 0; i < nThreads; ++i)
		mandelThreads[i].join();
	
	if (!isBenchmark)
		saveImg(frameBuffer, width, height);	
	
	delete[] frameBuffer;
	delete [] mandelThreads;
}

int main()
{
	int width = 0, height = 0, isBenchmark = 0, useSIMD = 0;
	char ch = ' ';	
	std::cout << "This program renders the Mandelbrot set in a non real-time context.\n";
	std::cout << "Please enter the desired resolution in pixels. Width, followed by height.\n";
	std::cin >> width;
	std::cin >> height;
	std::cout << "Please enter the desired number of iterations to calculate the Mandelbrot set.\n";
	std::cin >> MAX_ITR;
	std::cout << "Enable multithreading? (Y/N)\n";
	std::cin >> ch;
	std::cout << "Enable benchmark mode? Disables file output for more accurate performance measurement. (1 = Yes / 0 = No[default])\n";
	std::cin >> isBenchmark;
	std::cout << "Use SIMD for better performance? (1 = Yes / 0 = No[default])\n";
	std::cin >> useSIMD;
	
	std::chrono::time_point<std::chrono::high_resolution_clock> start, stop;
	
	if (ch == 'y' || ch == 'Y')
	{		
		std::cout << "Use OpenMP? (Y/N)\n";
		std::cin >> ch;
		if (ch == 'y' || ch == 'Y')
		{
			std::cout << "Using OpenMP for parallelism.\n";
			start = std::chrono::high_resolution_clock::now();
			drawMandelbrotOMP(width, height, isBenchmark);
			stop = std::chrono::high_resolution_clock::now();
		}
		else
		{
			std::cout << "Using STL threads for parallelism.\n";
			start = std::chrono::high_resolution_clock::now();
			drawMandelbrotMT(width, height, isBenchmark);	
			stop = std::chrono::high_resolution_clock::now();			
		}
	}		
	else if ((ch == 'n' || ch == 'N'))
	{
		start = std::chrono::high_resolution_clock::now();
		
		if (useSIMD)
			drawMandelbrotSIMD(width, height, isBenchmark);
		else
			drawMandelbrot(width, height, isBenchmark);
		
		stop = std::chrono::high_resolution_clock::now();
	}
	else
	{
		std::cout << "Invalid choice! Aborting...\n";
		exit(EXIT_FAILURE);
	}
	
	auto diff = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
	std::cout << "Time taken is " << (int)diff.count() << " seconds.\n";
	return 0;
}