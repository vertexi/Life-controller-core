cmake_minimum_required(VERSION 3.24)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options("/utf-8")
endif(MSVC)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # using Clang
    if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
        # using clang with clang-cl front end
        # suppress deprecated declarations warning
        add_compile_options(-Wno-deprecated-declarations)
    elseif (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "GNU")
        # using clang with regular front end
    else()
        # suppress deprecated declarations warning
        add_compile_options(-Wno-deprecated-declarations)
    endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # using GCC
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  # using Intel C++
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # using Visual Studio C++
endif()

configure_file(../app_config.hpp.in app_config.hpp @ONLY)
add_library(rapidjson INTERFACE)
target_include_directories(rapidjson INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
file(GLOB sources *.hpp *.cpp)
add_library(Life-controller-core ${sources})
target_include_directories(Life-controller-core PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(Life-controller-core PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(Life-controller-core INTERFACE rapidjson)

target_include_directories(Life-controller-core PRIVATE ${doctest_header_dir})

if (EMSCRIPTEN)
    target_compile_options(Life-controller-core PUBLIC -pthread)
    target_link_options(Life-controller-core PUBLIC -lidbfs.js)
    target_link_options(Life-controller-core PRIVATE -pthread)
    target_link_options(Life-controller-core PRIVATE -sPTHREAD_POOL_SIZE=3)
endif()
