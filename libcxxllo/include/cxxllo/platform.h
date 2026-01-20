#ifndef _CXXLLO_PLATFORM
#define _CXXLLO_PLATFORM

#if defined (_WIN32) || defined (_WIN64)
#define __PLATFORM_WIN32__
#endif

#if defined(__unix__) || defined(__unix) || defined(unix)
#define __PLATFORM_UNIX__
#endif

#endif //_CXXLLO_PLATFORM
