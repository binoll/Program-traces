#include <ctime>
#include <iomanip>
#include <iostream>
#include "prefetch_file_analysis/prefetch_file_parser.hpp"

// Helper function to convert Windows FILETIME to time_t
time_t filetime_to_timet(uint64_t filetime) {
    if (filetime == 0) {
        return 0;
    }
    // Convert 100-ns intervals to seconds since 1601-01-01
    filetime /= 10000000ULL;
    // Subtract difference between 1601 and 1970 (11644473600 seconds)
    return static_cast<time_t>(filetime - 11644473600ULL);
}

void printPrefetchInfo(const PrefetchAnalysis::PrefetchData& info) {
    std::cout << "Executable: " << info.executable_name() << "\n"
              << "Prefetch Hash: 0x" << std::hex << info.prefetch_hash() << "\n"
              << "Run Count: " << std::dec << info.run_count() << "\n\n"
              << "Last Run Times:\n";

    for (const auto& time : info.run_times()) {
        std::cout << "  "
                  << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
                  << "\n";
    }

    std::cout << "\nVolumes:\n";
    for (const auto& vol : info.volumes()) {
        time_t creation_time = filetime_to_timet(vol.creation_time());
        std::cout << "  Device: " << vol.device_path() << "\n"
                  << "  Serial: 0x" << std::hex << vol.serial_number() << "\n"
                  << "  Created: "
                  << std::put_time(std::localtime(&creation_time), "%Y-%m-%d %H:%M:%S")
                  << "\n\n";
    }

    std::cout << "Tracked Files:\n";
    for (const auto& metric : info.metrics()) {
        std::cout << "  " << metric.filename() << " [Ref: 0x" << std::hex
                  << metric.file_reference() << "]\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <prefetch_file>\n";
        return 1;
    }

    try {
        PrefetchAnalysis::PrefetchParser parser;
        PrefetchAnalysis::PrefetchData info = parser.parse(argv[1]);
        printPrefetchInfo(info);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 2;
    }

    return 0;
}