cmake_minimum_required(VERSION 3.24)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options("/utf-8")
    set(HELLOIMGUI_WIN32_NO_CONSOLE OFF CACHE BOOL "Disable console window on Windows" FORCE)
endif(MSVC)

# Add imgui_bundle
# =================
# 1/  Option 1: if you added imgui_bundle in a subfolder, you can add it to your project with:
if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/imgui_bundle)
    add_subdirectory(imgui_bundle)
endif()

## 2/  Option 2: simply fetch imgui_bundle during the build
if (NOT TARGET imgui_bundle)
    message(STATUS "Fetching imgui_bundle...")
    include(FetchContent)
    Set(FETCHCONTENT_QUIET FALSE)
    FetchContent_Declare(imgui_bundle GIT_REPOSITORY https://github.com/pthom/imgui_bundle.git GIT_TAG main)
    FetchContent_MakeAvailable(imgui_bundle)

    # Uncomment the next line if you which to also automatically fetch and compile OpenCV for immvision support
    # set(IMMVISION_FETCH_OPENCV ON)
endif()

file(GLOB sources *.hpp *.cpp *.c *.h)
imgui_bundle_add_app(Life-controller-app ${sources})
target_include_directories(Life-controller-app PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(Life-controller-app PUBLIC Life-controller-core)
target_link_libraries(Life-controller-app INTERFACE rapidjson)


find_package(asio CONFIG REQUIRED)
target_link_libraries(Life-controller-app PRIVATE asio asio::asio)

add_subdirectory(tray)
target_link_libraries(Life-controller-app PRIVATE tray::tray)
target_include_directories(Life-controller-app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/tray)
if(MSVC)
    target_link_options(Life-controller-app PRIVATE "/NODEFAULTLIB:library")
endif()

add_custom_command(TARGET Life-controller-app PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/assets/app_settings/icon.ico $<TARGET_FILE_DIR:Life-controller-app>/icon.ico)
add_custom_command(TARGET Life-controller-app PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/assets/app_settings/icon.png $<TARGET_FILE_DIR:Life-controller-app>/icon.png)
