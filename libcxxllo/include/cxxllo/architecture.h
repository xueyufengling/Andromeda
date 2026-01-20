#ifndef _CXXLLO_ARCHIHTECTURE
#define _CXXLLO_ARCHIHTECTURE

#if defined(__alpha__) || defined(__alpha) || defined(_M_ALPHA)
#define __ARCHITECTURE_ALPHA__
#endif

#if defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) || defined(__amd64__) || defined(__amd64)
//x86 64位
#define __ARCHITECTURE_X86_64__
#endif

#if defined(__i386__) || defined(__i386) || defined(i386) || defined(_M_IX86) || defined(_M_I86) || defined(__IA32__) || defined(__X86__) || defined(_X86_)
//x86 32位
#define __ARCHITECTURE_X86__
#endif

#if defined(__aarch64__) || defined(_M_ARM) || defined(_M_ARM64)
#define __ARCHITECTURE_AARCH_64__
#endif

#if defined(__powerpc) || defined(__powerpc__) || defined(__powerpc64__) || defined(__POWERPC__) || defined(__ppc__) || defined(__ppc64__) || defined(__PPC__) || defined(__PPC64__) || defined(_ARCH_PPC) || defined(_ARCH_PPC64) || defined(_M_PPC)
#define __ARCHITECTURE_POWERPC_64__
#endif

#endif //_CXXLLO_ARCHIHTECTURE
