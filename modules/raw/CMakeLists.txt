project(zia_module_raw VERSION 1.0.0 LANGUAGES CXX)

set(ZIA_MODULE_RAW_REQUIRED_LIBRARIES ${openZiaLibs} zia_common)
set(ZIA_MODULE_RAW_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
set(ZIA_MODULE_RAW_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(ZIA_MODULE_RAW_SRC
    ${ZIA_MODULE_RAW_SRC_DIR}/RawModule.cpp
    ${ZIA_MODULE_RAW_SRC_DIR}/MIMEType.cpp
)

add_library(zia_raw SHARED ${ZIA_MODULE_RAW_SRC})
target_include_directories(zia_raw PUBLIC ${ZIA_MODULE_RAW_INCLUDE_DIRS})
target_link_libraries(zia_raw ${ZIA_MODULE_RAW_REQUIRED_LIBRARIES})

if (BUILD_TESTS)
    set_source_files_properties(${ZIA_MODULE_RAW_SRC} PROPERTIES
        COMPILE_FLAGS "-fprofile-arcs -ftest-coverage"
    )

    add_executable(zia_module_raw_units
        ${ZIA_MODULE_RAW_SRC}
    )

    target_include_directories(zia_module_raw_units PRIVATE ${ZIA_MODULE_RAW_INCLUDE_DIRS})
    target_link_libraries(zia_module_raw_units ${ZIA_MODULE_RAW_REQUIRED_LIBRARIES} criterion gcov)
endif ()