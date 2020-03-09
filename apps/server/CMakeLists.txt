project(zia_server VERSION 1.0.0 LANGUAGES CXX)

set(ZIA_SERVER_REQUIRED_LIBRARIES ${openZiaLibs} zia_common)
set(ZIA_SERVER_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
set(ZIA_SERVER_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(ZIA_SERVER_SRC
    ${ZIA_SERVER_SRC_DIR}/Server.cpp
    ${ZIA_SERVER_SRC_DIR}/Client.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -g")

add_executable(zia src/Main.cpp ${ZIA_SERVER_SRC})

target_include_directories(zia PRIVATE ${ZIA_SERVER_INCLUDE_DIRS})
target_link_libraries(zia ${ZIA_SERVER_REQUIRED_LIBRARIES})

if (UNIX AND NOT APPLE)
    target_link_libraries(zia pthread)
endif ()

if (WIN32)
    target_link_libraries(zia ws2_32)
endif ()

if (BUILD_TESTS)
    set_source_files_properties(${ZIA_SERVER_SRC} PROPERTIES
        COMPILE_FLAGS "-fprofile-arcs -ftest-coverage"
    )

    add_executable(zia_server_units
        ${ZIA_SERVER_SRC}
    )

    target_include_directories(zia_server_units PRIVATE ${ZIA_SERVER_INCLUDE_DIRS})
    target_link_libraries(zia_server_units ${ZIA_SERVER_REQUIRED_LIBRARIES} criterion gcov)
    target_link_libraries(zia gcov)
endif ()