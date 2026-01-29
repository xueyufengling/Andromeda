#ifndef _CXXLLO_ASM
#define _CXXLLO_ASM

/**
 * 内嵌汇编定义宏
 */

#include <cxxllo/architecture.h>
#include <ppmp/ppmp.h>

/**
 * 具有约束字符的输入、输出变量声明
 */
#define __asm_constraint__(asm_var_name, constraint, value)\
	__if_else__(__is_empty__(asm_var_name))(\
		__str__(constraint) (value),\
		[asm_var_name] __str__(constraint) (value)\
	)

/**
 * @brief 只读的asm输入变量
 * @param constraint 变量的约束符，汇编代码块的期望类型，即如何将给定的值传递给汇编代码块。如果约束符的类型与实际值的类型不一致会生成mov指令转换
 * 					 局部变量为m，立即值为i。如果传入的值类型为m或i，且约束符为寄存器（例如r），则将会生成mov指令把值从m或i移动到寄存器。由于一些汇编指令直接支持内存操作数，这时这些mov指令就是冗余的。
 */
//仅限C/C++变量
#define __asm_in__2(var_name, constraint) __asm_in__3(var_name, constraint, var_name)
//支持所有约束，例如立即数
#define __asm_in__3(asm_var_name, constraint, value) __asm_constraint__(asm_var_name, constraint, value)
#define __asm_in__(...) __macro_with_params__(__asm_in__, __VA_ARGS__)

/**
 * @brief 可交换次序的asm输入变量，不能是最后一个输入变量
 */
#define __asm_inxchg__2(var_name, constraint) __asm_inxchg__3(var_name, constraint, var_name)
#define __asm_inxchg__3(asm_var_name, constraint, value) __asm_constraint__(asm_var_name, %constraint, value)
#define __asm_inxchg__(...) __macro_with_params__(__asm_inxchg__, __VA_ARGS__)

/**
 * @brief 只写的asm输出变量
 */
#define __asm_out__2(var_name, constraint) __asm_out__3(var_name, constraint, var_name)
#define __asm_out__3(asm_var_name, constraint, var_name) __asm_constraint__(asm_var_name, =&constraint, var_name)
#define __asm_out__(...) __macro_with_params__(__asm_out__, __VA_ARGS__)

/**
 * @brief 可读也可写的输入同时也是输出变量
 */
#define __asm_inout__2(var_name, constraint) __asm_inout__3(var_name, constraint, var_name)
#define __asm_inout__3(asm_var_name, constraint, var_name) __asm_constraint__(asm_var_name, +constraint, var_name)
#define __asm_inout__(...) __macro_with_params__(__asm_inout__, __VA_ARGS__)

/**
 * @brief asm拓展的列表必须写入此宏内，即输入列表、输出列表、破坏寄存器列表、跳转标签列表
 */
#define __asm_list__(...) : __VA_ARGS__

#define __asm_modifiers_none__()
#define __asm_modifiers_volatile_goto__() __volatile__, goto
#define __asm_modifiers_volatile__() __volatile__
#define __asm_modifiers_goto__() goto
#define __equal____asm_modifiers_volatile_goto__(x) x
#define __equal____asm_modifiers_goto__(x) x

/**
 * __asm__内联并不是完全只内联体内的指令，也会自动生成所使用了的寄存器的保存和恢复（现场恢复）相关mov指令
 * 相比于inline函数，__asm__内联少了参数、局部变量的栈空间和栈变量初始化、参数传递相关的指令。
 */
#define __asm_def__(asm_modifiers, clobber_list, asm_in_list, asm_out_list, ...)\
		__asm__ __replace_delim__(, asm_modifiers())(\
			__replace_delim__("\n\t", __VA_ARGS__)\
			__if_else__(__or__(__equal__(asm_modifiers, __asm_modifiers_goto__), __equal__(asm_modifiers, __asm_modifiers_volatile_goto__)))(\
				:\
				__pack_list__(asm_in_list)\
				__pack_list__(clobber_list)\
				__pack_list__(asm_out_list),\
				__pack_list__(asm_out_list)\
				__pack_list__(asm_in_list)\
				__pack_list__(clobber_list)\
			)\
		)

#define __asm_inline__optimized_var(clobber_list, asm_in_list, asm_out_list, ...) __asm_def__(__asm_modifiers_none__, __pack_list__(clobber_list), __pack_list__(asm_in_list), __pack_list__(asm_out_list), ##__VA_ARGS__)
#define __asm_inline__volatile_var(clobber_list, asm_in_list, asm_out_list, ...) __asm_def__(__asm_modifiers_volatile__, __pack_list__(clobber_list), __pack_list__(asm_in_list), __pack_list__(asm_out_list), ##__VA_ARGS__)
#define __asm_inline__optimized_jmp(clobber_list, asm_in_list, asm_out_list, ...) __asm_def__(__asm_modifiers_goto__, __pack_list__(clobber_list), __pack_list__(asm_in_list), __pack_list__(asm_out_list), ##__VA_ARGS__)
#define __asm_inline__volatile_jmp(clobber_list, asm_in_list, asm_out_list, ...) __asm_def__(__asm_modifiers_volatile_goto__, __pack_list__(clobber_list), __pack_list__(asm_in_list), __pack_list__(asm_out_list), ##__VA_ARGS__)

/**
 * @brief 内联汇编指令
 * @param optimize_flag 编译器优化选项，可选值有optimized、volatile
 * @param out_flag 汇编对外输出选项，可选值有var、jmp
 * @details 例如以下示例
 * 			__asm_inline__(volatile, var)(
 * 				__asm_list__("%eax", "cc"),
 * 				__asm_list__(__asm_in__(instant_val, i, 1), __asm_in__(c_var, r)),
 * 				__asm_list__(__asm_out__(result, r)),
 * 				"mov %[instant_val], %%eax",
 * 				"add %[c_var], %%eax",
 * 				"mov %%eax, %[result]"
 * 			)
 * 			该宏展开后为
 *			__asm__ __volatile__(
 *				"mov %[instant_val], %%eax"
 *				"add %[c_var], %%eax"
 *				"mov %%eax, %[result]"
 *				: [result] "=&r" (result)
 *				: [instant_val] "i" (1), [c_var] "r" (c_var)
 *				: "%eax", "cc"
 *			)
 *			如果out_flag为jmp，则asm_out_list为C/C++的goto跳转标签列表
 */
#define __asm_inline__(optimize_flag, out_flag) __cats__(4)(__asm_inline__, optimize_flag, _, out_flag)

#endif //_CXXLLO_ASM
