#ifndef ANDROMEDA_PLATFORM_PLATFORM
#define ANDROMEDA_PLATFORM_PLATFORM

#if defined (_WIN32) || defined (_WIN64)
#define __PLATFORM_WIN__
#endif

#if defined(__unix__) || defined(__unix) || defined(unix)
#define __PLATFORM_UNIX__
#endif

#endif //ANDROMEDA_PLATFORM_PLATFORM
