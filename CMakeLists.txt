cmake_minimum_required(VERSION 2.8)
PROJECT(sitoa)
include(CTest)

ADD_DEFINITIONS(-std=c++0x)

include_directories (
    include
    vendor
)
add_executable ( sitoa sitoa.cpp )

set(TOOL_CMDS calculate_score expand_move_opponent time_it )
foreach(TOOL_CMD ${TOOL_CMDS})
    add_executable ( ${TOOL_CMD} tools/${TOOL_CMD}.cpp )
    set_target_properties ( ${TOOL_CMD} PROPERTIES COMPILE_FLAGS -mcmodel=large )
endforeach(TOOL_CMD)

add_subdirectory(ext/gtest)

FILE(GLOB_RECURSE TESTSOURCES ${CMAKE_SOURCE_DIR}/tests/*.cpp)

set(TEST_NAMES)
foreach (TESTSOURCE ${TESTSOURCES})
GET_FILENAME_COMPONENT(TESTSOURCE_NAME ${TESTSOURCE} NAME_WE)
add_executable(${TESTSOURCE_NAME} ${TESTSOURCE})
target_link_libraries(${TESTSOURCE_NAME} gtest_main)
add_test(${TESTSOURCE_NAME} ${TESTSOURCE_NAME})
list(APPEND TEST_NAMES ${TESTSOURCE_NAME})
endforeach(TESTSOURCE)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
                  DEPENDS ${TEST_NAMES})


add_executable(all_tests ${TESTSOURCES})
target_link_libraries(all_tests gtest_main)

FILE (GLOB_RECURSE clion_all_headers ${CMAKE_SOURCE_DIR}/include/*.h)
ADD_CUSTOM_TARGET(all_clion
    SOURCES ${clion_all_headers}
)
