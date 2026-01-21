#ifndef _CXXLLO_PRIMITIVE
#define _CXXLLO_PRIMITIVE

/**
 * 指令级底层操作和原语
 */
#include <stdint.h>
#include <cxxllo/asm.h>

/**
 * @brief 加载值到指定位置
 * @param dest 目标位置
 * @param src 要加载的值，可以是立即值或C/C++变量，这些src值都会被加载进临时寄存器，随后再从临时寄存器移入目标位置
 */
#define __ld(dest, src)\
		__asm_inline__(optimized, var)(\
			__asm_list__(),\
			__asm_list__(__asm_in__(_mov_src, r, src)),\
			__asm_list__(),\
			"mov %[_mov_src], %" __str__(dest)\
		)

/**
 * @brief 加载值到寄存器，可以是立即值或C/C++变量
 */
#define __Rl(reg_name, src) __ld(%reg_name, src)

/**
 * @brief 从指定位置读取值
 * @param src 要读取的位置
 * @param dest 目标位置，必须是C/C++变量
 */
#define __st(dest, src)\
		__asm_inline__(volatile, var)(\
			__asm_list__(),\
			__asm_list__(),\
			__asm_list__(__asm_out__(dest, m)),\
			"mov %" __str__(src) ", %" __str__([dest])\
		)

/**
 * @brief 将寄存器的值存储到指定C/C++变量
 */
#define __Rs(reg_name, dest) __st(dest, %reg_name)

/**
 * @brief 编译器屏障，强制编译器在优化阶段指令重排时在此语句之前完成内存写入，且此语句之后的内存读取不得提前。
 * 		  原理：使用memory标志通知编译器此语句会破坏内存，防止指令重排后内存访问顺序更改
 */
__attribute__((always_inline)) inline void __cb(void)
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
__attribute__((always_inline)) inline void __rwmb(void)
{
	__asm_inline__(volatile, var)(
			__asm_list__("memory"),
			__asm_list__(),
			__asm_list__(),
#if defined(__ARCHITECTURE_X86_64__) || defined(__ARCHITECTURE_X86__)
			"mfence"
#elif defined(__ARCHITECTURE_AARCH_64__)
			"dmb sy"
#elif defined(__ARCHITECTURE_POWERPC_64__)
			"lwsync"
#else
			static_assert(false, "no full memory barrier instruction for current architecture");
#endif
	);
}

/**
 * @brief 读内存屏障
 */
__attribute__((always_inline)) inline void __rmb(void)
{
	__asm_inline__(volatile, var)(
			__asm_list__("memory"),
			__asm_list__(),
			__asm_list__(),
#if defined(__ARCHITECTURE_X86_64__) || defined(__ARCHITECTURE_X86__)
			"lfence"
#elif defined(__ARCHITECTURE_AARCH_64__)
			"dmb ld"
#else
			static_assert(false, "no read memory barrier instruction for current architecture");
#endif
	);
}

/**
 * @brief 写内存屏障
 */
__attribute__((always_inline)) inline void __wmb(void)
{
	__asm_inline__(volatile, var)(
			__asm_list__("memory"),
			__asm_list__(),
			__asm_list__(),
#if defined(__ARCHITECTURE_X86_64__) || defined(__ARCHITECTURE_X86__)
			"sfence"
#elif defined(__ARCHITECTURE_AARCH_64__)
			"dmb st"
#else
			static_assert(false, "no write memory barrier instruction for current architecture");
#endif
	);
}

/**
 * @brief	比较并交换1、2、4、8字节的值
 * @detail	待比较的期望值必须送入寄存器a，这是因为cmpxchg指令的比较操作数只从寄存器a取。src操作数必须是寄存器
 * 			注：不使用inline函数是因为inline函数虽然内联函数体，但每个inline内联展开的参数和局部变量都会独立地、额外地占用栈空间，且也会进行对这些栈内存进行初始化和参数传递，从而生成一堆不必要的mov指令
 */
#if defined(__ARCHITECTURE_X86_64__) || defined(__ARCHITECTURE_X86__)
#define __cmpxchgb(dest, expected, src, prev)\
		__asm_inline__(volatile, var)(\
				__asm_list__("memory", "cc"),\
				__asm_list__(__asm_in__(_expected, a, (uint8_t)expected), __asm_in__(_src, r, (uint8_t)src)),\
				__asm_list__(__asm_inout__(_dest, m, dest), __asm_out__(_prev, m, prev)),\
				"lock cmpxchgb %[_src], %[_dest]",\
				"movb %%al, %[_prev]"\
		)

#define __casb(dest, expected, src, success)\
		__asm_inline__(volatile, var)(\
				__asm_list__("memory", "cc"),\
				__asm_list__(__asm_in__(_expected, a, (uint8_t)expected), __asm_in__(_src, r, (uint8_t)src)),\
				__asm_list__(__asm_inout__(_dest, m, dest), __asm_out__(_success, m, success)),\
				"lock cmpxchgb %[_src], %[_dest]",\
				"setz %[_success]"\
		)

#define __cmpxchgw(dest, expected, src, prev)\
		__asm_inline__(volatile, var)(\
				__asm_list__("memory", "cc"),\
				__asm_list__(__asm_in__(_expected, a, (uint16_t)expected), __asm_in__(_src, r, (uint16_t)src)),\
				__asm_list__(__asm_inout__(_dest, m, dest), __asm_out__(_prev, m, prev)),\
				"lock cmpxchgw %[_src], %[_dest]",\
				"movw %%ax, %[_prev]"\
		)

#define __casw(dest, expected, src, success)\
		__asm_inline__(volatile, var)(\
				__asm_list__("memory", "cc"),\
				__asm_list__(__asm_in__(_expected, a, (uint16_t)expected), __asm_in__(_src, r, (uint16_t)src)),\
				__asm_list__(__asm_inout__(_dest, m, dest), __asm_out__(_success, m, success)),\
				"lock cmpxchgw %[_src], %[_dest]",\
				"setz %[_success]"\
		)

#define __cmpxchgl(dest, expected, src, prev)\
		__asm_inline__(volatile, var)(\
				__asm_list__("memory", "cc"),\
				__asm_list__(__asm_in__(_expected, a, (uint32_t)expected), __asm_in__(_src, r, (uint32_t)src)),\
				__asm_list__(__asm_inout__(_dest, m, dest), __asm_out__(_prev, m, prev)),\
				"lock cmpxchgl %[_src], %[_dest]",\
				"movl %%eax, %[_prev]"\
		)

#define __casl(dest, expected, src, success)\
		__asm_inline__(volatile, var)(\
				__asm_list__("memory", "cc"),\
				__asm_list__(__asm_in__(_expected, a, (uint32_t)expected), __asm_in__(_src, r, (uint32_t)src)),\
				__asm_list__(__asm_inout__(_dest, m, dest), __asm_out__(_success, m, success)),\
				"lock cmpxchgl %[_src], %[_dest]",\
				"setz %[_success]"\
		)

#define __cmpxchgq(dest, expected, src, prev)\
		__asm_inline__(volatile, var)(\
				__asm_list__("memory", "cc"),\
				__asm_list__(__asm_in__(_expected, a, (uint64_t)expected), __asm_in__(_src, r, (uint64_t)src)),\
				__asm_list__(__asm_inout__(_dest, m, dest), __asm_out__(_prev, m, prev)),\
				"lock cmpxchgq %[_src], %[_dest]",\
				"movq %%rax, %[_prev]"\
		)

#define __casq(dest, expected, src, success)\
		__asm_inline__(volatile, var)(\
				__asm_list__("memory", "cc"),\
				__asm_list__(__asm_in__(_expected, a, (uint64_t)expected), __asm_in__(_src, r, (uint64_t)src)),\
				__asm_list__(__asm_inout__(_dest, m, dest), __asm_out__(_success, m, success)),\
				"lock cmpxchgq %[_src], %[_dest]",\
				"setz %[_success]"\
		)
#endif

#endif //_CXXLLO_PRIMITIVE
