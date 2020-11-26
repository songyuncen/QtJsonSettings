#include <QtTest>
#include <QFileInfo>
#include <QFile>
#include <QVariant>
#include <QColor>

#include "qt_json_settings.h"

class TestJsonSettings : public QObject {
Q_OBJECT
 private slots:
  void initTestCase() {
    RemoveSettingsFiles();
    QtJsonSettings::Initialize();
    CreateSettingsFile("TestRead.json");
    CreateSettingsFile("TestModify.json");
  }

  void TestRead() {
    QDir dir = QCoreApplication::applicationDirPath();
    QSettings settings(dir.absoluteFilePath("TestRead.json"), QtJsonSettings::GetFormat());
    QCOMPARE(settings.value("a"), 12);
    QCOMPARE(settings.value("b"), 1.23);
    QCOMPARE(settings.value("c"), "d:/projects");
    QList<QVariant> l = settings.value("d").toList();
    QList<QVariant> r{ 1, 2, 3 };
    QCOMPARE(l, r);

    QCOMPARE(settings.value("e/f"), 23);
    QCOMPARE(settings.value("e/g"), 23.4);
    QCOMPARE(settings.value("e/h"), "f:/k.bmp");
    l = settings.value("e/i").toList();
    r = QList<QVariant>{ 1.1, 3.3, 7.7 };
    QCOMPARE(l, r);

    QCOMPARE(settings.value("e/j/k"), 734);
    l = settings.value("e/j/l").toList();
    r = QList<QVariant>{ 123, 3.3, "abc" };
    QCOMPARE(l, r);
    QCOMPARE(settings.value("e/j/m"), "ccc");
  }

  void TestModify() {
    QList<QVariant> colors;
    colors << QVariant::fromValue<QColor>(Qt::red);
    colors << QVariant::fromValue<QColor>(Qt::green);
    colors << QVariant::fromValue<QColor>(Qt::blue);

    QDir dir = QCoreApplication::applicationDirPath();
    {
      QSettings settings(dir.absoluteFilePath("TestModify.json"), QtJsonSettings::GetFormat());
      settings.setValue("a", 3.3);
      settings.setValue("k", "abc");
      settings.setValue("d", QList<QVariant>{1, 1.2, "abc"});
      settings.setValue("e/f", "def");
      settings.setValue("e/j", 3.3);
      settings.setValue("k/l", 3.3);
      settings.setValue("color", QVariant::fromValue<QColor>(Qt::red));
      settings.setValue("colors", colors);
      settings.sync();
    }
    {
      QSettings settings(dir.absoluteFilePath("TestModify.json"), QtJsonSettings::GetFormat());
      QCOMPARE(settings.value("a"), 3.3);
      QCOMPARE(settings.value("k"), "abc");
      QCOMPARE(settings.value("e/f"), "def");
      QCOMPARE(settings.value("e/j"), 3.3);
      QCOMPARE(settings.value("k/l"), 3.3);
      QList<QVariant> l = settings.value("d").toList();
      QList<QVariant> r{ 1, 1.2, "abc" };
      QCOMPARE(l, r);

      QCOMPARE(settings.value("color").value<QColor>(), Qt::red);
      l = settings.value("colors").toList();
      QCOMPARE(l, colors);
    }
  }

 private:
  /*
    {
      "a" : 12,
      "b" : 1.23,
      "c" : "d:/projects",
      "d" : [1, 2, 3],
      "e" :
      {
        "f" : 23,
        "g" : 23.4,
        "h" : "f:/k.bmp"
        "i" : [1.1, 3.3, 7.7]
        "j" : {
          "k" : 734,
          "l" : [
            123,
            3.3,
            "abc"
          ],
          "m" : "ccc"
        }
      }
    }
  */

  void RemoveSettingsFiles() {
    QDir dir = QCoreApplication::applicationDirPath();
    dir.remove("TestRead.json");
    dir.remove("TestModify.json");
  }

  void CreateSettingsFile(const QString &fname) {
    QDir dir = QCoreApplication::applicationDirPath();
    QSettings settings(dir.absoluteFilePath(fname), QtJsonSettings::GetFormat());
    settings.setValue("a", 12);
    settings.setValue("b", 1.23);
    settings.setValue("c", "d:/projects");
    QList<QVariant> arr{ 1, 2, 3 };
    settings.setValue("d", arr);
    settings.setValue("e/f", 23);
    settings.setValue("e/g", 23.4);
    settings.setValue("e/h", "f:/k.bmp");
    arr = { 1.1, 3.3, 7.7 };
    settings.setValue("e/i", arr);
    settings.setValue("e/j/k", 734);
    arr = { 123, 3.3, "abc" };
    settings.setValue("e/j/l", arr);
    settings.setValue("e/j/m", "ccc");
    settings.sync();
  }
 };

QTEST_MAIN(TestJsonSettings)
#include "test_settings.moc"

