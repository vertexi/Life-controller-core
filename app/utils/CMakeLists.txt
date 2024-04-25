cmake_minimum_required(VERSION 3.24)

find_package(cppzmq CONFIG REQUIRED)

add_executable(windowsInfoGather windowsInfoGather.cpp zhelpers.hpp)
target_link_libraries(windowsInfoGather PRIVATE cppzmq)
