#include "file_util.hpp"
#include <filesystem>
#include <fstream>

namespace zen::util {
std::vector<char> read_file_binary_data(const std::string &file_name) {

  // Open stream at the end of the file to read it's size.
  auto full_path = std::string(ZEN_SHADER_PATH) + "/" + file_name;
  std::ifstream file(full_path.c_str(), std::ios::ate | std::ios::binary | std::ios::in);

  if (!file) {
    throw std::runtime_error("Error: Could not open file " + full_path + "!");
  }

  // Read the size of the file
  const auto file_size = file.tellg();

  std::vector<char> buffer(file_size);

  // Set the file read position to the beginning of the file
  file.seekg(0);

  file.read(buffer.data(), file_size);

  return buffer;
}
}