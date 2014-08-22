cmake_minimum_required(VERSION 2.8)
PROJECT(sitoa)

ADD_DEFINITIONS(-std=c++0x)

include_directories (
    include
)
add_executable ( sitoa sitoa.cpp )

set(TEST_CMDS test_print test_moves make_board test_articulation_points test_clusters test_possible_moves test_sol_distance)
foreach(TEST_CMD ${TEST_CMDS})
    add_executable ( ${TEST_CMD} tools/${TEST_CMD}.cpp )
endforeach(TEST_CMD)


add_executable (test test/test.cpp )