project(zia_module_ssl VERSION 1.0.0 LANGUAGES CXX)

set(ZIA_MODULE_SSL_REQUIRED_LIBRARIES ${openZiaLibs} zia_common)
set(ZIA_MODULE_SSL_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
set(ZIA_MODULE_SSL_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(ZIA_MODULE_SSL_SRC
    ${ZIA_MODULE_SSL_SRC_DIR}/SSLModule.cpp
)

add_library(zia_ssl SHARED ${ZIA_MODULE_SSL_SRC})
target_include_directories(zia_ssl PUBLIC ${ZIA_MODULE_SSL_INCLUDE_DIRS})
target_link_libraries(zia_ssl ${ZIA_MODULE_SSL_REQUIRED_LIBRARIES})

if (WIN32)
    target_link_libraries(zia_ssl ws2_32)
endif ()

if (BUILD_TESTS)
    set_source_files_properties(${ZIA_MODULE_SSL_SRC} PROPERTIES
        COMPILE_FLAGS "-fprofile-arcs -ftest-coverage"
    )

    add_executable(zia_module_ssl_units
        ${ZIA_MODULE_SSL_SRC}
    )

    target_include_directories(zia_module_ssl_units PRIVATE ${ZIA_MODULE_SSL_INCLUDE_DIRS})
    target_link_libraries(zia_module_ssl_units ${ZIA_MODULE_SSL_REQUIRED_LIBRARIES} criterion gcov)
endif ()