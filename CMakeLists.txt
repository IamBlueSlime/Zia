cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

project(zia VERSION 1.0.0 LANGUAGES CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
include_directories(${CMAKE_INCLUDE_PATH})
link_libraries(${CONAN_LIBS})

include(vendor/openZia/openZia/openZia.cmake)
include_directories(${openZiaIncludes})

option(BUILD_SERVER "Enable build for the server" ON)
option(BUILD_MODULES "Enable build for the modules" ON)
option(BUILD_TESTS "Enable tests" OFF)

add_compile_definitions(OPENZIA_PRE_WINDOWS_INCLUDE="winsock2.h")

if (CMAKE_BUILD_TYPE EQUAL "Debug")
    add_compile_options(-g3)
endif ()

if (UNIX)
    add_compile_options(-Wall -Wextra)
endif ()

if (UNIX AND NOT APPLE)
    link_libraries(stdc++fs)
endif ()

if (WIN32)
    add_compile_options(-DNOMINMAX -D_WINSOCKAPI_)
    add_link_options(/ignore:4099)
endif ()

set(toml11_BUILD_TEST OFF CACHE BOOL "")
add_subdirectory(vendor/toml11)
link_libraries(toml11)

add_subdirectory(apps)
add_subdirectory(libs)

if (BUILD_MODULES)
    add_subdirectory(modules)
endif ()

if (BUILD_TESTS)
    enable_testing()
    add_test(NAME zia_server_tests COMMAND zia_server_units --xml=report.xml)
endif ()