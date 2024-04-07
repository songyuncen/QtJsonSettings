#ifndef QT_JSON_SETTINGS_H_
#define QT_JSON_SETTINGS_H_

#ifdef WIN32
#ifdef QtJsonSettings_EXPORTS
# define JsonExport __declspec(dllexport)
#else
# define JsonExport __declspec(dllimport)
#endif
#else
#define JsonExport
#endif

#include <QSettings>

namespace QtJsonSettings {

JsonExport bool Initialize();
JsonExport QSettings::Format GetFormat();

}

#endif  // QT_JSON_SETTINGS_H_

