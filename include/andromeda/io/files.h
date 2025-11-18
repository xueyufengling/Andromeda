#ifndef ANDROMEDA_IO_IOUTILS
#define ANDROMEDA_IO_IOUTILS

#include <fstream>
#include <string.h>

#define WIN32_PATH_SEPARATOR '\\'
#define UNIX_PATH_SEPARATOR '/'

#ifdef _WIN32
#define PATH_SEPARATOR WIN32_PATH_SEPARATOR
#else
#define PATH_SEPARATOR UNIX_PATH_SEPARATOR
#endif

namespace rapidcsv
{
class Document;
}

namespace andromeda
{
namespace io
{
/**
 * @brief 以二进制读取文件内容到内存
 * @param filename 文件路径
 * @param data_length 记录返回数组长度，读取到的文件实际大小
 * @param read_length read_length<0则读取全部内容
 * @param reserve_length 读取完毕后内存末尾多分配的空字节数
 */
extern unsigned char* read(std::string filename, size_t* data_length, long long int read_length = -1, size_t reserve_length = 0);

/**
 * @brief 以文本形式读取文件内容
 * @param filename 文件路径
 * @param read_length read_length=<0则读取全部内容
 * @param reserve_length 读取完毕后内存末尾多分配的空字节数
 */
extern std::string read_string(std::string filename, long long int read_length = -1, size_t reserve_length = 0);

extern void write(std::string filename, void* data, size_t length);

extern void append(std::string filename, void* data, size_t length);

extern std::string directory_of(std::string file_path);

inline void clear(std::string file_name)
{
	std::ofstream f(file_name, std::ios::out | std::ios::binary);
	f.write(nullptr, 0);
	f.close();
}

enum csv_option : unsigned char
{
	NO_HEADER = 0b00000000, ROW_HEADER = 0b00000001, COLUMN_HEADER = 0b00000010, BOTH_HEADER = ROW_HEADER | COLUMN_HEADER
};

rapidcsv::Document read_csv(std::string filename, char comma = ',', csv_option option = BOTH_HEADER);
}
}

#endif // ANDROMEDA_IO_IOUTILS
