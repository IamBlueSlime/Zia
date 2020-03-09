project(zia_module_authorization VERSION 1.0.0 LANGUAGES CXX)

set(ZIA_MODULE_AUTHORIZATION_REQUIRED_LIBRARIES ${openZiaLibs} zia_common)
set(ZIA_MODULE_AUTHORIZATION_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
set(ZIA_MODULE_AUTHORIZATION_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(ZIA_MODULE_AUTHORIZATION_SRC
    ${ZIA_MODULE_AUTHORIZATION_SRC_DIR}/AuthorizationModule.cpp
)

add_library(zia_authorization SHARED ${ZIA_MODULE_AUTHORIZATION_SRC})
target_include_directories(zia_authorization PUBLIC ${ZIA_MODULE_AUTHORIZATION_INCLUDE_DIRS})
target_link_libraries(zia_authorization ${ZIA_MODULE_AUTHORIZATION_REQUIRED_LIBRARIES})

if (BUILD_TESTS)
    set_source_files_properties(${ZIA_MODULE_AUTHORIZATION_SRC} PROPERTIES
        COMPILE_FLAGS "-fprofile-arcs -ftest-coverage"
    )

    add_executable(zia_module_authorization_units
        ${ZIA_MODULE_AUTHORIZATION_SRC}
    )

    target_include_directories(zia_module_authorization_units PRIVATE ${ZIA_MODULE_AUTHORIZATION_INCLUDE_DIRS})
    target_link_libraries(zia_module_authorization_units ${ZIA_MODULE_AUTHORIZATION_REQUIRED_LIBRARIES} criterion gcov)
endif ()