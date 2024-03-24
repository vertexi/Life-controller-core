cmake_minimum_required(VERSION 3.24)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options("/utf-8")
    set(HELLOIMGUI_WIN32_NO_CONSOLE OFF CACHE BOOL "Disable console window on Windows" FORCE)
endif(MSVC)

include(FetchContent)

FetchContent_Declare(
        tray
        GIT_REPOSITORY https://github.com/wormyrocks/tray.git
        GIT_TAG 3e4a1447bff413e6907c5e43ac8d65e36866fdfd
)
FetchContent_GetProperties(tray)

if (NOT tray_POPULATED)
    FetchContent_Populate(tray)
    add_library(tray INTERFACE)
    target_sources(tray INTERFACE ${tray_SOURCE_DIR}/tray.h)
    target_include_directories(tray INTERFACE ${tray_SOURCE_DIR})
    if (WIN32)
        target_compile_definitions(tray INTERFACE TRAY_WINAPI)
    elseif (APPLE)
        target_compile_definitions(tray INTERFACE TRAY_APPKIT)
        target_link_libraries(tray INTERFACE "-framework Cocoa")
        # Some warnings the library doesn't deal with
        target_compile_options(tray INTERFACE -Wno-unused-parameter)
        target_compile_options(tray INTERFACE -Wno-unused-variable)
        target_compile_options(tray INTERFACE -Wno-writable-strings)
        target_compile_options(tray INTERFACE -Wno-misleading-indentation)
    elseif (UNIX)
        target_compile_definitions(tray INTERFACE TRAY_APPINDICATOR)
        target_link_libraries(tray INTERFACE "$(shell pkg-config --cflags appindicator3-0.1 --libs appindicator3-0.1)")
        # Some warnings the library doesn't deal with
        target_compile_options(tray INTERFACE -Wno-unused-parameter)
        target_compile_options(tray INTERFACE -Wno-unused-variable)
        target_compile_options(tray INTERFACE -Wno-writable-strings)
        target_compile_options(tray INTERFACE -Wno-misleading-indentation)
    endif ()
endif ()

add_subdirectory(hello_imgui)
file(GLOB sources *.hpp *.cpp)
hello_imgui_add_app(Life-controller-app ${sources})
target_include_directories(Life-controller-app PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(Life-controller-app PUBLIC Life-controller-core)
target_link_libraries(Life-controller-app INTERFACE rapidjson)
target_link_libraries(Life-controller-app PUBLIC tray)

if (WIN32)
    file(COPY ${tray_SOURCE_DIR}/icon.ico DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
elseif (APPLE)
    file(COPY ${tray_SOURCE_DIR}/icon.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif ()

find_package(asio CONFIG REQUIRED)
target_link_libraries(Life-controller-app PRIVATE asio asio::asio)
