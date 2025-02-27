#include "file_system_manager.hpp"
#include <dirent.h>
#include <sys/stat.h>
#include <iostream>

FileSystemManager::FileSystemManager(const std::string& path)
    : basePath(path) {}

std::vector<std::string> FileSystemManager::listFiles(bool recursive) const {
  std::vector<std::string> files;
  if (recursive) {
    listFilesRecursive(basePath, files);
  } else {
    DIR* dir;
    struct dirent* ent;
    if ((dir = opendir(basePath.c_str())) != nullptr) {
      while ((ent = readdir(dir)) != nullptr) {
        std::string fileName = ent->d_name;
        if (fileName != "." && fileName != "..") {
          files.push_back(basePath + "/" + fileName);
        }
      }
      closedir(dir);
    } else {
      throw std::runtime_error("Could not open directory: " + basePath);
    }
  }
  return files;
}

bool FileSystemManager::isDirectory(const std::string& path) const {
  struct stat statbuf;
  if (stat(path.c_str(), &statbuf) != 0)
    return false;
  return S_ISDIR(statbuf.st_mode);
}

std::string FileSystemManager::getPath() const {
  return basePath;
}

void FileSystemManager::listFilesRecursive(
    const std::string& path, std::vector<std::string>& files) const {
  DIR* dir;
  struct dirent* ent;
  if ((dir = opendir(path.c_str())) != nullptr) {
    while ((ent = readdir(dir)) != nullptr) {
      std::string fileName = ent->d_name;
      if (fileName != "." && fileName != "..") {
        std::string fullPath = path + "/" + fileName;
        files.push_back(fullPath);
        if (isDirectory(fullPath)) {
          listFilesRecursive(fullPath, files);
        }
      }
    }
    closedir(dir);
  } else {
    throw std::runtime_error("Could not open directory: " + path);
  }
}