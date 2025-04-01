cmake_minimum_required(VERSION 3.23)

project(
    egt-thermostat
    VERSION 1.8.1
    DESCRIPTION "A thermostat application built upon the EGT library."
    LANGUAGES C;CXX
)
#set(PROJECT_VERSION_RC 1)
if (DEFINED PROJECT_VERSION_RC)
    string(APPEND PROJECT_VERSION "-rc${PROJECT_VERSION_RC}")
endif()

include(CheckIncludeFileCXX)
include(GNUInstallDirs)

set(CMAKE_VERBOSE_MAKEFILE True)

set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)

pkg_check_modules(LIBEGT REQUIRED libegt>=1.10)

add_executable(egt-thermostat
    src/thermostat.cpp
    src/logic.cpp
    src/pages.cpp
    src/window.cpp
    src/settings.cpp
    src/sensors.cpp
)

target_compile_definitions(egt-thermostat PRIVATE DATADIR="${CMAKE_INSTALL_FULL_DATADIR}")

target_include_directories(egt-thermostat PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}
)

target_link_libraries(egt-thermostat PRIVATE dl)

target_include_directories(egt-thermostat PRIVATE ${LIBEGT_INCLUDE_DIRS})
target_compile_options(egt-thermostat PRIVATE ${LIBEGT_CFLAGS_OTHER})
target_link_directories(egt-thermostat PRIVATE ${LIBEGT_LIBRARY_DIRS})
target_link_libraries(egt-thermostat PRIVATE ${LIBEGT_LIBRARIES})
target_link_options(egt-thermostat PRIVATE ${LIBEGT_LDFLAGS_OTHER})

option(WITH_SENSORS "Ignore the presence of sensors and disable them" ON)
if(WITH_SENSORS)
    CHECK_INCLUDE_FILE_CXX(sensors/sensors.h HAVE_SENSORS)
    if (HAVE_SENSORS)
        target_compile_definitions(egt-thermostat PRIVATE LUA_USE_READLINE)
	target_link_libraries(egt-thermostat PRIVATE sensors)
    endif()
endif()

option(ENABLE_DATABASE "enable the database to store data and settings" OFF)
if(ENABLE_DATABASE)
    add_subdirectory(external)

    add_dependencies(egt-thermostat sqlite3)

    target_include_directories(egt-thermostat PRIVATE
        ${CMAKE_SOURCE_DIR}/external/sqlite3
        ${CMAKE_SOURCE_DIR}/external/sqlite3pp/headeronly_src
    )
    target_link_directories(egt-thermostat PRIVATE ${CMAKE_BINARY_DIR}/external)
    target_link_libraries(egt-thermostat PRIVATE sqlite3)
endif()

target_compile_definitions(egt-thermostat PRIVATE HAVE_CONFIG_H)
configure_file(_config.h.in ${CMAKE_BINARY_DIR}/config.h @ONLY)

install(TARGETS egt-thermostat RUNTIME)
install(FILES egt-thermostat.xml egt-thermostat.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/egt/thermostat
)
install(DIRECTORY images/
        DESTINATION ${CMAKE_INSTALL_DATADIR}/egt/thermostat
        FILES_MATCHING
	PATTERN "*.png"
)
