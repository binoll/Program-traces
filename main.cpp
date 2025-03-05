#include <iomanip>
#include <iostream>
#include "file_reader/file_reader.hpp"
#include "file_reader/file_reader_exceptions.hpp"

void process_chunk(const std::vector<char>& chunk, size_t& total_size) {
  total_size += chunk.size();
}

int main(const int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <disk_path>\n";
    return 1;
  }

  const std::string disk_path(argv[1]);
  size_t total_bytes = 0;
  size_t chunk_counter = 0;

  try {
    FileReader reader(disk_path, std::ios::binary);

    std::cout << "Analyzing disk: " << disk_path << "\n";
    std::cout << std::setw(6) << "Chunk" << std::setw(12) << "Size (MB)"
              << std::setw(15) << "Total (MB)\n";

    while (true) {
      auto chunk = reader.read_chunk(1024 * 1024);
      if (chunk.empty())
        break;

      process_chunk(chunk, total_bytes);

      // Вывод прогресса каждые 100 чанков
      if (++chunk_counter % 100 == 0) {
        std::cout << std::setw(6) << chunk_counter << std::setw(12)
                  << chunk.size() / 1048576.0 << std::setw(15)
                  << total_bytes / 1048576.0 << "\n";
      }
    }

    std::cout << "\nAnalysis complete!\n"
              << "Total processed: " << total_bytes << " bytes (" << std::fixed
              << std::setprecision(2) << total_bytes / 1048576.0 << " MB)\n";

  } catch (const FileException& e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 2;
  }

  return 0;
}
