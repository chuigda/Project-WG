#ifndef PROJECT_WG_INI_LOADER_H
#define PROJECT_WG_INI_LOADER_H

#include <QString>
#include <QMap>

#include "util/Sinkrate.h"
#include "util/Derive.h"

namespace cw {

class IniSection {
public:
  IniSection(QString sectionName, Sinkrate);

  CW_DERIVE_DEFAULT_COPY(IniSection)
  CW_DERIVE_DEFAULT_MOVE(IniSection)

  void AddData(QString const& key, QString const& value, Sinkrate);

  [[nodiscard]] QString const& GetSectionName() const;

  [[nodiscard]] QString GetData(QString const& key) const;

  [[nodiscard]] int GetIntValue(QString const& key, int defaultValue = 0) const;

  [[nodiscard]] float GetFloatValue(QString const& key, float defaultValue = 0.0) const;

  [[nodiscard]] double GetDoubleValue(QString const& key, double defaultValue = 0.0) const;

  [[nodiscard]] bool GetBoolValue(QString const& key, bool defaultValue = false) const;

private:
  QString m_SectionName;
  std::unordered_map<QString, QString> m_SectionData;
};

class IniFileData {
public:
  explicit IniFileData(Sinkrate);

  CW_DERIVE_DEFAULT_COPY(IniFileData)
  CW_DERIVE_DEFAULT_MOVE(IniFileData)

  void AddSection(IniSection&& section, Sinkrate);

  [[nodiscard]] IniSection const* GetSection(QString const& sectionName) const;

  [[nodiscard]] QString GetData(QString const& composedKey) const;

  [[nodiscard]] int GetIntValue(QString const& composedKey, int defaultValue = 0) const;

  [[nodiscard]] float GetFloatValue(QString const& composedKey, float defaultValue = 0.0) const;

  [[nodiscard]] double GetDoubleValue(QString const& composedKey, double defaultValue = 0.0) const;

  [[nodiscard]] bool GetBoolValue(QString const& composedKey, bool defaultValue = false) const;

  [[nodiscard]] QString GetData(QString const& sectionName, QString const& key) const;

  [[nodiscard]] int GetIntValue(QString const& sectionName, QString const& key, int defaultValue = 0) const;

  [[nodiscard]] float GetFloatValue(QString const& sectionName, QString const& key, float defaultValue = 0.0) const;

  [[nodiscard]] double GetDoubleValue(QString const& sectionName, QString const& key, double defaultValue = 0.0) const;

  [[nodiscard]] bool GetBoolValue(QString const& sectionName, QString const& key, bool defaultValue = false) const;

private:
  std::unordered_map<QString, IniSection> m_Sections;
};

IniFileData ParseIniData(QString const& data);

} // namespace cw

#endif // PROJECT_WG_INI_LOADER_H
