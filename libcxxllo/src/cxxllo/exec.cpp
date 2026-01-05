#include <cxxllo/exec.h>

#include <cxxllo/platform.h>

#include <dlfcn.h>

#if !defined(DLFCN_H)

#if defined(__PLATFORM_WIN__)
#error "dependency <dlfcn.h> not found, please install dlfcn-win32 package, or run pacman -S mingw-w64-x86_64-dlfcn in msys2"
#endif

#endif

#include <string.h>
#include <cxxabi.h>
#include <new>

char* cxx_name_demangling(const char* mangled_name)
{
	int status = 0;
	char* name = abi::__cxa_demangle(mangled_name, nullptr, nullptr, &status);
	if(status == 1)
		throw std::bad_alloc();
	return name;
}

/**
 * @brief 收集栈帧信息的回调
 * @param context 栈帧上下文
 * @param _call_stack_ptr 传入_Unwind_Backtrace()的第二个参数，即自定义数据结构
 * @return 本次栈帧遍历完成的操作
 */
static _Unwind_Reason_Code _unwind_callback(struct _Unwind_Context* context, void* _iter_ptr)
{
	call_stack::iterator* iter = (call_stack::iterator*)_iter_ptr;
	if(iter->_skip_counter != 0)
	{
		--iter->_skip_counter; //跳过顶部指定数量的栈帧
		return _Unwind_Reason_Code::_URC_NO_REASON;
	}
	if(iter->frame_count < iter->step_depth)
	{
		//执行遍历栈帧的操作回调。如果没有到栈底且op()返回值为true，则继续执行下一个栈帧的回调，否则终止遍历
		return (_Unwind_Reason_Code)iter->op(context, iter->arg, iter->step_depth, iter->frame_count++);
	}
	else
	{
		return _Unwind_Reason_Code::_URC_END_OF_STACK; //停止遍历栈帧
	}
}

unwind_action call_stack::unwind(unwind_op op, void* arg, size_t step_depth, size_t skip_depth)
{
	iterator iter(step_depth, skip_depth, op, arg);
	return (unwind_action)_Unwind_Backtrace(&_unwind_callback, &iter);
}

static unwind_action _unwind_ip_op(unwind_frame context, void* _call_stack_ptr, int step_depth, int frame_depth)
{
	call_stack* _call_stack = (call_stack*)_call_stack_ptr;
	_call_stack->frame_count = frame_depth + 1;
	_call_stack->instruction_pointers[frame_depth] = get_frame_ip(context);
	return unwind_action::UNWIND_CONTINUE;
}

call_stack call_stack::unwind(size_t step_depth, size_t skip_depth)
{
	call_stack cs;
	cs.instruction_pointers = (void**)malloc(sizeof(void*) * step_depth);
	unwind(&_unwind_ip_op, &cs, step_depth, skip_depth + 1); //skip_depth + 1是为了跳过本函数的栈帧
	return cs;
}

void call_stack::clean()
{
	free(instruction_pointers);
	frame_count = 0;
	instruction_pointers = nullptr;
}

symbol_info symbol_info::resolve(void* symbol_addr)
{
	symbol_info _symbol_info;
	Dl_info _dl_info;
	if(dladdr(symbol_addr, &_dl_info))
	{
		_symbol_info.binary_path = strcpy(new char[strlen(_dl_info.dli_fname) + 1], _dl_info.dli_fname); //线程不安全，需要拷贝
		_symbol_info.symbol_name = _dl_info.dli_sname;
		_symbol_info.base = (void*)_dl_info.dli_fbase;
		_symbol_info.address = (void*)_dl_info.dli_saddr;
	}
	return _symbol_info;
}

