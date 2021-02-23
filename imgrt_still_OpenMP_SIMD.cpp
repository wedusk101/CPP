#include <fstream>
#include <cmath>
#include <iostream> 
#include <string>
#include <chrono>
#include <climits>
#include <cstdlib>
#include <cstdint>
#include <thread>
#include <vector>
#include <cstring>
#include <immintrin.h>
#include "omp.h"


struct Vec3
{
	float x;
	float y;
	float z;
	
	Vec3() : x(0), y(0), z(0) {}
	Vec3(const float &x_, const float &y_, const float &z_) : x(x_), y(y_), z(z_) {}
	
	Vec3(const Vec3 &v) : x(v.x), y(v.y), z(v.z) {}	
	
	Vec3& operator=(const Vec3 &v) // copy assignment
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}
	
	Vec3 operator+(const Vec3 &v) const // addition
	{
		return Vec3(x + v.x, y + v.y, z + v.z);
	}
	
	Vec3 operator-(const Vec3 &v) const // subtraction
	{
		return Vec3(x - v.x, y - v.y, z - v.z);
	}
	
	Vec3 operator*(const float &c) const // scalar multiplication
	{
		return Vec3(c * x, c * y, c * z);
	}
	
	Vec3 operator/(const float &c) const // scalar division
	{
		return Vec3(x/c, y/c, z/c);
	}
	
	float operator%(const Vec3 &v) const // dot product
	{
		return x * v.x + y * v.y + z * v.z;
	}
	
	Vec3 operator&(const Vec3 &v) const // cross product
	{
		return Vec3(y * v.z - v.y * z, z * v.x - x * v.z, x * v.y - y * v.x);
	}
	
	float dot(const Vec3 &v) const // dot product
	{
		return x * v.x + y * v.y + z * v.z;
	}
	
	float getMagnitude() const
	{
		return sqrt(x * x + y * y + z * z);
	}
	
	Vec3 getNormalized() const
	{
		float mag = getMagnitude();
		return Vec3(x/mag, y/mag, z/mag);
	}
	
	void display() const
	{
		std::cout << "[" << x << ", " << y << ", " << z << "]" << std::endl;
	}
};

struct Ray
{
	Vec3 o; // origin
	Vec3 d; // direction
    mutable float t;
	float tMin;
	mutable float tMax;
	
	Ray(const Vec3 &o_, const Vec3 &d_) : o(o_), d(d_), t(INT_MAX), tMin(0.1), tMax(INT_MAX) {}
	Ray(const Ray &r)
	{
		o = r.o;
		d = r.d;
		t = r.t;
		tMin = r.tMin;
		tMax = r.tMax;
	}
	
	Ray& operator=(const Ray &r)
	{
		o = r.o;
		d = r.d;
		t = r.t;
		tMin = r.tMin;
		tMax = r.tMax;
		return *this;
	}
	
	void display() const
	{
		std::cout << "\n\n";
		std::cout << "Origin: ";
		o.display();
		std::cout << "Direction: ";
		d.display();
		std::cout << "t: " << t << std::endl;
		std::cout << "tMin: " << tMin << std::endl;
		std::cout << "tMax: " << tMax << std::endl;		
		std::cout << "\n\n";
	}
};

struct Vec3Cluster4
{
	float x[4];
	float y[4];
	float z[4];
};

struct RayCluster4
{
	float ox[4];
	float oy[4];
	float oz[4];
	
	float dx[4];
	float dy[4];
	float dz[4];
	
	float t[4];
	float tMin[4];
	float tMax[4];
	
	float hasHit[4];
};

struct Geometry
{
	Vec3 color;

	virtual ~Geometry() {}

	virtual void intersectsBatch(RayCluster4 &rayBatch) const = 0;
	virtual bool intersects(const Ray &ray) const = 0;
	virtual Vec3 getNormal(const Vec3 &point) const = 0;
};

struct Sphere : public Geometry
{
	Vec3 center;
	float radius;

	Sphere(const Vec3 &c, const float &rad, const Vec3 &col) : center(c), radius(rad)
    {
        color = col;
    }

	~Sphere() {}
	
	virtual Vec3 getNormal(const Vec3 &point) const // returns the surface normal at a point
	{
		return (point - center)/radius;
	}
	
	virtual bool intersects(const Ray &ray) const
	{
		const double eps = 1e-4;
		const Vec3 oc = ray.o - center;
		const double b = 2 * (ray.d % oc);
		const double a = ray.d % ray.d;
		const double c = (oc % oc) - (radius * radius);
		double delta = b * b - 4 * a * c;
		if(delta < eps) // discriminant is less than zero
			return false;
		delta = sqrt(delta);
		const double t0 = (-b + delta) / (2 * a);
		const double t1 = (-b - delta) / (2 * a);
		ray.t = (t0 < t1) ? t0 : t1;
		if (ray.t >= ray.tMin && ray.t <= ray.tMax)
		{
			ray.tMax = ray.t;
			return true;
		}
		else
			return false;
	}
	
	virtual void intersectsBatch(RayCluster4 &rayBatch) const
	{
		// const float eps = 1e-4;
		__m128 _eps = _mm_set1_ps(1e-4);
		
		__m128 _rayox = _mm_setr_ps(rayBatch.ox[3], rayBatch.ox[2], rayBatch.ox[1], rayBatch.ox[0]);
		__m128 _rayoy = _mm_setr_ps(rayBatch.oy[3], rayBatch.oy[2], rayBatch.oy[1], rayBatch.oy[0]);
		__m128 _rayoz = _mm_setr_ps(rayBatch.oz[3], rayBatch.oz[2], rayBatch.oz[1], rayBatch.oz[0]);
		
		__m128 _raydx = _mm_setr_ps(rayBatch.dx[3], rayBatch.dx[2], rayBatch.dx[1], rayBatch.dx[0]);
		__m128 _raydy = _mm_setr_ps(rayBatch.dy[3], rayBatch.dy[2], rayBatch.dy[1], rayBatch.dy[0]);
		__m128 _raydz = _mm_setr_ps(rayBatch.dz[3], rayBatch.dz[2], rayBatch.dz[1], rayBatch.dz[0]);
		
		__m128 _centerx = _mm_set1_ps(center.x);
		__m128 _centery = _mm_set1_ps(center.y);
		__m128 _centerz = _mm_set1_ps(center.z);
		
		// const Vec3 oc = ray.o - center;
		__m128 _ocx = _mm_sub_ps(_rayox, _centerx);
		__m128 _ocy = _mm_sub_ps(_rayoy, _centery);
		__m128 _ocz = _mm_sub_ps(_rayoz, _centerz);		
		
		// const float b = 2 * (ray.d % oc);
		__m128 _const2 = _mm_set1_ps(2.0);		
		__m128 _bx = _mm_mul_ps(_raydx, _ocx);
		__m128 _by = _mm_mul_ps(_raydy, _ocy);
		__m128 _bz = _mm_mul_ps(_raydz, _ocz);	
		
		__m128 _b = _mm_add_ps(_bx, _by);
		_b = _mm_add_ps(_b, _bz);
		_b = _mm_mul_ps(_b, _const2);
		
		// const float a = rayd % rayd;
		__m128 _ax = _mm_mul_ps(_raydx, _raydx);
		__m128 _ay = _mm_mul_ps(_raydy, _raydy);
		__m128 _az = _mm_mul_ps(_raydz, _raydz);
		
		__m128 _a = _mm_add_ps(_ax, _ay);
		_a = _mm_add_ps(_a, _az);		
		
		// const float c = (oc % oc) - (radius * radius);
		__m128 _ocx2 = _mm_mul_ps(_ocx, _ocx);
		__m128 _ocy2 = _mm_mul_ps(_ocy, _ocy);
		__m128 _ocz2 = _mm_mul_ps(_ocz, _ocz);
		
		__m128 _ocdot2 = _mm_add_ps(_ocx2, _ocy2);
		_ocdot2 = _mm_add_ps(_ocdot2, _ocz2);
		
		__m128 _radius = _mm_set1_ps(radius);
		__m128 _radius2 = _mm_mul_ps(_radius, _radius);
		__m128 _c = _mm_sub_ps(_ocdot2, _radius2);
		
		// float delta = b * b - 4 * a * c;
		__m128 _const4 = _mm_set1_ps(4.0);
		__m128 _b2 = _mm_mul_ps(_b, _b);
		__m128 _4ac = _mm_mul_ps(_a, _c);
		_4ac = _mm_mul_ps(_4ac, _const4);
		__m128 _delta = _mm_sub_ps(_b2, _4ac);		
		
		// if(delta < eps) return false;
		__m128 _maskdelta = _mm_cmplt_ps(_delta, _eps);
		
		int missRay0 = _mm_extract_ps(_maskdelta, 3);
		int missRay1 = _mm_extract_ps(_maskdelta, 2);
		int missRay2 = _mm_extract_ps(_maskdelta, 1);
		int missRay3 = _mm_extract_ps(_maskdelta, 0);
		
		rayBatch.hasHit[0] = missRay0 ? 0 : rayBatch.hasHit[0];
		rayBatch.hasHit[1] = missRay1 ? 0 : rayBatch.hasHit[1];
		rayBatch.hasHit[2] = missRay2 ? 0 : rayBatch.hasHit[2];
		rayBatch.hasHit[3] = missRay3 ? 0 : rayBatch.hasHit[3];
		
		if (missRay0 && missRay1 && missRay2 && missRay3)
			return;
		
		//delta = sqrt(delta);
		_delta = _mm_sqrt_ps(_delta);
		
		// const float t0 = (-b + delta) / (2 * a);
		__m128 _2a = _mm_mul_ps(_a, _const2);
		__m128 _const1n = _mm_set1_ps(-1.0);
		_b = _mm_mul_ps(_b, _const1n); // -b = b * (-1)		
		__m128 _bpdelta = _mm_add_ps(_b, _delta);
		__m128 _t0 = _mm_div_ps(_bpdelta, _2a);
		
		//const float t1 = (-b - delta) / (2 * a);
		__m128 _bmdelta = _mm_sub_ps(_b, _delta);
		__m128 _t1 = _mm_div_ps(_bmdelta, _2a);
		
		// ray.t = (t0 < t1) ? t0 : t1;
		__m128 _maskt = _mm_cmplt_ps(_t0, _t1);
		
		int tcmp0 = _mm_extract_ps(_maskt, 3);
		int tcmp1 = _mm_extract_ps(_maskt, 2);
		int tcmp2 = _mm_extract_ps(_maskt, 1);
		int tcmp3 = _mm_extract_ps(_maskt, 0);
		
		rayBatch.t[0] = tcmp0 ? _mm_extract_ps(_t0, 3) : _mm_extract_ps(_t1, 3);
		rayBatch.t[1] = tcmp1 ? _mm_extract_ps(_t0, 2) : _mm_extract_ps(_t1, 2);
		rayBatch.t[2] = tcmp2 ? _mm_extract_ps(_t0, 1) : _mm_extract_ps(_t1, 1);
		rayBatch.t[3] = tcmp3 ? _mm_extract_ps(_t0, 0) : _mm_extract_ps(_t1, 0);
		
		// if (ray.t >= ray.tMin && ray.t <= ray.tMax)
		//		ray.tMax = ray.t; return true;
	
		__m128 _rayt = _mm_setr_ps(rayBatch.t[3], rayBatch.t[2], rayBatch.t[1], rayBatch.t[0]);
		__m128 _raytmin = _mm_setr_ps(rayBatch.tMin[3], rayBatch.tMin[2], rayBatch.tMin[1], rayBatch.tMin[0]);
		__m128 _raytmax = _mm_setr_ps(rayBatch.tMax[3], rayBatch.tMax[2], rayBatch.tMax[1], rayBatch.tMax[0]);
		__m128 _maskRtgtMin = _mm_cmpge_ps(_rayt, _raytmin);
		__m128 _maskRtltMax = _mm_cmple_ps(_rayt, _raytmax);
		__m128 _maskhit = _mm_and_ps(_maskRtgtMin, _maskRtltMax);
		
		int maskHit0 = _mm_extract_ps(_maskhit, 3);
		int maskHit1 = _mm_extract_ps(_maskhit, 2);
		int maskHit2 = _mm_extract_ps(_maskhit, 1);
		int maskHit3 = _mm_extract_ps(_maskhit, 0);
		
		if (maskHit0) // && !missRay0)
		{
			rayBatch.tMax[0] = rayBatch.t[0];
			rayBatch.hasHit[0] = 1;
		}
		
		if (maskHit1) // && !missRay1)
		{
			rayBatch.tMax[1] = rayBatch.t[1];
			rayBatch.hasHit[1] = 1;
		}
		
		if (maskHit2) // && !missRay2)
		{
			rayBatch.tMax[2] = rayBatch.t[2];
			rayBatch.hasHit[2] = 1;
		}
		
		if (maskHit3) // && !missRay3)
		{
			rayBatch.tMax[3] = rayBatch.t[3];
			rayBatch.hasHit[3] = 1;
		}
	}		
};

struct Plane : public Geometry
{
	Vec3 normal; // normal of the plane
	Vec3 point; // a point on the plane
	
	Plane(const Vec3 &n, const Vec3 &p, const Vec3 &col) : normal(n), point(p)
    {
        color = col;
    }

	~Plane() {}
	
	virtual Vec3 getNormal(const Vec3 &point) const
	{
		return normal;
	}
	
	virtual bool intersects(const Ray &ray) const
	{
		const double eps = 1e-4;;
		double parameter = ray.d % normal;
		if(fabs(parameter) < eps) // ray is parallel to the plane
			return false;
		ray.t = ((point - ray.o) % normal) / parameter;
		if (ray.t >= ray.tMin && ray.t <= ray.tMax)
		{
			ray.tMax = ray.t;
			return true;
		}
		else
			return false;
	}
	
	virtual void intersectsBatch(RayCluster4 &rayBatch) const
	{
		return;
	}
};

struct Light
{
	Vec3 position;
	float radius;
	Vec3 color;
	float intensity;
	
	Light(const Vec3 &position_, const float &radius_, const Vec3 &color_, const float &intensity_) : position(position_), radius(radius_), color(color_), intensity(intensity_) {}
};

struct Camera
{
	Vec3 position;
	Vec3 direction;
	
	// add a lower left corner for orientation
	
	Camera(const Vec3 &pos, const Vec3 &dir) : position(pos), direction(dir) {}
};

inline float dot(const Vec3 &a, const Vec3 &b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline float getEuclideanDistance(const Vec3 &a, const Vec3 &b)
{
	return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
}

Vec3 cross(const Vec3 &a, const Vec3 &b)
{
	return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

Vec3 colorModulate(const Vec3 &lightColor, const Vec3 &objectColor) // performs component wise multiplication for colors  
{
	return Vec3(lightColor.x * objectColor.x, lightColor.y * objectColor.y, lightColor.z * objectColor.z);
}

void clamp(Vec3 &col)
{
	col.x = (col.x > 1) ? 1 : (col.x < 0) ? 0 : col.x;
	col.y = (col.y > 1) ? 1 : (col.y < 0) ? 0 : col.y;
	col.z = (col.z > 1) ? 1 : (col.z < 0) ? 0 : col.z;
}

void initVec3Batch(Vec3Cluster4 &vec3Batch)
{
	for (int i = 0; i < 4; i++)
		vec3Batch.x[i] = vec3Batch.y[i] = vec3Batch.z[i] = 0.0f;
}

void initVec3Batch(Vec3Cluster4 &vec3Batch, float x, float y, float z)
{
	for (int i = 0; i < 4; i++)
	{
		vec3Batch.x[i] = x;
		vec3Batch.y[i] = y;
		vec3Batch.z[i] = z;
	}
}

void initVec3Batch(Vec3Cluster4 &vec3Batch, const Vec3 &v)
{
	for (int i = 0; i < 4; i++)
	{
		vec3Batch.x[i] = v.x;
		vec3Batch.y[i] = v.y;
		vec3Batch.z[i] = v.z;
	}
}

void updateVec3Batch(Vec3Cluster4 &vec3Batch, int index, const Vec3 &v)
{
	vec3Batch.x[index] = v.x;
	vec3Batch.y[index] = v.y;
	vec3Batch.z[index] = v.z;
}

Vec3 getVec3BatchData(const Vec3Cluster4 &vec3Batch, int index)
{
	return Vec3(vec3Batch.x[index], vec3Batch.y[index], vec3Batch.z[index]);
}

void initRayBatch(RayCluster4 &rayBatch)
{
	for (int i = 0; i < 4; i++)
	{
		rayBatch.ox[i] = rayBatch.oy[i] = rayBatch.oz[i] = 0;
		rayBatch.dx[i] = rayBatch.dy[i] = rayBatch.dz[i] = 0;
		rayBatch.t[i] = INT_MAX;
		rayBatch.tMin[i] = 0.01;
		rayBatch.tMax[i] = INT_MAX;
		rayBatch.hasHit[i] = 0;
	}
}

void initRayBatch(RayCluster4 &rayBatch,
					float ox,
					float oy,
					float oz,
					float dx,
					float dy,
					float dz)
{
	for (int i = 0; i < 4; i++)
	{
		rayBatch.ox[i] = ox;
		rayBatch.oy[i] = oy;
		rayBatch.oz[i] = oz;
		rayBatch.dx[i] = dx;
		rayBatch.dy[i] = dy;
		rayBatch.dz[i] = dz;		
		
		rayBatch.t[i] = INT_MAX;
		rayBatch.tMin[i] = 0.01;
		rayBatch.tMax[i] = INT_MAX;
		rayBatch.hasHit[i] = 0;
	}
}

void initRayBatch(RayCluster4 &rayBatch,
					float ox[4],
					float oy[4],
					float oz[4],
					float dx[4],
					float dy[4],
					float dz[4])
{
	for (int i = 0; i < 4; i++)
	{
		rayBatch.ox[i] = ox[i];
		rayBatch.oy[i] = oy[i];
		rayBatch.oz[i] = oz[i];
		rayBatch.dx[i] = dx[i];
		rayBatch.dy[i] = dy[i];
		rayBatch.dz[i] = dz[i];		
		
		rayBatch.t[i] = INT_MAX;
		rayBatch.tMin[i] = 0.01;
		rayBatch.tMax[i] = INT_MAX;
		rayBatch.hasHit[i] = 0;
	}
}

Ray getRayBatchData(const RayCluster4 &rayBatch, int index)
{
	Vec3 o(rayBatch.ox[index], rayBatch.oy[index], rayBatch.oz[index]);
	Vec3 d(rayBatch.dx[index], rayBatch.dy[index], rayBatch.dz[index]);
	Ray r(o, d);
	r.tMin = rayBatch.tMin[index];
	r.t = rayBatch.t[index];
	r.tMax = rayBatch.tMax[index];
	return r;
}

void initRayBatch(RayCluster4 &rayBatch, const Ray &r0, const Ray &r1, const Ray &r2, const Ray &r3)
{
	rayBatch.ox[0] = r0.o.x;
	rayBatch.oy[0] = r0.o.y;
	rayBatch.oz[0] = r0.o.z;
	rayBatch.dx[0] = r0.d.x;
	rayBatch.dy[0] = r0.d.y;
	rayBatch.dz[0] = r0.d.z;
	
	rayBatch.ox[1] = r1.o.x;
	rayBatch.oy[1] = r1.o.y;
	rayBatch.oz[1] = r1.o.z;
	rayBatch.dx[1] = r1.d.x;
	rayBatch.dy[1] = r1.d.y;
	rayBatch.dz[1] = r1.d.z;
	
	rayBatch.ox[2] = r2.o.x;
	rayBatch.oy[2] = r2.o.y;
	rayBatch.oz[2] = r2.o.z;
	rayBatch.dx[2] = r2.d.x;
	rayBatch.dy[2] = r2.d.y;
	rayBatch.dz[2] = r2.d.z;
	
	rayBatch.ox[3] = r3.o.x;
	rayBatch.oy[3] = r3.o.y;
	rayBatch.oz[3] = r3.o.z;
	rayBatch.dx[3] = r3.d.x;
	rayBatch.dy[3] = r3.d.y;
	rayBatch.dz[3] = r3.d.z;
	
	for (int i = 0; i < 4; i++)
	{
		rayBatch.t[i] = INT_MAX;
		rayBatch.tMin[i] = 0.01;
		rayBatch.tMax[i] = INT_MAX;
		rayBatch.hasHit[i] = 0;
	}	
}

Vec3Cluster4 getPixelColorBatch(RayCluster4 &cameraRayBatch, const std::vector<Geometry*> &scene, const Light *light)
{
    Vec3 ambient(0.25, 0, 0);	// light red ambient light
	float ambientIntensity = 0.25;
	Vec3 bgColor = ambient * ambientIntensity;
	Vec3 outColor(bgColor);	
	Vec3 black;

	int i = 0;
	int hitIndex[4] = {-1};
	
	for (auto &geo : scene)
	{
		geo->intersectsBatch(cameraRayBatch);
		for (int j = 0; j < 4; j++) // loop through ray batch
		{
			if (cameraRayBatch.hasHit[j])
				hitIndex[j] = i;
		}
        i++; // enumerates geometry
	}	
	
	Vec3Cluster4 pixelColor4;
	initVec3Batch(pixelColor4, black);
	for (int k = 0; k < 4; k++) // loop through ray batch
	{		
		if (cameraRayBatch.hasHit[k])
		{
			Ray cameraRay = getRayBatchData(cameraRayBatch, k);
			Vec3 surf = cameraRay.o + cameraRay.d * cameraRay.tMax; // point of intersection
			Vec3 L = (light->position - surf).getNormalized();
			bool isOccluded = false;
			
			outColor = scene[hitIndex[k]]->color;
			updateVec3Batch(pixelColor4, k, outColor);
			
			/*
			
			// check for shadows
			Ray shadowRay(surf, L);
			for (auto &geo : scene)
				if (geo->intersects(shadowRay))
					isOccluded = true;
			
			if (isOccluded)
				updateVec3Batch(pixelColor4, k, outColor);
			else
			{
				Vec3 N = scene[hitIndex[k]]->getNormal(surf).getNormalized();
				float diffuse = L.dot(N);
				outColor = (colorModulate(light->color, scene[hitIndex[k]]->color) * diffuse) * light->intensity;
				clamp(outColor);
				updateVec3Batch(pixelColor4, k, outColor);
			}
			*/			
		}
	}
	return pixelColor4;
}

void renderSIMD(Vec3 *fb,
				Light *light,
				const std::vector<Geometry*> &scene,			
				int nThreads,
				const Camera &camera,
				int width,
				int height,
				bool isBenchmark,
				int nBenchLoops)
{
	for (int run = 0; run < nBenchLoops; run++)
    {		
#pragma omp parallel num_threads(nThreads) shared(fb)
		{
#pragma omp for schedule(dynamic, 1)
			for(int y = 0; y < height; y++)
			{
				int yw = y * width;
				
				for(int x = 0; x < width; x += 4)
				{
					Vec3Cluster4 pixelColor4;
					
					Ray cameraRay0(Vec3(x, y, 0), camera.direction); // camera ray from each pixel 
					Ray cameraRay1(Vec3(x + 1, y, 0), camera.direction); // camera ray from each pixel 
					Ray cameraRay2(Vec3(x + 2, y, 0), camera.direction); // camera ray from each pixel 
					Ray cameraRay3(Vec3(x + 3, y, 0), camera.direction); // camera ray from each pixel 
					
					RayCluster4 rayBatch;
					initRayBatch(rayBatch, cameraRay0, cameraRay1, cameraRay2, cameraRay3);
				
					// int index = y * width + x;			
					int index0 = yw + x; // y * width + x
					int index1 = yw + x + 1; // y * width + (x + 1)
					int index2 = yw + x + 2; // y * width + (x + 2)
					int index3 = yw + x + 3; // y * width + (x + 3)							
					
					pixelColor4 = getPixelColorBatch(rayBatch, scene, light);	
					
					fb[index0] = getVec3BatchData(pixelColor4, 0);	
					fb[index1] = getVec3BatchData(pixelColor4, 1);	
					fb[index2] = getVec3BatchData(pixelColor4, 2);
					fb[index3] = getVec3BatchData(pixelColor4, 3);
				}
			}
		}
    }
}

Vec3 getPixelColor(Ray &cameraRay, const std::vector<Geometry*> &scene, const Light *light)
{
    Vec3 ambient(0.25, 0, 0);	// light red ambient light
	double ambientIntensity = 0.25;
	Vec3 pixelColor; // = ambient * ambientIntensity;
	bool hitStatus = false;
	int hitIndex = 0, i = 0;
	
	for (auto &geo : scene)
	{
		if (geo->intersects(cameraRay))
		{
			hitStatus = true;		
			hitIndex = i;
		}
        i++;
	}

	if (hitStatus)
	{
		Vec3 surf = cameraRay.o + cameraRay.d * cameraRay.tMax; // point of intersection
		Vec3 L = (light->position - surf).getNormalized();

		// check for shadows
		Ray shadowRay(surf, L);
		for (auto &geo : scene)
			if (geo->intersects(shadowRay))
				return pixelColor;

		Vec3 N = scene[hitIndex]->getNormal(surf).getNormalized();
		float diffuse = L.dot(N);
		pixelColor = (colorModulate(light->color, scene[hitIndex]->color) * diffuse) * light->intensity;
		clamp(pixelColor);
	}

	return pixelColor;
}

void render(Vec3 *fb,
				Light *light,
				const std::vector<Geometry*> &scene,			
				int nThreads,
				const Camera &camera,
				int width,
				int height,
				bool isBenchmark,
				int nBenchLoops)
{
	for (int run = 0; run < nBenchLoops; run++)
    {
#pragma omp parallel for num_threads(nThreads) shared(fb) schedule(dynamic, 1) 
        for(int y = 0; y < height; y++)
        {
            for(int x = 0; x < width; x++)
            {
                size_t index = y * width + x;
                Ray cameraRay(Vec3(x, y, 0), camera.direction); // camera ray from each pixel 
				
                fb[index] = getPixelColor(cameraRay, scene, light);			
            }
        }
    }	
}

void saveImg(Vec3 *frameBuffer, int width, int height)
{
	int bufferSize = width * height;
	std::cout << "Saving render output..." << std::endl;

	const char *outputPath = "result.ppm";

	std::ofstream out(outputPath); // creates a PPM image file for saving the rendered output
	out << "P3\n" << width << " " << height << "\n255\n";

	for (uint32_t i = 0; i < bufferSize; i++)
		out << static_cast<int>(frameBuffer[i].x * 255.99) << " " << static_cast<int>(frameBuffer[i].y * 255.99) << " " << static_cast<int>(frameBuffer[i].z * 255.99) << "\n";
	
	std::cout << "Render output saved successfully.\n";
}

void cleanup(Vec3 **frameBuffer, Light **light, std::vector<Geometry*> &scene)
{
	delete [] *frameBuffer;
	delete *light;
	for (auto &geo : scene)
		delete geo;
}

int main(int argc, char* argv[])
{
    // colors (R, G, B)
	const Vec3 white(1, 1, 1);
	const Vec3 black(0, 0, 0);
	const Vec3 red(1, 0, 0);
	const Vec3 green(0, 1, 0);
	const Vec3 blue(0, 0, 1);
	const Vec3 cyan(0, 1, 1);
	const Vec3 magenta(1, 0, 1);
	const Vec3 yellow(1, 1, 0);

    // setup multithreading and benchmark parameters

    int nBenchLoops = 1; 
    int nThreads = 1;
    bool isBenchmark = false;
	bool useSIMD = false;
	int height = 1080;
	int width = 1920;

    for (int i = 0; i < argc; i++) // process command line args
    {
        if (!strcmp(argv[i], "-mt")) // enables multithreading using OpenMP
        {
            std::cout << "Multithreaded rendering enabled.\n";
            nThreads = std::thread::hardware_concurrency();
        }
		
		if (!strcmp(argv[i], "-simd")) 
        {
            std::cout << "SIMD processing enabled.\n";
            useSIMD = true;
        }
		
		if (!strcmp(argv[i], "-width")) // usage -width <width>
        {
            if (i + 1 < argc)
                width = atoi(argv[i+1]); 
            else
			{
				height = 1080;
				width = 1920;
                std::cout << "Resolution width value not provided. Using default value of 1920x1080 pixels.\n"; 
			}				
        }
		
		if (!strcmp(argv[i], "-height")) // usage -height <height>
        {
            if (i + 1 < argc)
                height = atoi(argv[i+1]); 
            else
			{
				height = 1080;
				width = 1920;
                std::cout << "Resolution height value not provided. Using default value of 1920x1080 pixels.\n";      
			}				
        }

        if (!strcmp(argv[i], "-bench")) // usage -bench <numberLoops>
        {
            isBenchmark = true;
            if (i + 1 < argc)
                nBenchLoops = atoi(argv[i+1]); // number of times to loop in benchmark mode
            else
            {
                std::cout << "Benchmark loop count not provided.\n";
                nBenchLoops = 5;
            }            
        }
    } 

	// setup camera, colors, objects and lights		
    const int fbSize = height * width;
    Vec3 *fb = new Vec3[fbSize]; 

    // scene objects and lights
    std::vector<Geometry*> scene;
    
    scene.push_back(new Sphere(Vec3(0.5 * width, 0.45 * height, 1000), 100, Vec3(1, 0, 0)));
    scene.push_back(new Sphere(Vec3(0.65 * width, 0.2 * height, 600), 50, Vec3(0, 0, 1)));
    // scene.push_back(new Plane(Vec3(0, 0, -1), Vec3(0.5 * width, 0.5 * height, 1500), Vec3(1, 1, 0)));
    scene.push_back(new Sphere(Vec3(0.5 * width, 0.52 * height, 700), 35, Vec3(0, 1, 1)));
	
	const Camera camera(Vec3(0.5 * width, 0.5 * height, 0), Vec3(0, 0, 1)); // scene camera	
	Light *light = new Light(Vec3(0.8 * width, 0.25 * height, 100), 1, white, 0.75); // white scene light		    		
	
    if (isBenchmark)
        std::cout << "Running in benchmark mode. Looping " << nBenchLoops << " times.\n";

    std::cout << "Rendering...\n";
	auto start = std::chrono::high_resolution_clock::now();

	if (useSIMD)
		renderSIMD(fb, light, scene, nThreads, camera, width, height, isBenchmark, nBenchLoops);
	else
		render(fb, light, scene, nThreads, camera, width, height, isBenchmark, nBenchLoops);

	auto stop = std::chrono::high_resolution_clock::now(); 
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    std::cout << "Rendering finished successfully.\n";
    std::cout << "\nTime taken to render was " << diff.count() << " milliseconds." << std::endl; 

	if (!isBenchmark)
		saveImg(fb, width, height);
    
	cleanup(&fb, &light, scene);
}