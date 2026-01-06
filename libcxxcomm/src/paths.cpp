#include <andromeda/io/files.h>

#include <string.h>

const char* andromeda::io::get_filename(const char* path)
{
	char* win32_filename = strrchr(path, WIN32_PATH_SEPARATOR) + 1;
	char* unix_filename = strrchr(path, UNIX_PATH_SEPARATOR) + 1;
	if(win32_filename || unix_filename)
	{
		if(win32_filename > unix_filename)
			return win32_filename;
		else
			return unix_filename;
	}
	else
		return path;
}
