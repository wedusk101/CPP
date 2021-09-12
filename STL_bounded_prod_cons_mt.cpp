/*The following code simulates the *Bounded* Producer-Consumer (supporting arbitrary number of producers
and consumers) problem using Standard Library threads. A producer thread produces a shared resource and
then sleeps for a second. The consumer thread consumes the resource and then sleeps for a second. How
much of the resource is produced or consumed is dictated by a pseudo-random number generator subroutine.

Compile using flags to enable C++11 and link to pthread if necessary.
On GCC this is done using the flags -std=c++11 -pthread passed to the compiler*/

#include <iostream>
#include <thread>
#include <mutex>
#include <random>
#include <chrono>
#include <atomic>
#include <condition_variable>

const int nProd = 4;
const int nCon = 4;
const int MAXSIZE = 10;

std::mutex resourceMutex;
std::atomic<int> resource(1); // simulates the shared buffer of produced resource
std::condition_variable cv;

size_t getRandUnitSize(std::default_random_engine &seed)
{
	std::uniform_real_distribution<double> rnd(0.0, 1.0);
	double trial = rnd(seed);
	return static_cast<size_t>(trial * MAXSIZE);
}

void produce(int id)
{
	size_t tid = std::hash<std::thread::id>()(std::this_thread::get_id());
	
	// seeds the rng using the thread id and current time
	std::default_random_engine seed(tid * 
									static_cast<uint64_t>
									(std::chrono::system_clock::to_time_t
									((std::chrono::system_clock::now())))); 
	
	while (true)
	{
		std::unique_lock<std::mutex> lock(resourceMutex);
		std::cout << "Lock acquired by producer " << id << " ... " << std::endl;
		size_t units = 0;
		
		cv.wait(lock, [&]()
		{
			units = getRandUnitSize(seed);
			std::cout << "Available: " << resource << std::endl;
			std::cout << "Units to produce: " << units << std::endl;
			int newAmount = resource.load() + units;
			std::cout << "Projected amount after production: " << newAmount << std::endl;
			bool predicate = (newAmount <= MAXSIZE);
			if (!predicate)
			{
				std::cout << "Produced resource limit reached. Sleeping...\n\n" << std::endl;
				cv.notify_one();
			}
			return predicate;
		});		
		
		resource += units;
		std::cout << "Produced " << units << " units." << std::endl;
		std::cout << "Total: " << resource << "\n\n" << std::endl;
		cv.notify_one();
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

void consume(int id)
{
	size_t tid = std::hash<std::thread::id>()(std::this_thread::get_id());
	
	// seeds the rng using the thread id and current time
	std::default_random_engine seed(tid * 
									static_cast<uint64_t>
									(std::chrono::system_clock::to_time_t
									((std::chrono::system_clock::now())))); 
	
	while (true)
	{
		std::unique_lock<std::mutex> lock(resourceMutex);
		std::cout << "Lock acquired by consumer " << id << " ... " << std::endl;
		size_t units = 0;
		
		cv.wait(lock, [&]()
		{
			units = getRandUnitSize(seed);
			std::cout << "Available: " << resource << std::endl;
			std::cout << "Units to consume: " << units << std::endl;
			int newAmount = resource.load() - units;
			std::cout << "Projected amount after consumption: " << newAmount << std::endl;
			bool predicate = (newAmount >= 0);
			if (!predicate)
			{
				std::cout << "Not enough resources to consume. Sleeping...\n\n" << std::endl;
				cv.notify_one();
			}
			return predicate;
		});			

		resource -= units;
		std::cout << "Consumed " << units << " units." << std::endl;
		std::cout << "Total: " << resource << "\n\n" << std::endl;
		cv.notify_one();
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}


int main()
{
	std::thread producers[nProd];
	std::thread consumers[nCon];
	
	for (int i = 0; i < nProd; ++i)
		producers[i] = std::thread(produce, i);
	
	for (int i = 0; i < nProd; ++i)
		consumers[i] = std::thread(consume, i);
	
	for (int i = 0; i < nProd; ++i)
		producers[i].join();
	
	for (int i = 0; i < nProd; ++i)
		consumers[i].join();
}