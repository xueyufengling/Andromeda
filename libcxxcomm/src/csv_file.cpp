#include <cxxcomm/csv_file.h>

#include <rapidcsv.h>

rapidcsv::Document cxxcomm::read_csv(const std::string filename, char comma, csv_option option)
{
	return rapidcsv::Document(filename, rapidcsv::LabelParams(option & ROW_HEADER ? 0 : -1, option & COLUMN_HEADER ? 0 : -1), rapidcsv::SeparatorParams(comma));
}
