cmake_minimum_required(VERSION 3.20)
project(Lith)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(CMakeDependentOption)
include(FeatureSummary)
include(GenerateExportHeader)
include(cmake/CPM.cmake)

if(NOT CMAKE_CROSSCOMPILING)
    # Doesn't make sense when running on another device
    option(LITH_FEATURE_QHOT "Download and build https://github.com/patrickelectric/qhot - a live-reloading tool for UI development." OFF)
endif()
option(LITH_FORCE_DOWNLOAD_DEPENDENCIES "Force downloading 3rd party dependencies even if they are installed." OFF)
option(LITH_FORCE_LOCAL_PACKAGES_ONLY "Completely disable downloading 3rd party dependencies, only the ones from find_package will be used." OFF)
option(LITH_BUILD_TESTS "While building the application, build tests, too" OFF)

if(NOT CMAKE_SYSTEM_NAME STREQUAL Emscripten)
    # There's no WebAssembly backend
    option(LITH_FEATURE_KEYCHAIN "Use QtKeychain to store credentials securely" ON)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(LITH_DEPENDENCY_ENABLE_DBUS ON)
else()
    set(LITH_DEPENDENCY_ENABLE_DBUS OFF)
endif()

if(LITH_FEATURE_KEYCHAIN)
    set(LITH_DEPENDENCY_ENABLE_KEYCHAIN ON)
else()
    set(LITH_DEPENDENCY_ENABLE_KEYCHAIN OFF)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    set(ANDROID_OPENSSL_PATH "${ANDROID_SDK_ROOT}/android_openssl/ssl_3" CACHE STRING "Path to the folder with https://github.com/KDAB/android_openssl.git")
endif()

set(LITH_VERSION "0.0.0" CACHE STRING "This is where we start working with the version string as passed from the build script")
set(LITH_VERSION_RELEASEDATE "1970-01-01" CACHE STRING "Release date to be used in metadata, used in Linux appdata only for now.")

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
    OUTPUT_VARIABLE LITH_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND "${GIT_EXECUTABLE}" describe --tags --dirty
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE LITH_GIT_STATE
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND "${GIT_EXECUTABLE}" describe --tags --abbrev=0
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE GIT_LAST_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND "${GIT_EXECUTABLE}" tag -l "${GIT_LAST_TAG}" "--format=%(creatordate:short)"
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE GIT_LAST_TAG_DATE
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if (LITH_VERSION AND NOT "${LITH_VERSION}" STREQUAL "0.0.0")
    set(LITH_PROJECT_VERSION ${LITH_VERSION} CACHE STRING "" FORCE)
    if (NOT "${LITH_VERSION_RELEASEDATE}" STREQUAL "1970-01-01")
        set(LITH_PROJECT_VERSION_RELEASEDATE "${LITH_VERSION_RELEASEDATE}" CACHE STRING "" FORCE)
    else()
        message("Release date was not supplied but version was, metadata will show today")
        string(TIMESTAMP LITH_PROJECT_VERSION_RELEASEDATE "%Y-%m-%d")
    endif()
    message("Building Lith version ${LITH_PROJECT_VERSION}, release date: ${LITH_PROJECT_VERSION_RELEASEDATE}")
elseif(GIT_LAST_TAG)
    set(LITH_PROJECT_VERSION "${GIT_LAST_TAG}" CACHE STRING "" FORCE)
    set(LITH_PROJECT_VERSION_RELEASEDATE "${GIT_LAST_TAG_DATE}" CACHE STRING "" FORCE)
    message("Version passed to CMake was empty, using last one retrieved from git: ${LITH_PROJECT_VERSION}, tagged on ${LITH_PROJECT_VERSION_RELEASEDATE}")
else()
    set(LITH_PROJECT_VERSION "0.0.0" CACHE STRING "" FORCE)
    set(LITH_PROJECT_VERSION_RELEASEDATE "1970-01-01" CACHE STRING "" FORCE)
    message(WARNING "Version passed to CMake was empty, this may have been done on accident, using ${LITH_PROJECT_VERSION}")
endif()

string(REPLACE "." ";" VERSION_LIST ${LITH_PROJECT_VERSION})
list(GET VERSION_LIST 0 LITH_PROJECT_VERSION_MAJOR)
list(GET VERSION_LIST 1 LITH_PROJECT_VERSION_MINOR)
list(GET VERSION_LIST 2 LITH_PROJECT_VERSION_PATCH)

if(CMAKE_BUILD_TYPE STREQUAL Debug OR CMAKE_BUILD_TYPE STREQUAL Profile)
    set(LITH_IS_DEBUG_BUILD ON CACHE STRING "")
endif()

find_package(Qt6 COMPONENTS Widgets Gui Quick Multimedia Qml QuickControls2 QuickLayouts QuickDialogs2 QuickTemplates2 WebSockets Xml REQUIRED)
if (LITH_DEPENDENCY_ENABLE_DBUS)
    find_package(Qt6 COMPONENTS DBus REQUIRED)
endif()

if(LITH_BUILD_TESTS)
    find_package(Qt6 COMPONENTS Test REQUIRED)
    enable_testing()
endif()

if (LITH_FORCE_DOWNLOAD_DEPENDENCIES)
    message(STATUS "All dependencies will be downloaded and built")
    set(CPM_USE_LOCAL_PACKAGES OFF CACHE BOOL "" FORCE)
    set(CPM_LOCAL_PACKAGES_ONLY OFF CACHE BOOL "" FORCE)
elseif(LITH_FORCE_LOCAL_PACKAGES_ONLY)
    message(STATUS "CMake will enforce use your systemwide dependencies")
    set(CPM_USE_LOCAL_PACKAGES ON CACHE BOOL "" FORCE)
    set(CPM_LOCAL_PACKAGES_ONLY ON CACHE BOOL "" FORCE)
else()
    message(STATUS "CMake will try to use your systemwide dependencies, if installed")
    set(CPM_USE_LOCAL_PACKAGES ON CACHE BOOL "" FORCE)
    set(CPM_LOCAL_PACKAGES_ONLY OFF CACHE BOOL "" FORCE)
endif()

if(LITH_DEPENDENCY_ENABLE_KEYCHAIN)
    CPMAddPackage(
        NAME Qt6Keychain
        GIT_REPOSITORY https://github.com/frankosterfeld/qtkeychain.git
        GIT_TAG 38ff50766c7e4ec490fecb2d9939b7cdc4b74b8e
        VERSION 0.14.3
        EXCLUDE_FROM_ALL ON
        OPTIONS "BUILD_WITH_QT6 ON" "USE_CREDENTIAL_STORE ON" "BUILD_SHARED_LIBS OFF" "BUILD_TRANSLATIONS OFF"
    )
endif()

set(LITH_QCORO_DEPENDENCY_MODULES
    Core
    Network
    WebSockets
)
if (LITH_DEPENDENCY_ENABLE_DBUS)
    list(APPEND LITH_QCORO_DEPENDENCY_MODULES
        "DBus"
    )
endif()
CPMAddPackage(
    NAME QCoro6
    GIT_REPOSITORY https://github.com/danvratil/qcoro
    GIT_TAG fa5bc4296a252ad71c85209b60675bb025024a1e
    VERSION 0.10.0
    EXCLUDE_FROM_ALL ON
    OPTIONS "QCORO_BUILD_EXAMPLES OFF" "BUILD_TESTING OFF" "QCORO_WITH_QTDBUS ${LITH_DEPENDENCY_ENABLE_DBUS}"
    FIND_PACKAGE_ARGUMENTS "COMPONENTS ${LITH_QCORO_DEPENDENCY_MODULES} REQUIRED"
)

qt_standard_project_setup(REQUIRES 6.5)
get_target_property(QT_MOC_EXECUTABLE Qt::moc IMPORTED_LOCATION)
get_filename_component(QT_BINARY_PREFIX ${QT_MOC_EXECUTABLE} DIRECTORY)
get_filename_component(QT_INSTALL_PREFIX ${QT_BINARY_PREFIX} DIRECTORY)
qcoro_enable_coroutines()

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

set(QML_IMPORT_PATH "${CMAKE_BINARY_DIR}/modules" CACHE STRING "")
#set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${LITH_INSTALL_LIBDIR}")

add_subdirectory(assets)
add_subdirectory(modules/Lith/Style)
add_subdirectory(modules/Lith/UI)
add_subdirectory(modules/Lith/Core)
add_subdirectory(app)

add_dependencies(Lith LithStyleplugin LithUIplugin LithCoreplugin)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    configure_file(${CMAKE_SOURCE_DIR}/dist/linux/app.lith.Lith.appdata.xml.in ${CMAKE_BINARY_DIR}/dist/linux/app.lith.Lith.appdata.xml)
    install(FILES ${CMAKE_BINARY_DIR}/dist/linux/app.lith.Lith.appdata.xml DESTINATION share/metainfo)
    install(FILES ${CMAKE_SOURCE_DIR}/dist/linux/app.lith.Lith.desktop DESTINATION share/applications)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets/icons/linux/hicolor DESTINATION share/icons)
endif()

if (LITH_FEATURE_QHOT)
    CPMAddPackage(
        NAME qhot
        GIT_REPOSITORY https://github.com/MartinBriza/qhot.git
        GIT_TAG 942aa3d6fb1badf8952b4fb127d6881861611771
        EXCLUDE_FROM_ALL ON
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
        if (APPLE)
            set(qhot_FULL_BINARY_PATH "${qhot_BINARY_DIR}/${qhot_NAME}.app/Contents/MacOS/${qhot_NAME}")
        else()
            set(qhot_FULL_BINARY_PATH "${qhot_BINARY_DIR}/${qhot_NAME}")
        endif()
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
