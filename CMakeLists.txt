cmake_minimum_required(VERSION 3.15)
project(QtJsonSettings)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)

find_package(Qt${QT_VERSION_MAJOR}
  COMPONENTS
    Core
    Gui
    Test
  REQUIRED
  )

add_library(QtJsonSettings SHARED)
target_sources(QtJsonSettings
  PUBLIC
    qt_json_settings.h
  PRIVATE
    qt_json_settings.cc
  )

target_link_libraries(QtJsonSettings
  PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
  )


add_executable(test_settings test_settings)
target_link_libraries(test_settings
  PRIVATE
    Qt${QT_VERSION_MAJOR}::Test
    Qt${QT_VERSION_MAJOR}::Gui QtJsonSettings
  )
add_test(NAME test_settings COMMAND $<TARGET_FILE:test_settings>)


