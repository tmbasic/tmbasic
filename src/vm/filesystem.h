#pragma once

#include "../common.h"

namespace vm {

std::vector<std::string> listFiles(const std::string& path);
std::vector<std::string> listDirectories(const std::string& path);
void createDirectory(const std::string& path);
void deleteDirectory(const std::string& path, bool recursive);
std::string getTempFilePath(const std::string& filename);
void deleteFile(const std::string& path);

}  // namespace vm
