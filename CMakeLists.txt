cmake_minimum_required(VERSION 3.20)
project(Lith)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

include(FeatureSummary)
include(GenerateExportHeader)
include(cmake/CPM.cmake)

option(LITH_FEATURE_QHOT "Download and build https://github.com/patrickelectric/qhot - a live-reloading tool for UI development." OFF)

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
get_target_property(QT_MOC_EXECUTABLE Qt::moc IMPORTED_LOCATION)
get_filename_component(QT_BINARY_PREFIX ${QT_MOC_EXECUTABLE} DIRECTORY)
get_filename_component(QT_INSTALL_PREFIX ${QT_BINARY_PREFIX} DIRECTORY)

# I'm pretty sure I'm doing something wrong with this but I haven't found a better way to create the macOS bundle
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(LITH_INSTALL_LIBDIR "Lith.app/Contents/Frameworks/")
else()
    set(LITH_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR})
endif()
if (UNIX AND NOT APPLE)
    set(LITH_INSTALL_MODULEDIR "${CMAKE_INSTALL_PREFIX}/${LITH_INSTALL_LIBDIR}/Lith/modules")
else()
    set(LITH_INSTALL_MODULEDIR "${CMAKE_INSTALL_PREFIX}/${LITH_INSTALL_LIBDIR}/modules")
endif()

#set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${LITH_INSTALL_LIBDIR}")

add_subdirectory(assets)
add_subdirectory(modules/Lith/Style)
add_subdirectory(modules/Lith/UI)
add_subdirectory(modules/Lith/Core)
add_subdirectory(app)

add_dependencies(Lith LithStyleplugin LithUIplugin LithCoreplugin)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    install(FILES ${CMAKE_SOURCE_DIR}/dist/linux/app.lith.Lith.appdata.xml DESTINATION share/metainfo)
    install(FILES ${CMAKE_SOURCE_DIR}/dist/linux/app.lith.Lith.desktop DESTINATION share/applications)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets/icons/linux/hicolor DESTINATION share/icons)
endif()

if (LITH_FEATURE_QHOT)
    CPMAddPackage(
        NAME qhot
        GIT_REPOSITORY https://github.com/MartinBriza/qhot.git
        GIT_TAG 942aa3d6fb1badf8952b4fb127d6881861611771
    )
    get_property(qhot_BINARY_DIR TARGET qhot PROPERTY BINARY_DIR)
    get_property(qhot_NAME TARGET qhot PROPERTY NAME)

    add_executable(Lith-qhot EXCLUDE_FROM_ALL)
    add_dependencies(Lith Lith-qhot)
    add_dependencies(Lith-qhot qhot)
    if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        set(qhot_FULL_BINARY_PATH "${CMAKE_BINARY_DIR}/${qhot_NAME}.exe")
        set_target_properties(Lith-qhot PROPERTIES
            LINKER_LANGUAGE NONE
            OUTPUT_NAME Lith-qhot
            SUFFIX .bat
        )
        configure_file(dist/Lith-qhot.bat.in Lith-qhot.bat
            @ONLY
            FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        )
    else()
        set(qhot_FULL_BINARY_PATH "${qhot_BINARY_DIR}/${qhot_NAME}")
        set_target_properties(Lith-qhot PROPERTIES
            LINKER_LANGUAGE NONE
            OUTPUT_NAME Lith-qhot.sh
        )
        configure_file(dist/Lith-qhot.sh.in Lith-qhot.sh
            @ONLY
            FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        )
    endif()
endif()

feature_summary(WHAT ALL)
