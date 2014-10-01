cmake_minimum_required(VERSION 2.8)
PROJECT(sitoa)

ADD_DEFINITIONS(-std=c++0x)

include_directories (
    include
)
add_executable ( sitoa sitoa.cpp )

#set(TEST_CMDS make_board)
#foreach(TEST_CMD ${TEST_CMDS})
#    add_executable ( ${TEST_CMD} tools/${TEST_CMD}.cpp )
#endforeach(TEST_CMD)

FILE (GLOB_RECURSE clion_all_headers ${CMAKE_SOURCE_DIR}/include/*.h)
ADD_CUSTOM_TARGET(all_clion
    SOURCES ${clion_all_headers}
)
