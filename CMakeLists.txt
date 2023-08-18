cmake_minimum_required(VERSION 3.20)
project(Lith)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

set(VERSION "0.0.0" CACHE STRING "This is where we start working with the version string as passed from the build script")

if (VERSION)
    set(PROJECT_VERSION ${VERSION})
else()
    message(WARNING "Version passed to CMake was empty, this may have been done on accident")
    set(PROJECT_VERSION "0.0.0")
endif()

string(REPLACE "." ";" VERSION_LIST ${PROJECT_VERSION})
list(GET VERSION_LIST 0 PROJECT_VERSION_MAJOR)
list(GET VERSION_LIST 1 PROJECT_VERSION_MINOR)
list(GET VERSION_LIST 2 PROJECT_VERSION_PATCH)

find_package(Git REQUIRED)
# rerun git metadata retrieval on each commit message change
set_property(
    DIRECTORY
    APPEND
    PROPERTY CMAKE_CONFIGURE_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/.git/COMMIT_EDITMSG
)
execute_process(
    COMMAND "${GIT_EXECUTABLE}" rev-parse HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND "${GIT_EXECUTABLE}" describe --tags --dirty
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE GIT_STATE
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

find_package(Qt6 COMPONENTS Widgets Gui Quick Multimedia Qml QuickControls2 QuickLayouts QuickDialogs2 QuickTemplates2 WebSockets Xml REQUIRED)
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(Qt6 COMPONENTS DBus REQUIRED)
endif()

qt_standard_project_setup(REQUIRES 6.5)

set(QML_IMPORT_PATH ${QML_IMPORT_PATH} ${CMAKE_BINARY_DIR}/imports CACHE STRING "" FORCE)

add_subdirectory(assets)
add_subdirectory(style)
add_subdirectory(ui)
add_subdirectory(src)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dist/linux/app.lith.Lith.appdata.xml DESTINATION share/metainfo)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dist/linux/app.lith.Lith.desktop DESTINATION share/applications)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets/icons/linux/hicolor DESTINATION share/icons)
endif()
