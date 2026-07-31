cmake_minimum_required(VERSION 3.12)
include("CMakeUtils.cmake")

project(tests VERSION 0.0.1 LANGUAGES CXX C)

init_project(
    "tests_cpp11"
    "tests_cpp14"
    "tests_cpp17"
    "tests_cpp20"
    "tests_cpp23"
    "tests_cpp26"
)

fetch_git("deps/string-view-lite" https://github.com/martinmoene/string-view-lite.git master NO)

#if(CMAKE_CXX_STANDARD_EXPERIMENTAL GREATER_EQUAL 17)
#    fetch_git("deps/uni-algo" https://github.com/uni-algo/uni-algo.git v1.2.0 NO)
#    add_subdirectory("deps/uni-algo")
#endif()


add_executable(tests_cpp11 "main.cpp")
target_include_directories(tests_cpp11 PUBLIC "deps/string-view-lite/include/nonstd")
target_compile_definitions(tests_cpp11 PRIVATE _CRT_SECURE_NO_WARNINGS)
set_target_properties(tests_cpp11 PROPERTIES CXX_STANDARD 11)
add_metainfo(tests_cpp11 VERSION 1.0)

if(CMAKE_CXX_STANDARD_EXPERIMENTAL GREATER_EQUAL 14)
    add_executable(tests_cpp14 "main.cpp")
    target_include_directories(tests_cpp14 PUBLIC "deps/string-view-lite/include/nonstd")
    target_compile_definitions(tests_cpp14 PRIVATE _CRT_SECURE_NO_WARNINGS)
    set_target_properties(tests_cpp14 PROPERTIES CXX_STANDARD 14)
    add_metainfo(tests_cpp14 VERSION 1.0)
endif()

if(CMAKE_CXX_STANDARD_EXPERIMENTAL GREATER_EQUAL 17)
    add_executable(tests_cpp17 "main.cpp")
    target_include_directories(tests_cpp17 PUBLIC "deps/string-view-lite/include/nonstd")
    #target_link_libraries(tests_cpp17 PUBLIC uni-algo::uni-algo)
    #target_compile_definitions(tests_cpp17 PUBLIC OPTION_UNI_ALGO_AVAILABLE)
    set_target_properties(tests_cpp17 PROPERTIES CXX_STANDARD 17)
    add_metainfo(tests_cpp17 VERSION 1.0)
endif()

if(CMAKE_CXX_STANDARD_EXPERIMENTAL GREATER_EQUAL 20)
    add_executable(tests_cpp20 "main.cpp")
    target_include_directories(tests_cpp20 PUBLIC "deps/string-view-lite/include/nonstd")
    #target_link_libraries(tests_cpp20 PUBLIC uni-algo::uni-algo)
    #target_compile_definitions(tests_cpp20 PUBLIC OPTION_UNI_ALGO_AVAILABLE)
    set_target_properties(tests_cpp20 PROPERTIES CXX_STANDARD 20)
    add_metainfo(tests_cpp20 VERSION 1.0)
endif()

if(CMAKE_CXX_STANDARD_EXPERIMENTAL GREATER_EQUAL 23)
    add_executable(tests_cpp23 "main.cpp")
    target_include_directories(tests_cpp23 PUBLIC "deps/string-view-lite/include/nonstd")
    #target_link_libraries(tests_cpp23 PUBLIC uni-algo::uni-algo)
    #target_compile_definitions(tests_cpp23 PUBLIC OPTION_UNI_ALGO_AVAILABLE)
    set_target_properties(tests_cpp23 PROPERTIES CXX_STANDARD 23)
    add_metainfo(tests_cpp23 VERSION 1.0)
endif()

if(CMAKE_CXX_STANDARD_EXPERIMENTAL GREATER_EQUAL 26)
    add_executable(tests_cpp26 "main.cpp")
    target_include_directories(tests_cpp26 PUBLIC "deps/string-view-lite/include/nonstd")
    #target_link_libraries(tests_cpp26 PUBLIC uni-algo::uni-algo)
    #target_compile_definitions(tests_cpp26 PUBLIC OPTION_UNI_ALGO_AVAILABLE)
    set_target_properties(tests_cpp26 PROPERTIES CXX_STANDARD 26)
    add_metainfo(tests_cpp26 VERSION 1.0)
endif()

#target_compile_features(tests_cpp11 PRIVATE cxx_std_11)
#target_compile_features(tests_cpp14 PRIVATE cxx_std_14)
#target_compile_features(tests_cpp17 PRIVATE cxx_std_17)
#target_compile_features(tests_cpp20 PRIVATE cxx_std_20)
#target_compile_features(tests_cpp23 PRIVATE cxx_std_23)
