#ifndef ANDROMEDA_THREAD_COROUTINELOCK
#define ANDROMEDA_THREAD_COROUTINELOCK

#include <atomic>
#include <mutex>
#include <condition_variable>

namespace andromeda
{
namespace thread
{
/**
 * @brief 多个线程顺序串行执行的辅助类，A线程开始执行时B线程暂停，B线程执行时C暂停
 */
class sequential_lock
{
private:
	std::atomic_bool is_running;
	std::mutex mutex;
	std::condition_variable condition;

public:
	void swap(sequential_lock* swap_lock); //传入另一个线程的sequential_lock对象，阻塞调用该函数的线程，恢复执行传入的线程
};
}
}

#endif//ANDROMEDA_THREAD_COROUTINELOCK
