/**
 * @file PrefetchParser.cpp
 * @brief Реализация парсера Prefetch файлов
 */

#include "prefetch_parser.hpp"
#include <iostream>
#include <sstream>

PrefetchParser::PrefetchParser() : sccaFile(nullptr), sccaError(nullptr) {
    if (libscca_file_initialize(&sccaFile, &sccaError) != 1) {
        handleError("Ошибка инициализации libscca");
    }
}

PrefetchParser::~PrefetchParser() {
    if (sccaFile) {
        libscca_file_free(&sccaFile, &sccaError);
    }
}

PrefetchInfo PrefetchParser::parse(const std::string& filePath) {
    PrefetchInfo info;

    // Открытие файла
    if (libscca_file_open(sccaFile, filePath.c_str(), LIBSCCA_ACCESS_FLAG_READ, &sccaError) != 1) {
        handleError("Ошибка открытия файла");
    }

    // Получение основной информации
    char buffer[1024];
    size_t nameSize;

    // Имя исполняемого файла
    libscca_file_get_utf8_executable_filename_size(sccaFile, &nameSize, &sccaError);
    libscca_file_get_utf8_executable_filename(sccaFile, reinterpret_cast<uint8_t*>(buffer), nameSize, &sccaError);
    info.executableName = buffer;

    // Хэш Prefetch
    libscca_file_get_prefetch_hash(sccaFile, &info.prefetchHash, &sccaError);

    // Количество запусков
    libscca_file_get_run_count(sccaFile, &info.runCount, &sccaError);

    // Времена последних запусков
    for (int i = 0; ; i++) {
        uint64_t filetime;
        if (libscca_file_get_last_run_time(sccaFile, i, &filetime, &sccaError) != 1) break;
        info.lastRunTimes.push_back(filetimeToTimeT(filetime));
    }

    // Информация о томах
    int volumesCount;
    libscca_file_get_number_of_volumes(sccaFile, &volumesCount, &sccaError);
    for (int i = 0; i < volumesCount; i++) {
        libscca_volume_information_t* volume;
        libscca_file_get_volume_information(sccaFile, i, &volume, &sccaError);

        VolumeInfo volInfo;
        libscca_volume_information_get_creation_time(volume, &volInfo.creationTime, &sccaError);
        libscca_volume_information_get_serial_number(volume, &volInfo.serialNumber, &sccaError);

        libscca_volume_information_get_utf8_device_path_size(volume, &nameSize, &sccaError);
        libscca_volume_information_get_utf8_device_path(volume, reinterpret_cast<uint8_t*>(buffer), nameSize, &sccaError);
        volInfo.devicePath = buffer;

        info.volumes.push_back(volInfo);
        libscca_volume_information_free(&volume, &sccaError);
    }

    // Метрики файлов
    int metricsCount;
    libscca_file_get_number_of_file_metrics_entries(sccaFile, &metricsCount, &sccaError);
    for (int i = 0; i < metricsCount; i++) {
        libscca_file_metrics_t* metric;
        libscca_file_get_file_metrics_entry(sccaFile, i, &metric, &sccaError);

        FileMetric fileMetric;
        libscca_file_metrics_get_utf8_filename_size(metric, &nameSize, &sccaError);
        libscca_file_metrics_get_utf8_filename(metric, reinterpret_cast<uint8_t*>(buffer), nameSize, &sccaError);
        fileMetric.filename = buffer;

        libscca_file_metrics_get_file_reference(metric, &fileMetric.fileReference, &sccaError);

        info.fileMetrics.push_back(fileMetric);
        libscca_file_metrics_free(&metric, &sccaError);
    }

    return info;
}

time_t PrefetchParser::filetimeToTimeT(uint64_t filetime) {
    return static_cast<time_t>((filetime - 116444736000000000ULL) / 10000000ULL);
}

void PrefetchParser::handleError(const std::string& context) {
    std::stringstream ss;
    if (sccaError) {
        char errorBuffer[1024];
        libscca_error_sprint(sccaError, errorBuffer, sizeof(errorBuffer));
        ss << context << ": " << errorBuffer;
        libscca_error_free(&sccaError);
    } else {
        ss << context << ": Неизвестная ошибка";
    }
    throw std::runtime_error(ss.str());
}
