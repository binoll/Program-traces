#include <iostream>
#include "ntfs/file_analyzer.hpp"
#include "ntfs/file_system_manager.hpp"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <path> [--recursive]" << std::endl;
    return 1;
  }

  std::string path = argv[1];
  bool recursive = (argc > 2 && std::string(argv[2]) == "--recursive");

  try {
    FileSystemManager fsManager(path);
    auto files = fsManager.listFiles(recursive);

    for (const auto& file : files) {
      if (!fsManager.isDirectory(file)) {
        FileAnalyzer analyzer(file);
        analyzer.analyze();
      }
    }
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}