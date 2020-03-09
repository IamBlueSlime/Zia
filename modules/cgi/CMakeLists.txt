project(zia_module_CGI VERSION 1.0.0 LANGUAGES CXX)

set(ZIA_MODULE_CGI_REQUIRED_LIBRARIES ${openZiaLibs} zia_common)
set(ZIA_MODULE_CGI_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
set(ZIA_MODULE_CGI_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(ZIA_MODULE_CGI_SRC
    ${ZIA_MODULE_CGI_SRC_DIR}/CGIModule.cpp
    ${ZIA_MODULE_CGI_SRC_DIR}/Fork.cpp
)

add_library(zia_cgi SHARED ${ZIA_MODULE_CGI_SRC})
target_include_directories(zia_cgi PUBLIC ${ZIA_MODULE_CGI_INCLUDE_DIRS})
target_link_libraries(zia_cgi ${ZIA_MODULE_CGI_REQUIRED_LIBRARIES})

if (BUILD_TESTS)
    set_source_files_properties(${ZIA_MODULE_CGI_SRC} PROPERTIES
        COMPILE_FLAGS "-fprofile-arcs -ftest-coverage"
    )

    add_executable(zia_module_cgi_units
        ${ZIA_MODULE_CGI_SRC}
    )

    target_include_directories(zia_module_http_units PRIVATE ${ZIA_MODULE_CGI_INCLUDE_DIRS})
    target_link_libraries(zia_module_http_units ${ZIA_MODULE_CGI_REQUIRED_LIBRARIES} criterion gcov)
endif ()