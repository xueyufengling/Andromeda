#ifndef ANDROMEDA_IO_PATHS
#define ANDROMEDA_IO_PATHS

#define WIN32_PATH_SEPARATOR '\\'
#define UNIX_PATH_SEPARATOR '/'

#ifdef _WIN32
#define PATH_SEPARATOR WIN32_PATH_SEPARATOR
#else
#define PATH_SEPARATOR UNIX_PATH_SEPARATOR
#endif

namespace andromeda
{
namespace io
{
const char* get_filename(const char* path);
}
}

#endif //ANDROMEDA_IO_PATHS
