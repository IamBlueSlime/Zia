project(zia_common VERSION 1.0.0 LANGUAGES CXX)

set(ZIA_COMMON_REQUIRED_LIBRARIES ${openZiaLibs})
set(ZIA_COMMON_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
set(ZIA_COMMON_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(ZIA_COMMON_SRC
    ${ZIA_COMMON_INCLUDE_DIRS}/zia/common/BufferUtils.hpp
    ${ZIA_COMMON_INCLUDE_DIRS}/zia/common/StringUtils.hpp
    ${ZIA_COMMON_SRC_DIR}/Logger.cpp
    ${ZIA_COMMON_SRC_DIR}/Base64.cpp
)

add_library(zia_common STATIC ${ZIA_COMMON_SRC})
target_include_directories(zia_common PUBLIC ${ZIA_COMMON_INCLUDE_DIRS})
target_link_libraries(zia_common ${ZIA_COMMON_REQUIRED_LIBRARIES})

if (WIN32)
    target_link_libraries(zia_common ws2_32)
endif ()

if (BUILD_TESTS)
    set_source_files_properties(${ZIA_COMMON_SRC} PROPERTIES
        COMPILE_FLAGS "-fprofile-arcs -ftest-coverage"
    )

    add_executable(zia_common_units
        ${ZIA_COMMON_SRC}
    )

    target_include_directories(zia_common_units PRIVATE ${ZIA_COMMON_INCLUDE_DIRS})
    target_link_libraries(zia_common_units ${ZIA_COMMON_REQUIRED_LIBRARIES} criterion gcov)
endif ()