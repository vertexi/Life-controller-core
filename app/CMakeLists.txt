cmake_minimum_required(VERSION 3.24)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options("/utf-8")
    set(HELLOIMGUI_WIN32_NO_CONSOLE OFF CACHE BOOL "Disable console window on Windows" FORCE)
endif(MSVC)

add_subdirectory(hello_imgui)
file(GLOB sources *.hpp *.cpp)
hello_imgui_add_app(Life-controller-app ${sources})
target_include_directories(Life-controller-app PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(Life-controller-app PUBLIC Life-controller-core)
target_link_libraries(Life-controller-app INTERFACE rapidjson)

find_package(asio CONFIG REQUIRED)
target_link_libraries(Life-controller-app PRIVATE asio asio::asio)
