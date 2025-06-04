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

namespace rapidcsv{
	class Document;
}

namespace andromeda {
	namespace io {
		extern unsigned char* readBytesFromFile(std::string filename,size_t* data_length,long long int read_length=-1,size_t reserve_length=0); //data_length记录返回数组长度，read_length<0则读取全部内容，reserve_length为读取完毕后内存保留空字节数
		extern const char* readStringFromFile(std::string filename,size_t* str_length=nullptr,long long int read_length=-1,size_t reserve_length=0); //str_length记录返回字符串长度不含'\0'，read_length=<0则读取全部内容，reserve_length为内存保留字节数
		extern void writeBytesToFile(std::string filename,void* data,size_t length);
		extern void appendBytesToFile(std::string filename,void* data,size_t length);

		extern std::string getFileDir(std::string file_path);

		inline void clearFile(std::string file_name)
		{
			std::ofstream f(file_name,std::ios::out|std::ios::binary);
			f.write(nullptr,0);
			f.close();
		}

		enum CsvOption : unsigned char
		{
			NO_HEADER=0b00000000, ROW_HEADER=0b00000001, COLUMN_HEADER=0b00000010, BOTH_HEADER=ROW_HEADER|COLUMN_HEADER
		};

		rapidcsv::Document readCsv(std::string filename,char comma=',',CsvOption option=BOTH_HEADER);
	}
}

#endif // ANDROMEDA_IO_IOUTILS
