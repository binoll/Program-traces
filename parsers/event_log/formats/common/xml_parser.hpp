/// @file xml_parser.hpp
/// @brief Утилиты для парсинга XML в событиях Windows

#pragma once

#include <regex>
#include <string>
#include <unordered_map>

/// @namespace EventLogAnalysis
/// @brief Пространство имен для работы с журналами событий Windows
namespace EventLogAnalysis {

/// @class XmlEventParser
/// @brief Парсер XML данных из событий Windows
class XmlEventParser {
 public:
  /// @brief Извлекает данные события из XML
  /// @param xml Строка с XML-представлением события
  /// @return Словарь с извлеченными данными (имя параметра -> значение)
  static std::unordered_map<std::string, std::string> parseEventData(
      const std::string& xml);

  /// @brief Извлекает описание события из XML
  /// @param xml Строка с XML-представлением события
  /// @return Строка с описанием события или пустая строка если не найдено
  static std::string parseDescription(const std::string& xml);

  /// @brief Декодирует XML entities
  /// @param text Текст с XML-сущностями для декодирования
  /// @return Текст с декодированными XML-сущностями
  static std::string decodeXmlEntities(std::string text);

 private:
  inline static const std::regex DATA_REGEX{
      R"xml(<Data\s+Name="([^"]+)">([^<]*)</Data>)xml"};  ///< Регулярное
                                                          ///< выражение для
                                                          ///< поиска тегов
                                                          ///< <Data>

  inline static const std::regex DESC_REGEX{
      R"xml(<Description>([^<]+)</Description>)xml"};  ///< Регулярное выражение
                                                       ///< для поиска тега
                                                       ///< <Description>

  inline static const std::unordered_map<std::string, std::string> XML_ENTITIES{
      {"&amp;", "&"},
      {"&lt;", "<"},
      {"&gt;", ">"},
      {"&quot;", "\""},
      {"&apos;", "'"}};  ///< Карта XML-сущностей для декодирования
};

}
