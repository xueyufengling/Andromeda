#ifndef ANDROMEDA_THREAD_THREAD
#define ANDROMEDA_THREAD_THREAD

#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include "../traits/types.h"

//THREAD宏可用于任何Callable，但不可用func参数不能传入ClassObj.Func，因为decltype无法解析可能的重载函数
#define THREAD(obj_name,func) andromeda::util::Thread<decltype(func)> obj_name(func)

#ifndef HAS_FUNC_INITIALIZE
#define HAS_FUNC_INITIALIZE
def_cls_has_func(initialize)
#endif//HAS_FUNC_INITIALIZE#ifndef HAS_FUNC_RUN
#define HAS_FUNC_RUN
def_cls_has_func(run)
#endif//HAS_FUNC_RUN#ifndef HAS_FUNC_TERMINATE#define HAS_FUNC_TERMINATE
def_cls_has_func(terminate)
#endif//HAS_FUNC_TERMINATE#ifndef HAS_FUNC_BEFORE_STOP#define HAS_FUNC_BEFORE_STOP
def_cls_has_func(before_stop)
#endif//HAS_FUNC_BEFORE_STOP#ifndef HAS_FUNC_AFTER_STOP#define HAS_FUNC_AFTER_STOP
def_cls_has_func(after_stop)
#endif//HAS_FUNC_AFTER_STOP#ifndef HAS_FUNC_BEFORE_SUSPENDED#define HAS_FUNC_BEFORE_SUSPENDED
def_cls_has_func(before_suspended)
#endif//HAS_FUNC_BEFORE_SUSPENDED#ifndef HAS_FUNC_AFTER_SUSPENDED#define HAS_FUNC_AFTER_SUSPENDED
def_cls_has_func(after_suspended)
#endif//HAS_FUNC_AFTER_SUSPENDED#ifndef HAS_FUNC_BEFORE_RESUME#define HAS_FUNC_BEFORE_RESUME
def_cls_has_func(before_resume)
#endif//HAS_FUNC_BEFORE_RESUME#ifndef HAS_FUNC_AFTER_RESUME#define HAS_FUNC_AFTER_RESUME
def_cls_has_func(after_resume)
#endif//HAS_FUNC_AFTER_RESUME

//子类必须添加的friend class
#define DefineThread \
	friend class has_func(initialize)<void>;\
	friend class has_func(run)<void>;\
	friend class has_func(terminate)<void>;\
	friend class has_func(before_stop)<void>;\
	friend class has_func(after_stop)<void>;\
	friend class has_func(before_suspended)<void>;\
	friend class has_func(after_suspended)<void>;\
	friend class has_func(before_resume)<void>;\
	friend class has_func(after_resume)<void>;

namespace andromeda {
	namespace thread {
		enum ThreadWorkMode
		{
			Join, Detach
		};
		//处于Stopped时_thread=nullptr，只能重新调用start()执行
		enum ThreadState
		{
			Stopped, Running, Suspended
		};

		template<typename Thread>
		static inline void exitThread(Thread *thread)
		{
			thread->exit();
		}

		template<typename Callable,typename Derived=void> //Callable为任何可调用对象（包括成员函数），Derived用于继承时传入子类CRTP，若is_class<Derived>=false则表示不继承
		class Thread
		{
			DefineThread
			friend void exitThread<>(Thread<Callable,Derived>*thread);
		public:
			typedef typename andromeda::traits::degenerate_func<Callable>::result_type DegeneratedCallableType;
		private:
			std::thread* _thread=nullptr;
			ThreadWorkMode _work_state=Detach;
			ThreadState _state=Stopped;
			std::atomic_bool should_pause;
			std::atomic_bool should_stop;
			std::mutex _mutex;
			std::condition_variable _condition;
			void* _callable_obj=nullptr; //储存原始的调用对象或函数指针。
			std::function<DegeneratedCallableType> _callable; //用于实际执行的封装后的可调用对象，Callable如果是成员函数则退化为等效的普通函数并绑定this
			bool* loop_flag=nullptr; //循环控制变量，nullptr表示不循环执行
			bool is_callable_set=false; //如果为false则调用子类的run()函数

			template<typename ...Args>
			void _run(Args ...args) //非virtual类成员函数，其指针与通常函数指针不同
			{
				pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL); //允许退出线程
				pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL); //收到CANCEL信号后立即退出线程
				if(andromeda::traits::is_class<Derived>::result&&has_func(initialize)<void>::check<Derived>::result)
					_initialize();
				if(shouldLoop())
					while(!should_stop)
					{
						_callable(args...); //isLoop=true时重复调用执行函数
						if(should_pause)
						{
							std::unique_lock<std::mutex> lock(_mutex); //给互斥量上锁
							while(should_pause)
								_condition.wait(lock); //等待直到_condition通知该线程
						}
					}
				else
					_callable(args...); //is_loop=false时只调用一次执行函数
				should_pause=false;
				should_stop=false;
				if(andromeda::traits::is_class<Derived>::result&&has_func(terminate)<void>::check<Derived>::result)
					_terminate();
				exit(); //正常结束后释放线程，此时可通过start()再次调用而不必重新setThreadCallable()
			}

			void _run_derived() //使用子类run()
			{
				pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL); //允许退出线程
				pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL); //收到CANCEL信号后立即退出线程
				if(andromeda::traits::is_class<Derived>::result&&has_func(initialize)<void>::check<Derived>::result)
					_initialize();
				if(shouldLoop())
					while(!should_stop)
					{
						_run(); //isLoop=true时重复调用执行函数
						if(should_pause)
						{
							std::unique_lock<std::mutex> lock(_mutex);
							while(should_pause)
								_condition.wait(lock);
						}
					}
				else
					_run(); //isLoop=false时只调用一次执行函数
				should_pause=false;
				should_stop=false;
				if(andromeda::traits::is_class<Derived>::result&&has_func(terminate)<void>::check<Derived>::result)
					_terminate();
				exit(); //正常结束后释放线程，此时可通过start()再次调用而不必重新setThreadCallable()
			}

		protected:
			__attribute__((always_inline)) inline bool shouldLoop()
			{
				return loop_flag&&(*loop_flag);
			}
			//CRTP实现
			__attribute__((always_inline)) inline void _initialize() //执行函数执行之前调用一次
			{
				if(andromeda::traits::is_class<Derived>::result&&has_func(initialize)<void>::check<Derived>::result) //Derived是类且有该成员函数
					((typename andromeda::traits::_if<andromeda::traits::is_class<Derived>::result,Derived,Thread<Callable,Derived>>::result_type*)this)->initialize();
			}

			__attribute__((always_inline)) inline void _run() //供继承类重写使用的执行函数，重写后可以不执行_callable_obj
			{
				if(andromeda::traits::is_class<Derived>::result&&has_func(run)<void>::check<Derived>::result)
					((typename andromeda::traits::_if<andromeda::traits::is_class<Derived>::result,Derived,Thread<Callable,Derived>>::result_type*)this)->run();
			}

			__attribute__((always_inline)) inline void _terminate() //执行函数（包括isLoop=true时的情况）结束后调用一次
			{
				if(andromeda::traits::is_class<Derived>::result&&has_func(terminate)<void>::check<Derived>::result)
					((typename andromeda::traits::_if<andromeda::traits::is_class<Derived>::result,Derived,Thread<Callable,Derived>>::result_type*)this)->terminate();
			}

			__attribute__((always_inline)) inline void _before_stop() //每次成功调用stop()前调用一次
			{
				if(andromeda::traits::is_class<Derived>::result&&has_func(before_stop)<void>::check<Derived>::result)
					((typename andromeda::traits::_if<andromeda::traits::is_class<Derived>::result,Derived,Thread<Callable,Derived>>::result_type*)this)->before_stop();
			}

			__attribute__((always_inline)) inline void _after_stop() //每次成功调用stop()后调用一次
			{
				if(andromeda::traits::is_class<Derived>::result&&has_func(after_stop)<void>::check<Derived>::result)
					((typename andromeda::traits::_if<andromeda::traits::is_class<Derived>::result,Derived,Thread<Callable,Derived>>::result_type*)this)->after_stop();
			}

			__attribute__((always_inline)) inline void _before_suspended() //每次成功调用suspended()前调用一次
			{
				if(andromeda::traits::is_class<Derived>::result&&has_func(before_suspended)<void>::check<Derived>::result)
					((typename andromeda::traits::_if<andromeda::traits::is_class<Derived>::result,Derived,Thread<Callable,Derived>>::result_type*)this)->before_suspended();
			}

			__attribute__((always_inline)) inline void _after_suspended() //每次成功调用suspended()后调用一次
			{
				if(andromeda::traits::is_class<Derived>::result&&has_func(after_suspended)<void>::check<Derived>::result)
					((typename andromeda::traits::_if<andromeda::traits::is_class<Derived>::result,Derived,Thread<Callable,Derived>>::result_type*)this)->after_suspended();
			}

			__attribute__((always_inline)) inline void _before_resume() //每次成功调用resume()前调用一次
			{
				if(andromeda::traits::is_class<Derived>::result&&has_func(before_resume)<void>::check<Derived>::result)
					((typename andromeda::traits::_if<andromeda::traits::is_class<Derived>::result,Derived,Thread<Callable,Derived>>::result_type*)this)->before_resume();
			}

			__attribute__((always_inline)) inline void _after_resume() //每次成功调用resume()后调用一次
			{
				if(andromeda::traits::is_class<Derived>::result&&has_func(after_resume)<void>::check<Derived>::result)
					((typename andromeda::traits::_if<andromeda::traits::is_class<Derived>::result,Derived,Thread<Callable,Derived>>::result_type*)this)->after_resume();
			}
		public:
			Thread(void)
			{
				should_pause=false;
				should_stop=false;
			}
			//此构造函数可用于继承类中重写run()的类
			Thread(bool *loopFlag,ThreadWorkMode workState=Detach) :
					Thread() //默认采用Detach模式
			{
				this->loop_flag=loopFlag;
				setThreadWorkMode(workState);
			}
			//isLoop设定是否循环执行运行函数。如果设定为false，则不可使用pause()、resume()、stop()，在执行运行函数期间只可执行exit()操作
			template<typename MCallable=Callable>
			Thread(MCallable &op,bool *loopFlag=nullptr,ThreadWorkMode workState=Detach) :
					Thread() //默认采用Detach模式
			{
				setThreadCallable<MCallable>(op,loopFlag);
				setThreadWorkMode(workState);
			}
			template<typename MCallable=Callable>
			Thread(MCallable &&op,bool *loopFlag=nullptr,ThreadWorkMode workState=Detach) :
					Thread() //默认采用Detach模式
			{
				setThreadCallable<MCallable>(op,loopFlag);
				setThreadWorkMode(workState);
			}

			~Thread()
			{
				exit();
			}

			void exit() //调用exit()后强制结束线程并释放内存
			{
				pthread_cancel(_thread->native_handle());
				delete _thread;
				_thread=nullptr;
				_state=Stopped;
			}

			inline Thread& setThreadWorkMode(ThreadWorkMode workState) //只能在执行start()前调用，一旦执行start()后将无法改变
			{
				if(!_thread)
					_work_state=workState;
				return *this;
			}

			template<typename MCallable=Callable> //MCallable可以与Callable不同（例如Callable传入普通函数类型，MCallable传入等效成员函数类型或其他等效可调用类型），但最好保持一致
			Thread& setThreadCallable(MCallable &op,bool *loopFlag=nullptr) //储存_callable_obj原始对象，而实际执行的_callable对象在start()中才会实例化
			{
				if(_thread) //如果线程已经存在，则强制结束线程并释放
					exit();
				_callable_obj=(void*)&op;
				is_callable_set=true;
				this->loop_flag=loopFlag;
				return *this;
			}

			template<typename MCallable=Callable>
			Thread& setThreadCallable(MCallable &&op,bool *loopFlag=nullptr)
			{
				if(_thread) //如果线程已经存在，则强制结束线程并释放
					exit();
				_callable_obj=(void*)&std::move(op);
				is_callable_set=true;
				this->loop_flag=loopFlag;
				return *this;
			}

			Thread& clearThreadCallable() //清除已经设置的运行函数
			{
				if(_thread) //如果线程已经存在，则强制结束线程并释放
					exit();
				is_callable_set=false;
				return *this;
			}

			template<typename ...Args>
			void start(Args &&... args) //非成员函数调用
			{
				if(_thread) //已经调用了start()且不是处于Stopped状态就直接返回
					return;
				if(!is_callable_set) //没有设置运行函数，调用子类的run()
				{
					if(andromeda::traits::is_class<Derived>::result&&has_func(run)<void>::check<Derived>::result)
					{
						_thread=new std::thread(std::bind(&Thread<Callable,Derived>::_run_derived,this));
						goto END;
					}
					else
						//子类没有run()则是无效调用直接返回
						return;
				}
				_callable=std::function<DegeneratedCallableType>(*(DegeneratedCallableType*)_callable_obj);
				_thread=new std::thread(std::bind(&Thread<Callable,Derived>::_run<Args...>,this,args...));
				END:_state=Running;
				switch(_work_state)
				{
				case Join:
					_thread->join();
					break;
				case Detach:
					_thread->detach();
					break;
				}
			}

			template<typename Class,typename ...Args>
			void start(Class *cls,Args &&... args) //成员函数调用
			{
				if(_thread) //已经调用了start()且不是处于Stopped状态就直接返回
					return;
				if(!is_callable_set) //没有设置运行函数，调用子类的run()
				{
					if(andromeda::traits::is_class<Derived>::result&&has_func(run)<void>::check<Derived>::result)
					{
						_thread=new std::thread(std::bind(&Thread<Callable,Derived>::_run_derived,this));
						goto END;
					}
					else
						//子类没有run()则是无效调用直接返回
						return;
				}
				typedef typename andromeda::traits::get_func_ret_type<Callable,Args...>::result_type ret_type;
				_callable=andromeda::traits::bind_this<Class,ret_type,Args...>::bind(*(typename andromeda::traits::func_type<Class,ret_type,Args...>::result_type*)_callable_obj,cls); //为成员函数绑定this
				_thread=new std::thread(std::bind(&Thread<Callable,Derived>::_run<Args...>,this,args...));
				END:_state=Running;
				switch(_work_state)
				{
				case Join:
					_thread->join();
					break;
				case Detach:
					_thread->detach();
					break;
				}
			}

			//返回true表示操作成功，返回false表示失败
			bool suspend()
			{
				if(_thread&&shouldLoop())
				{
					if(andromeda::traits::is_class<Derived>::result&&has_func(before_suspended)<void>::check<Derived>::result)
						_before_suspended();
					should_pause=true;
					_state=Suspended;
					if(andromeda::traits::is_class<Derived>::result&&has_func(after_suspended)<void>::check<Derived>::result)
						_after_suspended();
					return true;
				}
				return false;
			}

			bool resume()
			{
				if(_thread&&shouldLoop())
				{
					if(andromeda::traits::is_class<Derived>::result&&has_func(before_resume)<void>::check<Derived>::result)
						_before_resume();
					should_pause=false;
					_condition.notify_all();
					_state=Running;
					if(andromeda::traits::is_class<Derived>::result&&has_func(after_resume)<void>::check<Derived>::result)
						_after_resume();
					return true;
				}
				return false;
			}

			bool stop() //stop()后依然可以直接继续调用start()重新执行
			{
				if(_thread&&shouldLoop())
				{
					if(andromeda::traits::is_class<Derived>::result&&has_func(before_stop)<void>::check<Derived>::result)
						_before_stop();
					should_pause=false;
					should_stop=true;
					_condition.notify_all();
					_thread->join();
					exit();
					if(andromeda::traits::is_class<Derived>::result&&has_func(after_stop)<void>::check<Derived>::result)
						_after_stop();
					return true;
				}
				return false;
			}

			inline ThreadWorkMode getThreadWorkState()
			{
				return _work_state;
			}

			inline ThreadState getThreadState()
			{
				return _state;
			}

			inline std::thread::id getThreadID() //线程不存在则返回id(0)
			{
				return _thread?_thread->get_id():std::thread::id(0);
			}

			inline std::thread::native_handle_type getNativeHandle() //线程不存在则返回nullptr
			{
				return _thread?_thread->native_handle():(std::thread::native_handle_type)nullptr;
			}
		};
	}
}
#endif//ANDROMEDA_THREAD_THREAD