cmake_minimum_required(VERSION 3.12)
include("CMakeUtils.cmake")

project(tests VERSION 0.0.1 LANGUAGES CXX C)

init_project(
    "tests_cpp11"
    "tests_cpp20"
)

add_executable(tests_cpp11 "main.cpp")
set_target_properties(tests_cpp11 PROPERTIES CXX_STANDARD 11)
target_compile_definitions(tests_cpp11 PRIVATE _CRT_SECURE_NO_WARNINGS)

add_executable(tests_cpp20 "main.cpp")
set_target_properties(tests_cpp20 PROPERTIES CXX_STANDARD 20)

fetch_git("deps/string-view-lite" https://github.com/martinmoene/string-view-lite.git master)
target_include_directories(tests_cpp11 PRIVATE "deps/string-view-lite/include/nonstd")

fetch_git("deps/tiny-utf8" https://github.com/DuffsDevice/tiny-utf8.git master)
target_include_directories(tests_cpp11 PRIVATE "deps/tiny-utf8/include/tinyutf8")
target_include_directories(tests_cpp20 PRIVATE "deps/tiny-utf8/include/tinyutf8")
