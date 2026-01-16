#ifndef _CXXSCI_LITERALDEFS
#define _CXXSCI_LITERALDEFS

#include <stdint.h>

//屏蔽编译器警告
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wliteral-suffix"

inline constexpr uint8_t operator "" u8(unsigned long long int value) noexcept
{
	return (uint8_t)value;
}

inline constexpr uint16_t operator "" u16(unsigned long long int value) noexcept
{
	return (uint16_t)value;
}

inline constexpr uint32_t operator "" u32(unsigned long long int value) noexcept
{
	return (uint32_t)value;
}

inline constexpr uint64_t operator "" u64(unsigned long long int value) noexcept
{
	return (uint64_t)value;
}

#pragma GCC diagnostic pop

#endif //_CXXSCI_LITERALDEFS
