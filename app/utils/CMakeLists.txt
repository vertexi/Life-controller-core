cmake_minimum_required(VERSION 3.24)

project(windowsInfoGatherer)

find_package(cppzmq CONFIG REQUIRED)

add_executable(windowsInfoGatherer windowsInfoGatherer.cpp zhelpers.hpp)
target_link_libraries(windowsInfoGatherer PRIVATE cppzmq)
