#ifndef _CXXCOMM_CSVFILE
#define _CXXCOMM_CSVFILE

/**
 * 需要repidcsv库为依赖
 * https://github.com/d99kris/rapidcsv/tree/master
 */
#include <string>

namespace rapidcsv
{
class Document;
}

namespace cxxcomm
{
enum csv_option : unsigned char
{
	NO_HEADER = 0b00000000, ROW_HEADER = 0b00000001, COLUMN_HEADER = 0b00000010, BOTH_HEADER = ROW_HEADER | COLUMN_HEADER
};

rapidcsv::Document read_csv(const std::string filename, char comma = ',', csv_option option = BOTH_HEADER);
}

#endif //_CXXCOMM_CSVFILE
