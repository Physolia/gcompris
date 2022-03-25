#=============================================================================
# SPDX-FileCopyrightText: 2021 Johnny Jazeix <jazeix@gmail.com>
#
# SPDX-License-Identifier: BSD-3-Clause
#=============================================================================

include(qt_helper)

getQtQmlPath(_qt_qml_system_path)
set(_calendar_system_dir "${_qt_qml_system_path}/QtQuick/Calendar")

# build qtquickcalendar ourselves from submodule
include(ExternalProject)
if(UBUNTU_TOUCH)
  unset(QT_QMAKE_EXECUTABLE CACHE)
  find_program(_qmake_program "qmake")
else()
  get_property(_qmake_program TARGET Qt5::qmake PROPERTY IMPORT_LOCATION)
endif()
set(_calendar_source_dir ${CMAKE_SOURCE_DIR}/external/qtquickcalendar)
set(_calendar_qml_files_dir ${_calendar_source_dir}/src/imports/calendar/)
if(WIN32)
  set(_calendar_library_dir "qml/QtQuick/Calendar/")
  set(_calendar_library_file "qtquickcalendarplugin.dll")
elseif(CMAKE_HOST_APPLE)
  set(_calendar_library_dir "")
  set(_calendar_library_file "libqtquickcalendarplugin.dylib")
else()
  set(_calendar_library_dir "qml/QtQuick/Calendar/")
  set(_calendar_library_file "libqtquickcalendarplugin.so")
endif()
set(_calendar_install_dir ${CMAKE_BINARY_DIR}/lib/qml/QtQuick/Calendar)

# for visual studio, we need to create a vcxproj
if(WIN32 AND NOT MINGW)
  set(_qmake_options -spec win32-msvc -tp vc -r) # Needs to be recursive, else it does not generate .vcxproj
  set(CALENDAR_BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} /p:Configuration=Release /p:Platform=x64)
else()
  set(_qmake_options "")
  set(CALENDAR_BUILD_COMMAND "${CMAKE_MAKE_PROGRAM}")
endif()
# Ninja is not supported by qmake.
# In case Ninja is set as generator, use make on Linux, nmake on Windows
if(${CMAKE_GENERATOR} MATCHES "Ninja")
  if(WIN32)
    set(CALENDAR_BUILD_COMMAND "nmake")
  else()
    set(CALENDAR_BUILD_COMMAND "make")
  endif()
endif()
ExternalProject_Add(qtquick_calendar_project
  DOWNLOAD_COMMAND ""
  SOURCE_DIR ${_calendar_source_dir}
  CONFIGURE_COMMAND ${_qmake_program} ${_qmake_options} ${_calendar_source_dir}/qtquickcalendar.pro
  BUILD_COMMAND ${CALENDAR_BUILD_COMMAND}
  INSTALL_DIR ${_calendar_install_dir}
  # TODO install the .qml instead of qmlc?
  INSTALL_COMMAND ${CMAKE_COMMAND} -E copy ${_calendar_library_dir}${_calendar_library_file} ${_calendar_library_dir}qmldir ${_calendar_qml_files_dir}DayOfWeekRow.qml ${_calendar_qml_files_dir}MonthGrid.qml ${_calendar_qml_files_dir}WeekNumberColumn.qml ${_calendar_install_dir}
  )

add_library(qtquickcalendar SHARED IMPORTED)
set_target_properties(qtquickcalendar PROPERTIES IMPORTED_LOCATION ${_calendar_install_dir}/${_calendar_library_file})

if(APPLE)
  install(DIRECTORY ${_calendar_install_dir} DESTINATION gcompris-qt.app/Contents/lib/qml/QtQuick)
else()
  install(DIRECTORY ${_calendar_install_dir} DESTINATION lib/qml/QtQuick)
endif()
