#ifndef _CXXLLO_ASM
#define _CXXLLO_ASM

/**
 * 内嵌汇编拓展定义宏
 */

#include <cxxllo/architecture.h>
#include <cxxtricks/macros.h>

/**
 * @brief 只读的asm输入变量
 */
//仅限C/C++变量
#define __asm_in__2(var_name, constraint) __asm_in__3(var_name, constraint, var_name)
//支持所有约束，例如立即数
#define __asm_in__3(asm_var_name, constraint, value) [asm_var_name] __str__(constraint) (value)
#define __asm_in__(...) __macro_with_params__(__asm_in__, __VA_ARGS__)

/**
 * @brief 只写的asm输出变量
 */
#define __asm_out__2(var_name, constraint) __asm_out__3(var_name, constraint, var_name)
#define __asm_out__3(asm_var_name, constraint, var_name) [asm_var_name] "=&" __str__(constraint) (var_name)
#define __asm_out__(...) __macro_with_params__(__asm_out__, __VA_ARGS__)

/**
 * @brief 可读也可写的输入同时也是输出变量
 */
#define __asm_inout__2(var_name, constraint) __asm_inout__3(var_name, constraint, var_name)
#define __asm_inout__3(asm_var_name, constraint, var_name) [asm_var_name] "+" __str__(constraint) (var_name)
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
 * 				__asm_list__(__asm_in__(instant_val, I, 1), __asm_in__(c_var, r)),
 * 				__asm_list__(__asm_out__(result)),
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
 *				: [instant_val] "=&I" (1), [c_var] "=&r" (c_var)
 *				: "%eax", "cc"
 *			)
 *			如果out_flag为jmp，则asm_out_list为C/C++的goto跳转标签列表
 */
#define __asm_inline__(optimize_flag, out_flag) __cat_4__(__asm_inline__, optimize_flag, _, out_flag)

#endif //_CXXLLO_ASM
