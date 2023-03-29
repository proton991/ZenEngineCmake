#ifndef ZENENGINE_FILE_UTIL_HPP
#define ZENENGINE_FILE_UTIL_HPP
#include <string>
#include <vector>

namespace zen::util {
std::vector<char> read_file_binary_data(const std::string& file_name);

}
#endif  //ZENENGINE_FILE_UTIL_HPP
