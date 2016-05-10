#include <iostream>
#include <cstdlib>
#include <thread>
#include <vector>
#include <algorithm>
#include <atomic>
#include <assert.h>
#include <mutex>



class Futex {
	// time 5.76
	private:
		std::atomic<int> hutex;
	public: 
		Futex() {
			hutex.store(-1);
		}
		void lock() {
			int thread_index = std::hash<std::thread::id>()(std::this_thread::get_id());
			int free = -1;
			while(!hutex.compare_exchange_strong(free, thread_index)) {
				free = -1;
				std::this_thread::yield();
			}
		}
		void unlock() {
			std::thread::id thread_index = std::this_thread::get_id();
			hutex.store(-1);
		}
};

class FuFutex {
	// time 3.69
	// Для не volatile переменных достаточно и одновременно необходимо иметь барьеры не слабее односторонних,
	// чтобы операции не выходили из критической секции.  Их я и использую, прирост в скорости ощутимый.
	private:
		std::atomic<int> hutex;
	public: 
		FuFutex() {
			hutex.store(-1);
		}
		void lock() {
			int thread_index = std::hash<std::thread::id>()(std::this_thread::get_id());
			int free = -1;
			while(!hutex.compare_exchange_strong(free, thread_index, std::memory_order_acquire)) {
				free = -1;
				std::this_thread::yield();
			}
		}
		void unlock() {
			std::thread::id thread_index = std::this_thread::get_id();
			//assert(hutex.load() == thread_index);
			hutex.store(-1, std::memory_order_release);
		}
};


class VolatileFutex {
	// time 3.78
	// Volatile - чтения и запись подразумевают соответствующие односторонние барьеры -> все корректно
	private:
		std::atomic<int> hutex;
	public: 
		VolatileFutex() {
			hutex.store(-1);
		}
		void lock() {
			int thread_index = std::hash<std::thread::id>()(std::this_thread::get_id());
			int free = -1;
			while(!hutex.compare_exchange_strong(free, thread_index, std::memory_order_relaxed)) {
				free = -1;
				std::this_thread::yield();
			}
		}
		void unlock() {
			std::thread::id thread_index = std::this_thread::get_id();
			//assert(hutex.load() == thread_index);
			hutex.store(-1, std::memory_order_relaxed);
		}
};

