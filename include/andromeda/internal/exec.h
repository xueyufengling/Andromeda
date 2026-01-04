#ifndef ANDROMEDA_INTERNAL_EXEC
#define ANDROMEDA_INTERNAL_EXEC

#include "../traits/types.h"
#include <typeinfo>
#include <unwind.h>

#define __DEFAULT_STEP_DEPTH__ 256

/**
 * @brief 将C++经过name mangling的名字恢复
 * @return 成功返回name mangling之前的名字，失败返回nullptr
 */
extern char* cxx_name_demangling(const char* mangled_name);

__attribute__((always_inline)) inline char* cxx_name_demangling(const std::type_info& t)
{
	return cxx_name_demangling(t.name());
}

template<typename _T>
__attribute__((always_inline)) inline char* cxx_name_demangling(const _T&& v)
{
	return cxx_name_demangling(typeid(v));
}

__attribute__((always_inline)) inline void* get_frame_ip(_Unwind_Context* frame_context)
{
	return (void*)_Unwind_GetIP(frame_context);
}

__attribute__((always_inline)) inline void set_frame_ip(_Unwind_Context* frame_context, void* target_ip)
{
	_Unwind_SetIP(frame_context, (_Unwind_Ptr)target_ip);
}

/**
 * @brief 移动指定栈帧的IP
 */
__attribute__((always_inline)) inline void shift_frame_ip(_Unwind_Context* frame_context, size_t shift_step)
{
	set_frame_ip(frame_context, (unsigned char*)(get_frame_ip(frame_context)) + shift_step);
}

/**
 * @brief 从Instruction Pointer获取该指令所属的函数的函数指针地址
 */
__attribute__((always_inline)) inline void* _function_pointer_from_ip(void* ip)
{
	return _Unwind_FindEnclosingFunction(ip);
}

template<typename _FuncType>
__attribute__((always_inline)) inline _FuncType* function_pointer_from_ip(void* ip)
{
	static_assert(type_classification_of<_FuncType>::result == type_classification::FUNCTION, "_FuncType must be a function type");
	return (_FuncType*)_function_pointer_from_ip(ip);
}

template<typename _RetType, typename ... _ArgTypes>
__attribute__((always_inline)) inline _RetType __call(void* func_ptr, _ArgTypes ...args)
{
	return ((_RetType (*)(_ArgTypes...))func_ptr)(args...);
}

enum unwind_action
{
	UNWIND_CONTINUE = _Unwind_Reason_Code::_URC_NO_REASON, //如果没有到栈底，则继续执行下一个栈帧的回调，否则终止遍历
	UNWIND_STOP = _Unwind_Reason_Code::_URC_END_OF_STACK, //停止遍历栈帧
};

typedef struct call_stack
{
	/**
	 * @brief 遍历栈帧的操作
	 * @param _Unwind_Context* 当前遍历中的栈帧上下文，该指针仅在遍历本栈帧时有效，遍历结束后已经销毁失效
	 * @param void* 传入unwind的arg参数
	 * @param int 遍历总深度，即step_depth
	 * @param int 当前所处深度，skip_depth不计入，从0开始
	 * @return 如何遍历下一个栈帧
	 */
	typedef unwind_action (*unwind_op)(_Unwind_Context*, void*, int, int); //回溯的操作

	/**
	 * 遍历过程中的迭代器
	 */
	struct iterator
	{
		unwind_op op;
		void* arg;
		size_t _skip_counter; //跳过栈帧计数器
		size_t frame_count = 0,  //实际的栈帧数目
				step_depth;  //可以遍历最大栈帧深度

		/**
		 * _skip_counter=skip_depth+1是为了跳过call_stack::unwind()本身的栈帧
		 */
		inline iterator(size_t step_depth, size_t skip_depth, unwind_op op, void* arg = nullptr) :
				op(op), arg(arg), _skip_counter(skip_depth + 1), step_depth(step_depth)
		{
		}
	};

	/**
	 * @brief 遍历栈帧并操作
	 * @param op 遍历中执行的操作
	 * @param arg 传入op的自定义参数
	 * @param step_depth 遍历最大深度，跳过的深度不计入
	 * @param skip_depth 跳过栈顶深度
	 * @return 最后一次遍历的栈帧op返回值
	 */
	static unwind_action unwind(unwind_op op, void* arg = nullptr, size_t step_depth = __DEFAULT_STEP_DEPTH__, size_t skip_depth = 0);

	inline call_stack() = default;

	static call_stack unwind(size_t step_depth = __DEFAULT_STEP_DEPTH__, size_t skip_depth = 0);

	size_t frame_count = 0;  //实际的栈帧数目
	void** instruction_pointers = nullptr; //每个栈帧的IP，即栈帧执行当前所处的指令地址

	/**
	 * @brief 释放资源
	 */
	void clean();

	/**
	 * @brief 获取指定栈深度的栈帧函数指针
	 */
	inline void* operator[](size_t depth)
	{
		return _function_pointer_from_ip(instruction_pointers[depth]);
	}

	template<typename _FuncType>
	inline _FuncType* at(size_t depth)
	{
		return function_pointer_from_ip<_FuncType>(instruction_pointers[depth]);
	}

	/**
	 * @brief 调用指定栈帧的函数，注意防止无限递归导致栈溢出
	 */
	template<typename _RetType, typename ... _ArgTypes>
	inline _RetType call(size_t depth, _ArgTypes ...args)
	{
		return __call<_RetType, _ArgTypes...>((*this)[depth], args...);
	}
} call_stack;

#endif //ANDROMEDA_INTERNAL_EXEC
