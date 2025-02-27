#pragma once

#include <string>

class FileAnalyzer {
 public:
  FileAnalyzer(const std::string& filePath);
  void analyze() const;

 private:
  std::string filePath;
  void countLinesAndWords(int& lines, int& words) const;
};
