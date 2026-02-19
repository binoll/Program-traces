#include "xml_parser.hpp"

namespace EventLogAnalysis {

std::unordered_map<std::string, std::string> XmlEventParser::parseEventData(
    const std::string& xml) {
  std::unordered_map<std::string, std::string> result;

  try {
    auto it = std::sregex_iterator(xml.begin(), xml.end(), DATA_REGEX);

    for (auto end = std::sregex_iterator(); it != end; ++it) {
      if (const std::smatch& match = *it; match.size() == 3) {
        std::string name = match[1].str();
        std::string value = decodeXmlEntities(match[2].str());
        result.emplace(std::move(name), std::move(value));
      }
    }
  } catch (const std::regex_error&) {
    // Игнорируем ошибки regex
  }

  return result;
}

std::string XmlEventParser::parseDescription(const std::string& xml) {
  try {
    std::smatch match;
    if (std::regex_search(xml, match, DESC_REGEX) && match.size() > 1) {
      return decodeXmlEntities(match[1].str());
    }
  } catch (const std::regex_error&) {
    // Игнорируем ошибки regex
  }

  return "";
}

std::string XmlEventParser::decodeXmlEntities(std::string text) {
  for (const auto& [entity, replacement] : XML_ENTITIES) {
    size_t pos = 0;
    while ((pos = text.find(entity, pos)) != std::string::npos) {
      text.replace(pos, entity.length(), replacement);
      pos += replacement.length();
    }
  }
  return text;
}

}
