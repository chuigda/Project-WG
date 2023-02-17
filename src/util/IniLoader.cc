#include "util/IniLoader.h"

#include <utility>

namespace cw {

IniSection::IniSection(QString sectionName, Sinkrate)
  : m_SectionName(std::move(sectionName))
{}

void IniSection::AddData(const QString &key, const QString &value, Sinkrate) {
  m_SectionData[key] = value;
}

QString const &IniSection::GetSectionName() const {
  return m_SectionName;
}

QString IniSection::GetData(const QString &key) const {
  auto it = m_SectionData.find(key);
  if (it == m_SectionData.end()) {
    return {};
  } else {
    return it->second;
  }
}

int IniSection::GetIntValue(const QString &key, int defaultValue) const {
  auto it = m_SectionData.find(key);
  if (it == m_SectionData.end()) {
    return defaultValue;
  } else {
    return it->second.toInt();
  }
}

float IniSection::GetFloatValue(const QString &key, float defaultValue) const {
  auto it = m_SectionData.find(key);
  if (it == m_SectionData.end()) {
    return defaultValue;
  } else {
    return it->second.toFloat();
  }
}

double IniSection::GetDoubleValue(const QString &key, double defaultValue) const {
  auto it = m_SectionData.find(key);
  if (it == m_SectionData.end()) {
    return defaultValue;
  } else {
    return it->second.toDouble();
  }
}

bool IniSection::GetBoolValue(const QString &key, bool defaultValue) const {
  auto it = m_SectionData.find(key);
  if (it == m_SectionData.end()) {
    return defaultValue;
  } else {
    return it->second == "1"
           || it->second.compare("true", Qt::CaseInsensitive) == 0;
  }
}

IniFileData::IniFileData(Sinkrate) {}

void IniFileData::AddSection(IniSection &&section, Sinkrate) {
  QString sectionName = section.GetSectionName();
  m_Sections.insert({ std::move(sectionName), std::move(section) });
}

IniSection const *IniFileData::GetSection(const QString &sectionName) const {
  auto it = m_Sections.find(sectionName);
  if (it == m_Sections.end()) {
    return nullptr;
  } else {
    return &it->second;
  }
}

QString IniFileData::GetData(const QString &composedKey) const {
  QStringList keyParts = composedKey.split('.');
  if (keyParts.size() != 2) {
    return {};
  } else {
    return GetData(keyParts[0].trimmed(), keyParts[1].trimmed());
  }
}

#define GENERATE_GET_VALUE(TYPE, FN_NAME) \
  TYPE IniFileData::FN_NAME(const QString& composedKey, TYPE defaultValue) const { \
    QStringList keyParts = composedKey.split('.'); \
    if (keyParts.size() != 2) { \
      return defaultValue; \
    } else { \
      return FN_NAME(keyParts[0].trimmed(), keyParts[1].trimmed(), defaultValue); \
    } \
  }

GENERATE_GET_VALUE(int, GetIntValue)
GENERATE_GET_VALUE(float, GetFloatValue)
GENERATE_GET_VALUE(double, GetDoubleValue)
GENERATE_GET_VALUE(bool, GetBoolValue)

QString IniFileData::GetData(const QString &sectionName, const QString &key) const {
  auto it = m_Sections.find(sectionName);
  if (it == m_Sections.end()) {
    return {};
  } else {
    return it->second.GetData(key);
  }
}

#define GENERATE_GET_VALUE2(TYPE, FN_NAME) \
  TYPE IniFileData::FN_NAME(const QString& sectionName, const QString& key, TYPE defaultValue) const { \
    auto it = m_Sections.find(sectionName); \
    if (it == m_Sections.end()) { \
      return defaultValue; \
    } else { \
      return it->second.FN_NAME(key, defaultValue); \
    } \
  }

GENERATE_GET_VALUE2(int, GetIntValue)
GENERATE_GET_VALUE2(float, GetFloatValue)
GENERATE_GET_VALUE2(double, GetDoubleValue)
GENERATE_GET_VALUE2(bool, GetBoolValue)

} // namespace cw