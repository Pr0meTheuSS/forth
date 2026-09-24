add_executable(test_stack
    test_stack.c
)

target_include_directories(test_stack PRIVATE
    ${PROJECT_SOURCE_DIR}
)

target_compile_options(test_stack PRIVATE
    -Wall
    -Werror
    -Wextra
    -Wpedantic
)

add_test(
    NAME stack
    COMMAND test_stack
)