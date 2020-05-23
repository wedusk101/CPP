/*The following code simulates the *Unbounded* Producer-Consumer problem using Standard Library threads. A producer thread produces a shared resource and then sleeps for a second.
The consumer thread consumes the resource and then sleeps for a second. How much of the resource is produced or consumed is dictated by a pseudo-random subroutine.*/
#include <iostream>
#include <thread>
#include <mutex>
#include <random>
#include <chrono>
#include <atomic>

std::mutex resourceMutex;
std::atomic<int> resource(10);

size_t toss(std::default_random_engine &seed)
{
	std::uniform_real_distribution<double> rnd(0.0, 1.0);
	double trial = rnd(seed);
	return static_cast<size_t>(trial * 10);
}


void produce()
{
	size_t tid = std::hash<std::thread::id>()(std::this_thread::get_id());
	std::default_random_engine seed(tid + static_cast<uint64_t>(std::chrono::system_clock::to_time_t((std::chrono::system_clock::now()))));
	while (resource > 0)
	{
		std::unique_lock<std::mutex> lock(resourceMutex);
		size_t units = toss(seed);
		resource += units;
		std::cout << "Produced " << units << " units." << std::endl;
		std::cout << "Total: " << resource << "\n\n" << std::endl;
		lock.unlock();
		
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

void consume()
{
	size_t tid = std::hash<std::thread::id>()(std::this_thread::get_id());
	std::default_random_engine seed(tid + static_cast<uint64_t>(std::chrono::system_clock::to_time_t((std::chrono::system_clock::now()))));
	while (resource > 0)
	{
		std::unique_lock<std::mutex> lock(resourceMutex);
		size_t units = toss(seed);
		resource -= units;
		std::cout << "Consumed " << units << " units." << std::endl;
		std::cout << "Total: " << resource << "\n\n" << std::endl;
		lock.unlock();
		
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}


int main()
{
	std::thread producer(produce);
	std::thread consumer(consume);
	
	producer.join();
	consumer.join();
}