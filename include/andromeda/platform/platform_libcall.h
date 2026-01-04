#ifndef ANDROMEDA_PLATFORM_PLATFORMLIBCALL
#define ANDROMEDA_PLATFORM_PLATFORMLIBCALL

#include "platform.h"
#include "../internal/clib_call.h"

#define platform_call(level, callable, ...) callable(__VA_ARGS__)

#if defined(__PLATFORM_WIN__)

#include <winerror.h>
#include <errhandlingapi.h>

decl_clibcall(windows, WINBOOL, GetLastError, ERROR_SUCCESS)

#define windows_call(level, callable, ...) clibcall(windows, level, __log_source__, callable, ##__VA_ARGS__)

#define platform_call(level, callable, ...) windows_call(level, callable, ##__VA_ARGS__)

/**
 * 常用函数的lib call
 */
#define win_SymInitialize(hProcess, UserSearchPath, fInvadeProcess) windows_call(andromeda::log::log_level::LOG_FATAL, SymInitialize, hProcess, UserSearchPath, fInvadeProcess)

#define win_SymCleanup(hProcess) windows_call(andromeda::log::log_level::LOG_FATAL, SymCleanup, hProcess)

#define win_GetCurrentProcess() windows_call(andromeda::log::log_level::LOG_DEBUG_ERROR, GetCurrentProcess)

#define win_SymFromAddr(hProcess, Address, Displacement, Symbol) windows_call(andromeda::log::log_level::LOG_DEBUG_ERROR, SymFromAddr, hProcess, Address, Displacement, Symbol)

#define win_GetModuleFileNameExA(hProcess, hModule, lpFilename, nSize) windows_call(andromeda::log::log_level::LOG_DEBUG_ERROR, GetModuleFileNameExA, hProcess, hModule, lpFilename, nSize)

#endif

#endif //ANDROMEDA_PLATFORM_PLATFORMLIBCALL
