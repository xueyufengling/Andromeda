#ifndef _CXXLLO_PRIMITIVE
#define _CXXLLO_PRIMITIVE

/**
 * 指令级底层操作和原语
 */

#include <cxxllo/asm.h>

/**
 * @brief 加载值到指定位置
 * @param dest 目标位置
 * @param src 要加载的值，可以是立即值或C/C++变量
 */
#define __ld__(dest, src)\
		__asm_inline__(optimized, var)(\
			__asm_list__(),\
			__asm_list__(__asm_in__(_mov_src, r, src)),\
			__asm_list__(),\
			"mov %[_mov_src], %" __str__(dest)\
		)

/**
 * @brief 加载值到寄存器，可以是立即值或C/C++变量
 */
#define __Rl__(reg_name, src) __ld__(%reg_name, src)

/**
 * @brief 从指定位置读取值
 * @param src 要读取的位置
 * @param dest 目标位置，必须是C/C++变量
 */
#define __st__(dest, src)\
		__asm_inline__(volatile, var)(\
			__asm_list__(),\
			__asm_list__(),\
			__asm_list__(__asm_out__(dest, r)),\
			"mov %" __str__(src) ", %" __str__([dest])\
		)

/**
 * @brief 将寄存器的值存储到指定C/C++变量
 */
#define __Rs__(reg_name, dest) __st__(dest, %reg_name)

/**
 * @brief 编译器屏障，强制编译器在优化阶段指令重排时在此语句之前完成内存写入，且此语句之后的内存读取不得提前。
 * 		  原理：使用memory标志通知编译器此语句会破坏内存，防止指令重排后内存访问顺序更改
 */
__attribute__((always_inline)) inline void __cb()
{
	__asm_inline__(volatile, var)(
			__asm_list__("memory"),
			__asm_list__(),
			__asm_list__(),
			""
	);
}

/**
 * @brief 全内存屏障
 */
__attribute__((always_inline)) inline void __mb()
{
#if defined(__ARCHITECTURE_X86_64__) || defined(__ARCHITECTURE_X86__)
	__asm_inline__(volatile, var)(
			__asm_list__("memory"),
			__asm_list__(),
			__asm_list__(),
			"mfence"
	);
#elif defined(__ARCHITECTURE_AARCH_64__)
	__asm_inline__(volatile, var)(
			__asm_list__("memory"),
			__asm_list__(),
			__asm_list__(),
			"dmb sy"
	);
#elif defined(__ARCHITECTURE_POWERPC_64__)
	__asm_inline__(volatile, var)(
			__asm_list__("memory"),
			__asm_list__(),
			__asm_list__(),
			"eieio"
	);
#else
	static_assert(false, "no full memory barrier instruction for current architecture");
#endif
}

/**
 * @brief 读内存屏障
 */
__attribute__((always_inline)) inline void __rmb()
{
#if defined(__ARCHITECTURE_X86_64__) || defined(__ARCHITECTURE_X86__)
	__asm_inline__(volatile, var)(
			__asm_list__("memory"),
			__asm_list__(),
			__asm_list__(),
			"lfence"
	);
#elif defined(__ARCHITECTURE_AARCH_64__)
	__asm_inline__(volatile, var)(
			__asm_list__("memory"),
			__asm_list__(),
			__asm_list__(),
			"dmb ld"
	);
#else
	static_assert(false, "no read memory barrier instruction for current architecture");
#endif
}

/**
 * @brief 写内存屏障
 */
__attribute__((always_inline)) inline void __wmb()
{
#if defined(__ARCHITECTURE_X86_64__) || defined(__ARCHITECTURE_X86__)
	__asm_inline__(volatile, var)(
			__asm_list__("memory"),
			__asm_list__(),
			__asm_list__(),
			"sfence"
	);
#elif defined(__ARCHITECTURE_AARCH_64__)
	__asm_inline__(volatile, var)(
			__asm_list__("memory"),
			__asm_list__(),
			__asm_list__(),
			"dmb st"
	);
#else
	static_assert(false, "no write memory barrier instruction for current architecture");
#endif
}

#endif //_CXXLLO_PRIMITIVE
