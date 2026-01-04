#include <andromeda/internal/exec.h>

#include <andromeda/platform/platform_libcall.h>
#include <andromeda/log/log.h>
#include <malloc.h>
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

#include <iostream>
static unwind_action _unwind_ip_op(unwind_frame context, void* _call_stack_ptr, int step_depth, int frame_depth)
{
	call_stack* _call_stack = (call_stack*)_call_stack_ptr;
	_call_stack->frame_count = frame_depth + 1;
	_call_stack->instruction_pointers[frame_depth] = get_frame_ip(context);
	std::cout << std::dec << frame_depth << " -> base = " << text_segment_base(context) << ", ip = " << (_call_stack->instruction_pointers[frame_depth]) << ", offset = " << std::hex << (text_segment_offset(context)) << std::endl;
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

#if defined (_WIN32) || defined (_WIN64)

#include <windows.h>
#include <dbghelp.h>
#include <psapi.h>

HANDLE symbol_processs_handle;
size_t symbol_name_max_len = 512;

__attribute__((constructor, used)) static void _win_symbol_lib_init()
{
	symbol_processs_handle = win_GetCurrentProcess();
	win_SymInitialize(symbol_processs_handle, nullptr, false);
}

__attribute__((destructor, used)) static void _win_symbol_lib_term()
{
	win_SymCleanup(symbol_processs_handle);
	symbol_processs_handle = NULL;
}

symbol_info symbol_info::resolve(void* symbol_addr)
{
	symbol_info _symbol_info;
	size_t _win_symbol_info_size = sizeof(SYMBOL_INFO) + sizeof(TCHAR) * (symbol_name_max_len - 1); //多余的空间为结构体末端Name字段的缓冲区
	SYMBOL_INFO* _win_symbol_info = (SYMBOL_INFO*)malloc(_win_symbol_info_size); //SYMBOL_INFO是动态大小的结构体，不能使用new分配
	_win_symbol_info->SizeOfStruct = _win_symbol_info_size;
	_win_symbol_info->MaxNameLen = symbol_name_max_len;
	if(win_SymFromAddr(symbol_processs_handle, (DWORD64 )symbol_addr, nullptr, _win_symbol_info))
	{
		_symbol_info.symbol_name = strcpy(new char[_win_symbol_info->NameLen + 1], _win_symbol_info->Name);
		_symbol_info.base = (void*)_win_symbol_info->ModBase;
		_symbol_info.address = (void*)_win_symbol_info->Address;
	}
	free(_win_symbol_info); //查询符号信息结束后释放结构体内存
	char* bin_path = new char[MAX_PATH];
	if(win_GetModuleFileNameExA(symbol_processs_handle, nullptr, bin_path, MAX_PATH))
	{
		_symbol_info.binary_path = bin_path;
	}
	else
	{
		delete[] bin_path;
	}
	return _symbol_info;
}

#elif defined(__unix__) || defined(__unix) || defined(unix)

#include <dlfcn.h>

symbol_info symbol_info::resolve(void* symbol_addr)
{
	symbol_info _symbol_info;
	Dl_info _dl_info;
	if(dladdr(symbol_addr, &_dl_info))
	{
		_symbol_info.binary_path = info.dli_fname;
		_symbol_info.symbol_name = info.dli_sname;
		_symbol_info.base = (void*)info.dli_fbase;
		_symbol_info.address = (void*)info.dli_saddr;
	}
	return _symbol_info;
}

#else
#error "get_symbol_info() is not available in current system"
#endif
