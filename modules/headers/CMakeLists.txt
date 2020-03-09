project(zia_module_headers VERSION 1.0.0 LANGUAGES CXX)

set(ZIA_MODULE_HEADERS_REQUIRED_LIBRARIES ${openZiaLibs} zia_common)
set(ZIA_MODULE_HEADERS_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
set(ZIA_MODULE_HEADERS_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(ZIA_MODULE_HEADERS_SRC
    ${ZIA_MODULE_HEADERS_SRC_DIR}/HeadersModule.cpp
)

add_library(zia_headers SHARED ${ZIA_MODULE_HEADERS_SRC})
target_include_directories(zia_headers PUBLIC ${ZIA_MODULE_HEADERS_INCLUDE_DIRS})
target_link_libraries(zia_headers ${ZIA_MODULE_HEADERS_REQUIRED_LIBRARIES})

if (WIN32)
    target_link_libraries(zia_headers ws2_32)
endif ()

if (BUILD_TESTS)
    set_source_files_properties(${ZIA_MODULE_HEADERS_SRC} PROPERTIES
        COMPILE_FLAGS "-fprofile-arcs -ftest-coverage"
    )

    add_executable(zia_module_headers_units
        ${ZIA_MODULE_Headers_SRC}
    )

    target_include_directories(zia_module_headers_units PRIVATE ${ZIA_MODULE_HEADERS_INCLUDE_DIRS})
    target_link_libraries(zia_module_headers_units ${ZIA_MODULE_HEADERS_REQUIRED_LIBRARIES} criterion gcov)
endif ()