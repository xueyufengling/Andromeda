#ifndef ANDROMEDA_INTERNAL_SIGNALS
#define ANDROMEDA_INTERNAL_SIGNALS

/**
 * 为了获取完整的POSIX信号，<signal.h>必须由本头文件第一个引入，否则部分POSIX信号将没有宏定义。
 * 最典型的例如<thread>、<istream>、<ostream>等就会引入<signal.h>，因此此头文件必须在这些头文件之前引入。
 */
#ifdef _INC_SIGNAL
#define __SIGNAL_H_ALMOST_INCLUDED__
#error "<andromeda/internal/signals.h> must be included before any other header files that including <signal.h>"
#endif

/**
 * 拓展了系统信号处理，不可再使用C/C++原本自带的信号处理，尤其是不可再更改信号的自定义处理函数
 */

#include "object.h"

//如果是非POSIX系统，则临时开启以初始化enum signal全部的信号
#ifndef _POSIX
#define __POSIX_FOR_SIG__
#define _POSIX
#endif

#ifndef __USE_MINGW_ALARM
#define __USE_MINGW_ALARM_FOR_SIG__
#define __USE_MINGW_ALARM
#endif

#include <signal.h>

enum system_signal : int
{
	//信号总个数
	SIG_NUM = NSIG,
	//通用信号
	SIG_INT = SIGINT, //Interactive attention，Ctrl+C触发
	SIG_ILL = SIGILL, //Illegal instruction，无效指令码
	SIG_ABRT_COMPAT = SIGABRT_COMPAT,
	SIG_FPE = SIGFPE, //Floating-Point Exception，代数计算错误，例如除以0操作
	SIG_SEGV = SIGSEGV, //Segment fault，段错误
	SIG_TERM = SIGTERM, //Termination，正常终止
	SIG_BREAK = SIGBREAK,
	SIG_ABRT = SIGABRT, //Abnormal termination，非正常终止，例如abort()
	SIG_ABRT2 = SIGABRT2,
	//POSIX
#ifndef __SIGNAL_H_ALMOST_INCLUDED__
	SIG_HUP = SIGHUP, //Hangup，POSIX独有
	SIG_QUIT = SIGQUIT, //Quit，POSIX独有
	SIG_TRAP = SIGTRAP, //Trace trap，POSIX独有
	SIG_IOT = SIGIOT, //IOT instruction，POSIX独有
	SIG_EMT = SIGEMT, //EMT instruction，POSIX独有
	SIG_KILL = SIGKILL, //Kill，POSIX独有
	SIG_BUS = SIGBUS, //Bus error，POSIX独有
	SIG_SYS = SIGSYS, //Bad argument to system call，POSIX独有
	SIG_PIPE = SIGPIPE, //只写入pipe无程序读取，POSIX独有
	SIG_ALRM = SIGALRM, //Alarm clock，POSIX独有
#endif
};

#ifdef __USE_MINGW_ALARM_FOR_SIG__
#undef __USE_MINGW_ALARM_FOR_SIG__
#undef __USE_MINGW_ALARM
#endif

//enum signal的信号初始化完毕后关闭临时的_POSIX宏定义
#ifdef __POSIX_FOR_SIG__
#undef __POSIX_FOR_SIG__
#undef _POSIX
#endif

/**
 * @brief 系统发送的信号的额外参数
 */
extern const object SYSTEM_SIG;

/**
 * @brief 发出进程信号
 * @param sig 信号
 * @param extra_param 信号的额外参数，系统发出的额外参数为0.可以通过额外参数区分在哪里发出的信号
 * @return 信号是否发出成功
 */
extern bool raise_signal(system_signal sig, object extra_param = 0);

/**
 * @breif 信号处理函数
 * @param system_signal 信号
 * @param int 本处理函数的优先级
 * @param object_val 信号额外参数
 * @return 是否将此信号传递给下一个处理函数
 */
typedef bool (*signal_handler)(system_signal, int, object);

/**
 * @brief 指定信号的处理
 */
extern void handle_signal(system_signal sig, signal_handler handler, int priority = 0);

/**
 * @brief 初始化本信号处理系统，此函数已经在进程启动时调用过一次，通常情况下无需再调用
 * 		  本函数使用C库的signal()函数为信号设置了自定义函数，整个程序运行期间不允许再调用signal()函数修改信号处理函数
 */
extern __attribute__((constructor, used)) void init_signals_lib();

#endif //ANDROMEDA_INTERNAL_SIGNALS
