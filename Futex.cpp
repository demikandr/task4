#include <iostream>
#include <cstdlib>
#include <thread>
#include <vector>
#include <algorithm>
#include <atomic>
#include <assert.h>
#include <mutex>
#include "Futex.h"

FuFutex mutex;
//std::mutex mutex;
std::vector<int> threads_rates(4);

void increment(volatile int& var, const int thread_index) {
	while(true) {
		mutex.lock();
		if (var == 100000000) {
			mutex.unlock();
			break;
		}
		++threads_rates[thread_index];
		++var;
		mutex.unlock();
	}	
}

int main ()
{
	std::chrono::time_point<std::chrono::system_clock> start, end;
	int countOfThreads = std::thread::hardware_concurrency();

	std::cout << "CountOfThreads could be " << countOfThreads << 
std::endl;
	std::vector <std::thread> threads;
	threads_rates.resize(countOfThreads);
	start = std::chrono::system_clock::now();
	volatile int val = 0;
	for (int i = 0; i < countOfThreads; ++i) {
		threads.push_back(std::thread(increment, std::ref(val), i));
	}
	for (int i = 0; i < countOfThreads; ++i) {
		threads[i].join();
	}
	
	end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end-start;
	std::cout << elapsed_seconds.count() << std::endl;
	std::cout << val << std::endl;
	for (int i = 0 ;i < countOfThreads; ++i ) {
		std::cout << threads_rates[i] << ' ';
	}
	//std::cout << *std::max_element(vector.begin(), vector.end());
	return 0;
}
