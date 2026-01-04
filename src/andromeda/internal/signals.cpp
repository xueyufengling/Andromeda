#include <andromeda/internal/signals.h>

#include <mutex>
#include <deque>
#include <vector>
#include <algorithm>

struct system_flag
{
};
//编译单元内变量，其指针只有SYSTEM_SIG持有，故可用作区分系统信号
static constexpr system_flag SYSTEM_FLAG;
const void* SYSTEM_SIG = &SYSTEM_FLAG;

struct _signal_handler_with_priority
{
	signal_handler handler;
	int priority;
};

//信号参数队列互斥锁
static std::mutex signal_params_mutex;

//调用init_signals_lib()前的系统信号处理函数
static __p_sig_fn_t default_signal_handlers[system_signal::SIG_NUM]{};

//信号处理函数队列
static std::vector<_signal_handler_with_priority> signal_handlers_queue[system_signal::SIG_NUM]{};

//信号额外参数队列
static std::deque<void*> signal_extra_params_deque[system_signal::SIG_NUM]{};

bool raise_signal(system_signal sig, void* extra_param)
{
	bool success = false;
	signal_params_mutex.lock();
	if((success = !raise(sig))) //返回0表示发送信号成功，只有成功才会推入额外参数
		signal_extra_params_deque[sig].push_back(extra_param);
	signal_params_mutex.unlock();
	return success;
}

void handle_signal(system_signal sig, signal_handler handler, int priority)
{
	std::vector<_signal_handler_with_priority>& handlers_queue = signal_handlers_queue[sig];
	handlers_queue.push_back({handler, priority});
	std::sort(handlers_queue.begin(), handlers_queue.end(), [](const _signal_handler_with_priority& h1, const _signal_handler_with_priority& h2) -> bool
			{
				return h1.priority > h2.priority;
			}); //按照优先级降序排列
}

static void _signals_handler(int sig)
{
	void* extra_param = (void*)SYSTEM_SIG; //没有自定义参数则视作系统发出的信号
	signal_params_mutex.lock();
	std::deque<void*>& params_deque = signal_extra_params_deque[sig];
	if(!params_deque.empty())
	{
		extra_param = params_deque.front(); //有额外参数，取出队列最前端参数
		params_deque.pop_front();
	}
	signal_params_mutex.unlock();
	std::vector<_signal_handler_with_priority>& handlers_queue = signal_handlers_queue[sig];
	if(handlers_queue.empty())
	{
		default_signal_handlers[sig](sig); //没有通过handle_signal()注册信号处理函数，则执行默认的信号处理函数
	}
	else
	{
		for(_signal_handler_with_priority& handler_with_priority : handlers_queue)
		{
			if(!handler_with_priority.handler((system_signal)sig, handler_with_priority.priority, extra_param))
				break; //若信号处理函数返回false，则阻断信号，不再传递给优先级更低的处理函数
		}
	}
}

void init_signals_lib()
{
	for(int sig = 1; sig < system_signal::SIG_NUM; ++sig)
		default_signal_handlers[sig] = signal(sig, _signals_handler); //设置新的信号处理函数，并储存原先的
}

