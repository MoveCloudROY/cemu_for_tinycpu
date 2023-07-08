cmake_minimum_required(VERSION 3.0.0)

message(STATUS "[INFO] Configure cemu")

add_library(cemu INTERFACE)
target_include_directories(cemu INTERFACE include)

if (CEMU_TEST)
    add_executable(test main.cpp)

    target_include_directories(
        test
        PUBLIC
        include 
    )

    target_compile_options(test PRIVATE -fno-omit-frame-pointer)
    target_compile_options(test PRIVATE -fsanitize=address)
    target_link_options(test PRIVATE -fsanitize=address)

endif()
