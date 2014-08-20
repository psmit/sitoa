cmake_minimum_required(VERSION 2.8)
PROJECT(sitoa)

include_directories (
    include
)
add_executable ( sitoa sitoa.c )

set(TEST_CMDS test_print test_moves)
foreach(TEST_CMD ${TEST_CMDS})
    add_executable ( ${TEST_CMD} tools/${TEST_CMD}.c )
endforeach(TEST_CMD)
