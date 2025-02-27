#include "file_analyzer.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

FileAnalyzer::FileAnalyzer(const std::string& filePath) : filePath(filePath) {}

void FileAnalyzer::analyze() const {
  int lines = 0, words = 0;
  countLinesAndWords(lines, words);
  std::cout << "File: " << filePath << "\nLines: " << lines
            << "\nWords: " << words << std::endl;
}

void FileAnalyzer::countLinesAndWords(int& lines, int& words) const {
  std::ifstream file(filePath);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open file: " + filePath);
  }
  std::string line;
  while (std::getline(file, line)) {
    ++lines;
    std::istringstream iss(line);
    std::string word;
    while (iss >> word) {
      ++words;
    }
  }
}