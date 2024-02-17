cmake_minimum_required(VERSION 3.24)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB sources *.hpp *.cpp)
add_executable(Life-controller-app ${sources})
target_include_directories(Life-controller-app PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(Life-controller-app PUBLIC Life-controller-core)

if(MSVC)
    add_compile_options("/utf-8")
endif(MSVC)
