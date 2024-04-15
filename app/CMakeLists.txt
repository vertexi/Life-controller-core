cmake_minimum_required(VERSION 3.24)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options("/utf-8")
    set(HELLOIMGUI_WIN32_NO_CONSOLE OFF CACHE BOOL "Disable console window on Windows" FORCE)
endif(MSVC)
if (EMSCRIPTEN)
    set(HELLOIMGUI_EMSCRIPTEN_PTHREAD ON CACHE BOOL "" FORCE)
    set(HELLOIMGUI_EMSCRIPTEN_PTHREAD_ALLOW_MEMORY_GROWTH ON CACHE BOOL "" FORCE)
endif()
# Add imgui_bundle
add_subdirectory(imgui_bundle)

file(GLOB sources *.hpp *.cpp *.c *.h)
imgui_bundle_add_app(Life-controller-app ${sources} app_config.hpp)
target_include_directories(Life-controller-app PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(Life-controller-app PUBLIC Life-controller-core)
target_link_libraries(Life-controller-app INTERFACE rapidjson)

set(MY_APP_NAME Life-controller-app CACHE STRING "App name" FORCE)
configure_file(app_config.hpp.in app_config.hpp @ONLY)
target_include_directories(Life-controller-app PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

if (EMSCRIPTEN)
    set(HELLOIMGUI_EMSCRIPTEN_PTHREAD ON CACHE BOOL "" FORCE)
    set(HELLOIMGUI_EMSCRIPTEN_PTHREAD_ALLOW_MEMORY_GROWTH ON CACHE BOOL "" FORCE)
    target_link_options(Life-controller-app PRIVATE -Wno-pthreads-mem-growth)
    hello_imgui_set_emscripten_target_initial_memory_megabytes(Life-controller-app 256)
    target_link_options(Life-controller-app PRIVATE -pthread)
    target_link_options(Life-controller-app PRIVATE -sPTHREAD_POOL_SIZE=3)
    target_compile_options(Life-controller-app PRIVATE -sUSE_BOOST_HEADERS=1)
    target_link_options(Life-controller-app PRIVATE -sFORCE_FILESYSTEM)

    target_link_options(Life-controller-app PUBLIC -lidbfs.js)
endif()

if (NOT EMSCRIPTEN)
    find_package(asio CONFIG REQUIRED)
    target_link_libraries(Life-controller-app PRIVATE asio asio::asio)

    add_subdirectory(tray)
    target_link_libraries(Life-controller-app PRIVATE tray::tray)
    target_include_directories(Life-controller-app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/tray)
endif()

if(MSVC)
    target_link_options(Life-controller-app PRIVATE "/NODEFAULTLIB:library")
endif()

if (NOT EMSCRIPTEN)
    add_custom_command(TARGET Life-controller-app PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/assets/app_settings/icon.ico $<TARGET_FILE_DIR:Life-controller-app>/icon.ico)
    add_custom_command(TARGET Life-controller-app PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/assets/app_settings/icon.png $<TARGET_FILE_DIR:Life-controller-app>/icon.png)
endif()
