cmake_minimum_required(VERSION 3.30)

project(forth LANGUAGES C)

add_executable(forth
    main.c
)

target_compile_options(forth PRIVATE
    -Wall
    -Werror
    -Wextra
    -Wpedantic
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address,undefined")

option(TESTING "Build tests for ${PROJECT_NAME}" ON)

if(TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()