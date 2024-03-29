#include "util/IniLoader.h"

#include <utility>
#include <QDebug>

namespace cw {

IniSection::IniSection(QString sectionName, Sinkrate)
  : m_SectionName(std::move(sectionName))
{}

QString const &IniSection::GetSectionName() const {
  return m_SectionName;
}

std::unordered_map<QString, QString> const &IniSection::GetData() const {
  return m_SectionData;
}

QString IniSection::GetData(const QString &key, QString const& defaultValue) const {
  auto it = m_SectionData.find(key);
  if (it == m_SectionData.end()) {
    return defaultValue;
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

void IniSection::AddData(const QString &key, const QString &value) {
  m_SectionData.insert({ key.toLower(), value });
}

std::unordered_map<QString, IniSection> const& IniFileData::GetData() const {
  return m_Sections;
}

IniSection const *IniFileData::GetSection(const QString &sectionName) const {
  auto it = m_Sections.find(sectionName);
  if (it == m_Sections.end()) {
    return nullptr;
  } else {
    return &it->second;
  }
}

QString IniFileData::GetData(const QString& composedKey,
                             const QString& defaultValue) const
{
  QStringList keyParts = composedKey.split('.');
  if (keyParts.size() != 2) {
    return defaultValue;
  } else {
    return GetDataEx(keyParts[0].trimmed(), keyParts[1].trimmed(), defaultValue);
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

QString IniFileData::GetDataEx(const QString &sectionName,
                               const QString &key,
                               QString const& defaultValue) const {
  auto it = m_Sections.find(sectionName);
  if (it == m_Sections.end()) {
    return {};
  } else {
    return it->second.GetData(key, defaultValue);
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

IniFileData IniFileData::Parse(QString const& data) {
  IniFileData result;
  std::unique_ptr<IniSection> currentSection = nullptr;

  QStringList lines = data.split('\n');
  std::size_t lineNo = 0;

  for (QString const& line : lines) {
    lineNo += 1;
    QString trimmed = line.trimmed();
    if (trimmed.isEmpty() || trimmed.startsWith(';') || trimmed.startsWith('#')) {
      continue;
    }

    if (trimmed.startsWith('[') && trimmed.endsWith(']')) {
      QString sectionName = trimmed.mid(1, trimmed.size() - 2);
      if (currentSection) {
        result.AddSection(std::move(*currentSection));
      }
      currentSection = std::make_unique<IniSection>(
        sectionName,
        SecretInternalsDoNotUseOrYouWillBeFired
      );
    } else {
      QStringList parts = trimmed.split('=');
      if (parts.size() != 2) {
        qWarning() << "IniFileData::Parse(QString const&): Invalid line in ini file at line"
                   << lineNo
                   << "(not a key-value pair)";
        continue;
      }

      if (!currentSection) {
        qWarning() << "IniFileData::Parse(QString const&): Invalid line in ini file at line"
                   << lineNo
                   << "(cannot add value without a valid section)";
        continue;
      }

      currentSection->AddData(parts[0].trimmed(), parts[1].trimmed());
    }
  }

  if (currentSection) {
    result.AddSection(std::move(*currentSection));
  }

  return result;
}

void IniFileData::AddSection(IniSection &&section) {
  QString sectionName = section.GetSectionName().toLower();
  m_Sections.insert({ std::move(sectionName), std::move(section) });
}

} // namespace cw
