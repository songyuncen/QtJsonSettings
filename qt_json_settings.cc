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

int GetVariantMetaType(const QVariant& var)
{
#if QT_VERSION_MAJOR == 6
    return var.typeId();
#else
    return static_cast<int>(var.type());
#endif
}

void FlattenVariantMap(const QVariantMap &input, const QString &prefix, QSettings::SettingsMap *settings) {
  for (auto iter = input.cbegin(); iter != input.cend(); ++iter) {
        if (GetVariantMetaType(*iter) == QMetaType::QVariantMap) {
      FlattenVariantMap(iter->toMap(), prefix + iter.key() + "/", settings);
    } else {
      (*settings)[prefix + iter.key()] = (*iter);
    }
  }
}

bool ReadJsonSettingsFile(QIODevice &device, QSettings::SettingsMap &settings) {
  QByteArray data = device.readAll();
  QJsonDocument doc = QJsonDocument::fromJson(data);
  if (doc.object().isEmpty()) {   // is array
    return false;
  }

  QVariantMap values = doc.object().toVariantMap();
  FlattenVariantMap(values, "", &settings);
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
  QJsonDocument doc;
  doc.setObject(QJsonObject());

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


