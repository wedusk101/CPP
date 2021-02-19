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

void drawMandelbrot(const int &, const int &, int);
void drawMandelbrotOMP(const int &, const int &, int);
void drawMandelbrotMT(const int &, const int &, int);
double getMappedScaleX(const int &, const int &); // maps pixel values in the X direction of screen space between (-2.5, 1)
double getMappedScaleY(const int &, const int &); // maps pixel values in the Y direction of screen space between (-1, 1)
void evalMandel(Complex &, const Complex &);

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

int main()
{
	int width = 0, height = 0, isBenchmark = 0;
	char ch = ' ';	
	std::cout << "This program renders the Mandelbrot set in a non real-time context.\n";
	std::cout << "Please enter the desired resolution in pixels. Width, followed by height.\n";
	std::cin >> width;
	std::cin >> height;
	std::cout << "Please enter the desired number of iterations to calculate the Mandelbrot set.\n";
	std::cin >> MAX_ITR;
	std::cout << "Enable multithreading? (Y/N)\n";
	std::cin >> ch;
	std::cout << "Enable benchmark mode? Disables file output for more accurate performance measurement. (1 = Yes / 2 = No[default])\n";
	std::cin >> isBenchmark;
	
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

double getMappedScaleX(const int &x, const int &xMax)
{
	return ((x / (double) xMax) * 3.5) - 2.5;
}

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