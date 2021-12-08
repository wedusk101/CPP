/*

The following piece of code presents a toy example of a thread pool based task system with the ability to pause
and resume tasks. The purpose of this program is to demonstrate a working albeit simple example of an event loop
along with the task system that can operate on arbitrary functions as per the users choice.

Here is the general algorithm of the task system (ThreadPool class):

[1] Initialize the thread pool and wait for tasks.
[2] User adds tasks to the task queue of the thread pool and notifies the task system.
[3] The waiting threads check periodically for available tasks.
[4] If a task (new or partially completed) is available in the queue, they take the task and begin execution.
[5] If the task execution finishes, the thread goes to the waiting state again.
[6] If the task execution was interrupted preemptively by the user, the task is moved to the queue of interrupted tasks.
[7] After step [6], the thread goes to the waiting state again.
[8] If the user chooses to shutdown the task system, the tasks are interrupted and the threads are joined.

We have a threadpool that spawns N + 2 threads where N is the number of hardware threads available on the system.
We can add any number of task functions to the queue of the threadpool. If there are more tasks available than 
there are free threads available in the pool, the remaining tasks wait in the queue until the current batch of
tasks finish execution.

Apart from the ThreadPool class, there is also a simple event loop to handle concurrent user input. This event
loop runs on a separate thread and queries the event queue from time to time to process any valid input event.
It is responsible for signaling the task system to start, pause, resume or shutdown the execution of tasks.
In a real system, these operations can be replaced by anything else like a mouse click or an incoming network
packet etc. Since this technically requires a thread to wait on multiple condition variables (one for the thread
pool and potentially one for the task callback function, this gets a bit tricky. On Windows this is natively allowed
but it becomes harder on Linux. Here we solve the problem by simulating a state machine using multiple shared boolean
variables that affect the execution state of the callback functions.

We have a couple of functions that represent a producer and a consumer, but these can be replaced with any compute
heavy task. They share a resource variable (the shared buffer) that represents the execution state of the functions at 
any point in time. Instead of waiting on condition variables, the worker threads wait on aquiring the lock on the 
shared buffer in a spinlock. When execution is paused by the user and then later resumed, execution resumes from the
point where it was interrupted. The atomic variable "resource", therefores also acts as a checkpoint for this resume
operation. In a real system, such variables will be replaced by whatever state the application has that needs to be
persisted across cycles of pause/resume operations. For e.g. a time point in a music player playing an MP3. This
execution state could theorectically be serialized along with the queue of saved tasks so that the resume operation
can be achieved even across different sessions of the application. This behavior is called re-entrant programming.
The demo presented here should act as a starting design guide for implementing the ideas in a more complex real
world application.

To compile the code using GCC, use the flags "-pthread" and "-std=c++14", to link the POSIX threads library and 
enable C++14 standard respectively.

*/

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
#include <vector>

// no. of threads to be used for producers and consumers
const int nProd = 4;
const int nCon = 4;

std::atomic<int> resource(1); // simulates the shared buffer of produced resource
const int MAXSIZE = 10; // maximum value of the buffer
std::mutex resourceMutex;

bool shuttingDown = false;
bool paused = false;
bool resumed = false;

// returns a random unit size in (0, MAXSIZE) to produce or consume
size_t getRandUnitSize(std::default_random_engine &seed)
{
	std::uniform_real_distribution<double> rnd(0.0, 1.0);
	double trial = rnd(seed);
	return static_cast<size_t>(trial * MAXSIZE);
}

// producer subroutine
void produce(int id)
{
	if (resumed) // there's a bug here. This should be done for each task using it's own TaskState
		std::cout << "Resuming producer...\n";
		
	size_t tid = std::hash<std::thread::id>()(std::this_thread::get_id());
	
	// seeds the rng using the thread id and current time
	std::default_random_engine seed(tid * 
									static_cast<uint64_t>
									(std::chrono::system_clock::to_time_t
									((std::chrono::system_clock::now()))));
		
	while (!shuttingDown && !paused)
	{
		std::unique_lock<std::mutex> lock(resourceMutex);
		std::cout << "Lock acquired by producer " << id << " ... " << std::endl;
		size_t units = 0;
		 
		units = getRandUnitSize(seed); // produce a random number of units
		std::cout << "Available: " << resource << std::endl;
		std::cout << "Units to produce: " << units << std::endl;
		int newAmount = resource.load() + units;
		std::cout << "Projected amount after production: " << newAmount << std::endl;
		
		bool predicate = (newAmount <= MAXSIZE);		
		if (!predicate)
		{
			std::cout << "Produced resource limit reached. Sleeping...\n\n" << std::endl;
			lock.unlock();
			std::this_thread::sleep_for(std::chrono::seconds(1));
			continue;
		}
		
		resource += units;
		std::cout << "Produced " << units << " units." << std::endl;
		std::cout << "Total: " << resource << "\n\n" << std::endl;
		lock.unlock();		
	}
	
	if (paused) // there's a bug here. This should be done for each task using it's own TaskState
		std::cout << "Pausing producer...\n";
}

// consumer subroutine
void consume(int id)
{
	if (resumed) // there's a bug here. This should be done for each task using its own TaskState
		std::cout << "Resuming consumer...\n";
		
	size_t tid = std::hash<std::thread::id>()(std::this_thread::get_id());
	
	// seeds the rng using the thread id and current time
	std::default_random_engine seed(tid * 
									static_cast<uint64_t>
									(std::chrono::system_clock::to_time_t
									((std::chrono::system_clock::now()))));
		
	while (!shuttingDown && !paused)
	{
		std::unique_lock<std::mutex> lock(resourceMutex);
		std::cout << "Lock acquired by consumer " << id << " ... " << std::endl;
		size_t units = 0;
		
		units = getRandUnitSize(seed); // consume a random number of units
		std::cout << "Available: " << resource << std::endl;
		std::cout << "Units to consume: " << units << std::endl;
		int newAmount = resource.load() - units;
		std::cout << "Projected amount after consumption: " << newAmount << std::endl;
		
		bool predicate = (newAmount >= 0);
		if (!predicate)
		{
			std::cout << "Not enough resources to consume. Sleeping...\n\n" << std::endl;
			lock.unlock();
			std::this_thread::sleep_for(std::chrono::seconds(1));
			continue;
		}

		resource -= units;
		std::cout << "Consumed " << units << " units." << std::endl;
		std::cout << "Total: " << resource << "\n\n" << std::endl;
		lock.unlock();		
	}
	
	if (paused) // there's a bug here. This should be done for each task using its own TaskState
		std::cout << "Pausing consumer...\n";
}

// Wrapper Task interface
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

// Wrapper for the callback function that represents the task to be executed
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
	
	// creates threads and puts them in waiting state
	void init()
	{
		for (size_t i = 0; i < poolSize; ++i)
		{
			std::thread worker([&, this]() { wait(); });
			pool.push_back(std::move(worker));
		}
	}
	
	// starts the execution of tasks present in the task queue
	void start()
	{
		if (isStarted)
		{
			std::cout << "Task execution has already started. Skipping operation...\n";
			return;
		}
		
		isStarted = true;
		isPaused = false;
		std::cout << "Execution started...\n";
		waitCV.notify_all();
	}		
	
	// pauses all execution of tasks by putting the threads in the waiting state
	void pause()
	{
		if (!isStarted)
		{
			std::cout << "Task execution has not been started yet. Invalid operation!\n";
			return;
		}
		
		isPaused = true;
		paused = true;
		resumed = false;
		waitCV.notify_all();
	}
	
	// resumes execution of tasks
	void resume()
	{
		if (!isStarted)
		{
			std::cout << "Task execution has not been started yet. Invalid operation!\n";
			return;
		}
		
		if (isPaused)
		{
			isPaused = false;
			paused = false;
			resumed = true;
			waitCV.notify_all();
		}
	}
	
	// stops all execution of tasks and shuts down the threadpool
	void shutdown()
	{
		isShuttingDown = true;
		shuttingDown = true;
		
		while (numFinishedThreads != poolSize)
			waitCV.notify_all();
	}	
	
	bool isQueueEmpty()
	{
		std::unique_lock<std::mutex> lock(queueMutex);
		return taskQueue.empty();
	}
	
	// checks if there are partially finished but saved tasks in the queue
	bool resumedTasksAvailable()
	{
		std::unique_lock<std::mutex> lock(queueMutex);
		return !reentrantQueue.empty();
	}
	
	// checks if there are new or partially finished (saved) tasks in the queue
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

	// the primary function for orchestrating the task system
	void wait()
	{
		std::unique_ptr<ITask> job;
		
		while (true)
		{
			if (isShuttingDown)
			{
				++numFinishedThreads;				
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
				
				// save the partially finished task in 
				// case it was paused 				
				if (isPaused)
					reentrantQueue.push(std::move(job));				
			}
			else if (resumedTasksAvailable() && !isPaused)
			{
				std::unique_lock<std::mutex> lock(queueMutex);
				job = std::move(reentrantQueue.front());				
				reentrantQueue.pop();
				lock.unlock();
				Task<void>* resumedTask = static_cast<Task<void>*>(job.get());
				resumedTask->run();
				
				// save the remaining part of the partially finished
				// task in case it was paused
				if (isPaused)
					reentrantQueue.push(std::move(job));
			}
			else // go to waiting state
			{
				std::unique_lock<std::mutex> lock(waitMutex);
				waitCV.wait(lock, [&, this]()
				{
					// conditions for waking up the threads
					return !isPaused && (!isQueueEmpty() || resumedTasksAvailable()) || isShuttingDown;
				});
			}
		}
	}

	const size_t poolSize;
	const size_t nThreads;	
	
	std::vector<std::thread> pool;
	std::queue<std::unique_ptr<ITask>> taskQueue;
	std::queue<std::unique_ptr<ITask>> reentrantQueue;
	
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
	
	// event loop for handling user input
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << "Start/Pause/Resume/Quit ? (4/5/6/7)\n";
		std::cin >> input;
		std::cout << "Input event: " << input << "\n";
		eventQueue.push(input);
		
		if (!eventQueue.empty())
		{		
			event = std::move(eventQueue.front());
			std::cout << "Event to be processed: " << event << std::endl;
			eventQueue.pop();
		}
		
		if (event == 4) // 4 to start
		{
			threadPool.start();	
			continue;
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