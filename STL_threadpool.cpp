#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>
#include <string>
#include <functional>
#include <atomic>
#include <random>

const int nProd = 2;
const int nCon = 2;

size_t nProduced = 0;
size_t nConsumed = 0;

std::mutex streamMutex;
bool shuttingDown = false;
bool paused = false;
bool resumed = false;

void produce(int id)
{
	if (resumed)
		std::cout << "Resuming producer...\n";
		
	while (!shuttingDown && !paused)
	{
		++nProduced; 
		std::unique_lock<std::mutex> lock(streamMutex);
		std::cout << "Thread " << id << " producing..." << std::endl;
		std::cout << "Units produced: " << nProduced << std::endl;
		lock.unlock();
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	
	if (paused)
		std::cout << "Pausing producer...\n";
}

void consume(int id)
{
	if (resumed)
		std::cout << "Resuming consumer...\n";
		
	while (!shuttingDown && !paused)
	{
		++nConsumed;
		std::unique_lock<std::mutex> lock(streamMutex);
		std::cout << "Thread " << id << " consuming..." << std::endl;
		std::cout << "Units consumed: " << nConsumed << std::endl;
		lock.unlock();
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	
	if (paused)
		std::cout << "Pausing consumer...\n";
}

struct ITask
{
	enum struct TaskState
	{
		Uninitialized,
		Waiting,
		Running,
		Paused,
		Finished
	};
	
	std::string getState(TaskState state) const
	{
		switch (state)
		{
			case (TaskState::Uninitialized):
				return "Uninitialized";
				
			case (TaskState::Waiting):
				return "Waiting";
				
			case (TaskState::Running):
				return "Running";
				
			case (TaskState::Paused):
				return "Paused";
				
			case (TaskState::Finished):
				return "Finished";
				
			default:
				return "Unknown state";
		}	
	}		
};

template
<typename T>
struct Task : public ITask
{
	TaskState state;
	std::function<T()> job;
	
	Task(std::function<T()> &job_) :
		state(TaskState::Uninitialized),
		job(std::move(job_))
	{
	}
	
	T run()
	{
		state = TaskState::Running;
		std::cout << "Running..." << std::endl;
		return job();
	}
};

class ThreadPool
{
	
public:
	ThreadPool() : 
		poolSize(std::thread::hardware_concurrency() + 2),
		nThreads(std::thread::hardware_concurrency()),
		numFinishedThreads(0),
		isStarted(false),
		isPaused(false),
		isShuttingDown(false)
	{
	}
	
	~ThreadPool()
	{
		while (!isShuttingDown);
		
		waitCV.notify_all();
		for (size_t i = 0; i < pool.size(); ++i)
			pool[i].join();
	}
	
	void init()
	{
		for (size_t i = 0; i < poolSize; ++i)
		{
			std::thread worker([&, this]() { wait(); });
			pool.push_back(std::move(worker));
		}
	}
	
	void start()
	{
		isStarted = true;
		isPaused = false;
		std::cout << "Execution started...\n";
		waitCV.notify_all();
	}		
	
	void pause()
	{
		isPaused = true;
		paused = true;
		waitCV.notify_all();
	}
	
	void resume()
	{
		isPaused = false;
		paused = false;
		resumed = true;
		waitCV.notify_all();
	}
	
	void shutdown()
	{
		isShuttingDown = true;
		
		while (numFinishedThreads != poolSize)
			waitCV.notify_all();
	}	
	
	bool isQueueEmpty()
	{
		std::unique_lock<std::mutex> lock(queueMutex);
		return taskQueue.empty();
	}
	
	bool resumedTasksAvailable()
	{
		std::unique_lock<std::mutex> lock(queueMutex);
		return reentrantQueue.empty();
	}
	
	bool tasksAvailable()
	{
		return !isQueueEmpty() || resumedTasksAvailable();
	}
	
	void addTask(std::unique_ptr<ITask> &&task)
	{
		std::unique_lock<std::mutex> lock(queueMutex);
		taskQueue.push(std::move(task));
	}	
	
private:
	void wait()
	{
		std::unique_ptr<ITask> job;
		
		while (true)
		{
			if (isShuttingDown)
			{
				++numFinishedThreads;
				shuttingDown = true;
				break;
			}
			else if (!isQueueEmpty() && isStarted && !isPaused)
			{
				std::unique_lock<std::mutex> lock(queueMutex);
				job = std::move(taskQueue.front());
				taskQueue.pop();
				lock.unlock();
				Task<void>* t = static_cast<Task<void>*>(job.get());
				t->run();
					
				if (isPaused)
					reentrantQueue.push(std::move(job));				
			} /*
			else if (resumedTasksAvailable() && !isPaused)
			{
				std::unique_lock<std::mutex> lock(queueMutex);
				job = std::move(reentrantQueue.front());				
				reentrantQueue.pop();
				lock.unlock();
				Task<void>* resumedTask = static_cast<Task<void>*>(job.get());
				resumedTask->run();
					
				if (isPaused)
					reentrantQueue.push(std::move(job));
			} */
			else
			{
				std::unique_lock<std::mutex> lock(waitMutex);
				waitCV.wait(lock, [&, this]()
				{
					return !isQueueEmpty() || isShuttingDown || !isPaused;
				});
			}
		}
	}

	const size_t poolSize;
	const size_t nThreads;	
	
	std::vector<std::thread> pool;
	std::queue<std::unique_ptr<ITask>> taskQueue;
	std::queue<std::unique_ptr<ITask>> reentrantQueue;
	// std::queue<Task<void>*> reentrantQueue;
	
	std::mutex waitMutex;
	std::mutex queueMutex;
	
	std::condition_variable waitCV;
	
	std::atomic<int> numFinishedThreads;
	
	bool isStarted;
	bool isPaused;	
	bool isShuttingDown;	
};

int main()
{
	std::queue<int> eventQueue;
	ThreadPool threadPool;
	threadPool.init();
	int event = 0, input = 0, tmp = 0;	
	
	for (int i = 0; i < nProd; ++i)
	{
		std::function<void()> prodfn(std::bind(&produce, i));
		std::unique_ptr<ITask> prodTask = std::make_unique<Task<void>>(std::ref(prodfn));
		threadPool.addTask(std::move(prodTask));
	}
		
	for (int i = 0; i < nCon; ++i)
	{
		std::function<void()> consfn(std::bind(&consume, i));
		std::unique_ptr<ITask> consTask = std::make_unique<Task<void>>(std::ref(consfn));
		threadPool.addTask(std::move(consTask));
	}
	
	threadPool.start();	
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << "Pause/Resume/Quit ? (5/6/7)\n";
		std::cin >> input;
		std::cout << "Input event: " << input << "\n";
		eventQueue.push(input);
		
		if (!eventQueue.empty())
		{		
			event = std::move(eventQueue.front());
			std::cout << "Event to be processed: " << event << std::endl;
			eventQueue.pop();
		}
		
		if (event == 5) // 5 to pause
		{
			threadPool.pause();
			continue;
		}
		
		if (event == 6) // 6 to resume
		{
			threadPool.resume();		
			continue;
		}
			
		if (event == 7) // 7 to quit
			break;		
	}
	std::cout << "Shutting down...\n";
	threadPool.shutdown();
	return 0;
}