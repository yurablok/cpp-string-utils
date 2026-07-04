cmake_minimum_required(VERSION 3.12)
include("CMakeUtils.cmake")

project(tests VERSION 0.0.1 LANGUAGES CXX C)

init_project(
    "tests_cpp11"
    "tests_cpp14"
    "tests_cpp17"
    "tests_cpp20"
    "tests_cpp23"
)

add_executable(tests_cpp11 "main.cpp")
set_target_properties(tests_cpp11 PROPERTIES CXX_STANDARD 11)
target_compile_definitions(tests_cpp11 PRIVATE _CRT_SECURE_NO_WARNINGS)

add_executable(tests_cpp14 "main.cpp")
set_target_properties(tests_cpp14 PROPERTIES CXX_STANDARD 14)
target_compile_definitions(tests_cpp14 PRIVATE _CRT_SECURE_NO_WARNINGS)

add_executable(tests_cpp17 "main.cpp")
set_target_properties(tests_cpp17 PROPERTIES CXX_STANDARD 17)

add_executable(tests_cpp20 "main.cpp")
set_target_properties(tests_cpp20 PROPERTIES CXX_STANDARD 20)

add_executable(tests_cpp23 "main.cpp")
set_target_properties(tests_cpp23 PROPERTIES CXX_STANDARD 23)

fetch_git("deps/string-view-lite" https://github.com/martinmoene/string-view-lite.git master NO)
target_include_directories(tests_cpp11 PRIVATE "deps/string-view-lite/include/nonstd")
target_include_directories(tests_cpp14 PRIVATE "deps/string-view-lite/include/nonstd")
if(CMAKE_CXX_STANDARD_AVAILABLE LESS 17)
    target_include_directories(tests_cpp17 PRIVATE "deps/string-view-lite/include/nonstd")
    target_include_directories(tests_cpp20 PRIVATE "deps/string-view-lite/include/nonstd")
    target_include_directories(tests_cpp23 PRIVATE "deps/string-view-lite/include/nonstd")
endif()
