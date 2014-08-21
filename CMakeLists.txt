cmake_minimum_required(VERSION 2.8)
PROJECT(sitoa)

include_directories (
    include
)
add_executable ( sitoa sitoa.c )

set(TEST_CMDS test_print test_moves make_board test_articulation_points test_clusters test_possible_moves test_sol_distance)
foreach(TEST_CMD ${TEST_CMDS})
    add_executable ( ${TEST_CMD} tools/${TEST_CMD}.c )
endforeach(TEST_CMD)
