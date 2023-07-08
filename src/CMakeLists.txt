cmake_minimum_required(VERSION 3.0.0)

MACRO(HEADER_DIRECTORIES return_list)
    FILE(GLOB_RECURSE docs_list CONFIGURE_DEPENDS *.h *.hpp)
    SET(dir_list "")
    FOREACH(file_path ${docs_list})
        GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
        SET(dir_list ${dir_list} ${dir_path})
    ENDFOREACH()
    LIST(REMOVE_DUPLICATES dir_list)
    SET(${return_list} ${dir_list})
ENDMACRO()

HEADER_DIRECTORIES(header_dir_list)

list(LENGTH header_dir_list header_dir_list_count)
message(STATUS "[INFO] Found ${header_dir_list_count} header directories.")
foreach(path ${header_dir_list})
message(STATUS "[INFO] - ${path}")
endforeach()


if (CEMU_TEST)
    add_executable(test main.cpp)

    target_include_directories(
        test
        PUBLIC
        ${header_dir_list}
    )

    target_compile_options(test PRIVATE -fno-omit-frame-pointer)
    target_compile_options(test PRIVATE -fsanitize=address)
    target_link_options(test PRIVATE -fsanitize=address)

endif()
