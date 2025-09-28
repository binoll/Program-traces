#include "xml_parser.hpp"

namespace EventLogAnalysis {

const std::regex XmlEventParser::DATA_REGEX(
    R"(<Data\s+Name="([^"]+)\"[^>]*>([^<]*)</Data>)");
const std::regex XmlEventParser::DESC_REGEX(
    R"(<Description>([^<]+)</Description>)");

const std::unordered_map<std::string, std::string>
    XmlEventParser::XML_ENTITIES = {{"&amp;", "&"},
                                    {"&lt;", "<"},
                                    {"&gt;", ">"},
                                    {"&quot;", "\""},
                                    {"&apos;", "'"}};

std::unordered_map<std::string, std::string> XmlEventParser::parseEventData(
    const std::string& xml) {
  std::unordered_map<std::string, std::string> result;

  try {
    std::sregex_iterator it(xml.begin(), xml.end(), DATA_REGEX);
    std::sregex_iterator end;

    for (; it != end; ++it) {
      std::string name = (*it)[1].str();
      std::string value = decodeXmlEntities((*it)[2].str());
      result.emplace(std::move(name), std::move(value));
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
  for (const auto& entity : XML_ENTITIES) {
    size_t pos = 0;
    while ((pos = text.find(entity.first, pos)) != std::string::npos) {
      text.replace(pos, entity.first.length(), entity.second);
    }
  }
  return text;
}

}
