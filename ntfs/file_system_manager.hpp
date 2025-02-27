#pragma once

#include <stdexcept>
#include <string>
#include <vector>

class FileSystemManager {
 public:
  FileSystemManager(const std::string& path);
  std::vector<std::string> listFiles(bool recursive = false) const;
  bool isDirectory(const std::string& path) const;
  std::string getPath() const;

 private:
  std::string basePath;
  void listFilesRecursive(const std::string& path,
                          std::vector<std::string>& files) const;
};
