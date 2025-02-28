#include <iostream>

#include "exceptions/file_reader_exceptions.hpp"
#include "file_reader/file_reader.hpp"

int main(int argc, char* argv[]) {
  if (argc != 2) {
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