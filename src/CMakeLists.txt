
set(EXECUTABLE_NAME remote_exec_helper)
add_executable(${EXECUTABLE_NAME}
        main.cpp
)

set(EXECUTABLE_DUMMY_TEST_NAME remote_hello_world)
add_executable(${EXECUTABLE_DUMMY_TEST_NAME}
        hello_world.cpp
)