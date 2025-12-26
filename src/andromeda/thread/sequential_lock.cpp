#include <andromeda/thread/sequential_lock.h>

using namespace andromeda::thread;

void sequential_lock::swap(sequential_lock* swap_lock) //传入另一个线程的CoroutineLock对象，
{
	std::unique_lock<std::mutex> lock(mutex);
	{
		std::unique_lock<std::mutex> lock(swap_lock->mutex);
		swap_lock->is_running = true; //恢复执行传入的线程
		swap_lock->condition.notify_all(); //通知所有线程（即另一个线程）
	}
	is_running = false;
	while(!is_running)
		condition.wait(lock); //阻塞该线程，直到其他sequential_lock对象调用swap(本对象)才可以恢复执行。wait()会自动释放lock变量
}
