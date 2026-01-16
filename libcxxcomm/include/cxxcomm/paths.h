#ifndef _CXXCOMM_PATHS
#define _CXXCOMM_PATHS

#define WIN32_PATH_SEPARATOR '\\'
#define UNIX_PATH_SEPARATOR '/'

#ifdef _WIN32
#define PATH_SEPARATOR WIN32_PATH_SEPARATOR
#else
#define PATH_SEPARATOR UNIX_PATH_SEPARATOR
#endif

namespace cxxcomm
{
const char* get_filename(const char* path);
}

#endif //_CXXCOMM_PATHS
