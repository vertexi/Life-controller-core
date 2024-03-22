cmake_minimum_required(VERSION 3.24)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(rapidjson INTERFACE)
target_include_directories(rapidjson INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
file(GLOB sources *.hpp *.cpp)
add_library(Life-controller-core ${sources})
target_include_directories(Life-controller-core PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(Life-controller-core INTERFACE rapidjson)

if(MSVC)
    add_compile_options("/utf-8")
endif(MSVC)
