#include "qt_json_settings.h"

#include <QIODevice>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QVariant>

static bool ReadJsonSettingsFile(QIODevice &device, QSettings::SettingsMap &map);
static bool WriteJsonSettingsFile(QIODevice &device, const QSettings::SettingsMap &map);

QSettings::Format kSettingFormat = QSettings::InvalidFormat;

namespace QtJsonSettings {

bool Initialize() {
  kSettingFormat = QSettings::registerFormat("json", ReadJsonSettingsFile, WriteJsonSettingsFile, Qt::CaseSensitive);
  if (kSettingFormat != QSettings::InvalidFormat) {
    QSettings::setDefaultFormat(kSettingFormat);
  }
  return (kSettingFormat != QSettings::InvalidFormat);
}

QSettings::Format GetFormat() {
  return kSettingFormat;
}

}  // namespace QtJsonSettings

bool ReadValueFromJson(const QJsonDocument &doc, const QString &key, QVariant *value) {
  QStringList names = key.split("/", Qt::SkipEmptyParts);
  if (names.size() == 0) {
    return false;
  }

  int idx = 0;
  if (!doc.object().contains(names[idx])) {
    return false;
  }

  QJsonValue val = doc[names[idx++]];
  for (; idx < names.size(); ++idx) {
    if (!val.isObject()) {
      return false;
    }

    QJsonObject obj = val.toObject();
    if (!obj.contains(names[idx])) {
      return false;
    }
    val = obj[names[idx]];
  }

  if (val.isNull() || val.isObject()) {
    return false;
  } else if (val.isArray()) {
    *value = val.toArray().toVariantList();
  } else {
    *value = val.toVariant();
  }

  return true;
}

bool ReadJsonSettingsFile(QIODevice &device, QSettings::SettingsMap &settings) {
  QByteArray data = device.readAll();
  QJsonDocument doc = QJsonDocument::fromJson(data);

  for (QString &key : settings.keys()) {
    QVariant value;
    if (ReadValueFromJson(doc, key, &value)) {
      settings[key] = value;
    } else {
      return false;
    }
  }

  return true;
}

void SetValueToJson(QJsonObject &obj, const QStringList &names, int index, const QVariant &value) {
  if (index == names.size() - 1) {
    obj[names.last()] = QJsonValue::fromVariant(value);
    return;
  }

  if (!obj.contains(names[index])) {
    obj.insert(names[index], QJsonObject());
  }

  QJsonObject new_obj = obj[names[index]].toObject();
  SetValueToJson(new_obj, names, index + 1, value);
  obj[names[index]] = new_obj;
}

bool WriteValueToJson(QJsonObject &doc, const QString &key, const QVariant &value) {
  QStringList names = key.split("/", Qt::SkipEmptyParts);

  if (names.size() == 0) {
    return true;
  }

  SetValueToJson(doc, names, 0, value);

  return true;
}

bool WriteJsonSettingsFile(QIODevice &device, const QSettings::SettingsMap &settings) {
  QByteArray data = device.readAll();
  QJsonDocument doc;
  if (data.size() == 0) {
    doc.setObject(QJsonObject());
  } else {
    doc = QJsonDocument::fromJson(data);
  }

  QJsonObject root = doc.object();

  for (QString &key : settings.keys()) {
    if (!WriteValueToJson(root, key, settings[key])) {
      return false;
    }
  }

  doc.setObject(root);
  device.write(doc.toJson());

  return true;
}


