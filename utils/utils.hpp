#pragma once

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

/// @brief Разделяет строку на подстроки по указанному разделителю
/// @param str Исходная строка для разделения
/// @param delimiter Символ-разделитель
/// @return Вектор подстрок
static std::vector<std::string> split(const std::string& str, char delimiter);

/// @brief Удаляет пробельные символы в начале и конце строки
/// @param str Строка для обработки (изменяется на месте)
static void trim(std::string& str);

/// @brief Извлекает последний компонент из пути
/// @param path Путь в файловой системе
/// @param separator Разделитель пути
/// @return Последний компонент пути
static std::string getLastPathComponent(const std::string& path,
                                        char separator);

/**
 * @brief Безопасно преобразует uint64_t в time_t с проверкой диапазона
 * @param time_value Временная метка в uint64_t формате
 * @return Временная метка в time_t формате
 * @throw std::overflow_error Если значение превышает допустимый диапазон time_t
 */
std::string convert_run_times(uint64_t time_value);

std::vector<std::string> split(const std::string& str, const char delimiter) {
  std::vector<std::string> tokens;
  std::istringstream token_stream(str);
  std::string token;

  while (std::getline(token_stream, token, delimiter)) {
    if (!token.empty()) {
      tokens.push_back(token);
    }
  }
  return tokens;
}

void trim(std::string& str) {
  auto is_space = [](const char c) {
    return std::isspace(static_cast<unsigned char>(c));
  };
  str.erase(str.begin(), std::ranges::find_if_not(str, is_space));
  str.erase(std::find_if_not(str.rbegin(), str.rend(), is_space).base(),
            str.end());
}

std::string getLastPathComponent(const std::string& path,
                                 const char separator) {
  if (path.empty()) return "";

  size_t end = path.length();
  while (end > 0 && path[end - 1] == separator) --end;
  if (end == 0) return "";

  const size_t start = path.find_last_of(separator, end - 1);
  return (start == std::string::npos) ? path.substr(0, end)
                                      : path.substr(start + 1, end - start - 1);
}

inline std::string convert_run_times(const uint64_t time_value) {
  // Проверка на переполнение при преобразовании в time_t
  constexpr uint64_t max_time_t = std::numeric_limits<time_t>::max();
  if (time_value > max_time_t) {
    throw std::overflow_error(
        "Преобразование uint64_t в time_t: значение вне допустимого диапазона");
  }

  // Конвертация в time_t
  const time_t time_val = static_cast<time_t>(time_value);

  // Преобразование в структуру tm (локальное время)
  std::tm* timeinfo = std::localtime(&time_val);
  if (!timeinfo) {
    throw std::runtime_error("Ошибка преобразования времени");
  }

  // Форматирование времени в строку
  std::ostringstream oss;
  oss << std::put_time(timeinfo, "%Y-%m-%d %H:%M:%S");
  return oss.str();
}
