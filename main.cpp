#include <iostream>

#include "exceptions/file_reader_exceptions.hpp"
#include "file_reader/file_reader.hpp"

void print_usage(const char* program_name) {
  std::cerr << "NTFS File Reader\n"
            << "Usage: " << program_name << " <file_path>\n\n"
            << "Ensure:\n"
            << "1. NTFS drive is properly mounted\n"
            << "2. You have read permissions\n"
            << "3. Path uses Linux-style separators (/)\n";
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    print_usage(argv[0]);
    return 1;
  }

  try {
    FileReader reader(argv[1]);
    reader.open(std::ios::in);

    if (reader.is_open()) {
      std::cout << "Successfully opened: " << reader.path() << "\n";
      std::cout << "File content:\n" << reader.read() << "\n";
    }
  } catch (const FileOpenException& e) {
    std::cerr << "Critical error: " << e.what() << "\n";
    return 2;
  } catch (const FileReadException& e) {
    std::cerr << "Read failure: " << e.what() << "\n";
    return 3;
  } catch (const std::exception& e) {
    std::cerr << "Unexpected error: " << e.what() << "\n";
    return 4;
  }

  return 0;
}