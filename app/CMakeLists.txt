cmake_minimum_required(VERSION 3.24)

# set c++ standard
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# set pthread for every target
if (EMSCRIPTEN)
    add_compile_options(-pthread)
    add_link_options(-pthread)
    add_link_options(-sPTHREAD_POOL_SIZE=8)
    set(HELLOIMGUI_EMSCRIPTEN_PTHREAD ON CACHE BOOL "" FORCE)
    set(HELLOIMGUI_EMSCRIPTEN_PTHREAD_ALLOW_MEMORY_GROWTH ON CACHE BOOL "" FORCE)
endif()

if(MSVC)
    add_compile_options("/utf-8")
    set(HELLOIMGUI_WIN32_NO_CONSOLE OFF CACHE BOOL "Disable console window on Windows" FORCE)
endif(MSVC)
# Add imgui_bundle
add_subdirectory(imgui_bundle)

file(GLOB sources *.hpp *.cpp *.c *.h)
file(GLOB font_sources fonts/*.hpp fonts/*.cpp)
# set app name and set emscripten idbfs folder name in shell.html
set(MY_APP_NAME Life-controller-app CACHE STRING "App name" FORCE)
configure_file(app_config.hpp.in app_config.hpp @ONLY)
imgui_bundle_add_app(Life-controller-app ${sources} ${font_sources} app_config.hpp)
target_include_directories(Life-controller-app PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(Life-controller-app PUBLIC Life-controller-core)
target_link_libraries(Life-controller-app INTERFACE rapidjson)

target_include_directories(Life-controller-app PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

if (EMSCRIPTEN)
    target_link_options(Life-controller-app PRIVATE -Wno-pthreads-mem-growth)
    hello_imgui_set_emscripten_target_initial_memory_megabytes(Life-controller-app 1024)
    # use asio so set boost headers
    target_compile_options(Life-controller-app PRIVATE -sUSE_BOOST_HEADERS=1)
    target_link_options(Life-controller-app PRIVATE -sFORCE_FILESYSTEM)
    # use IDBFS for emscripten storage presistence data
    target_link_options(Life-controller-app PUBLIC -lidbfs.js)

    execute_process(COMMAND em++ --cflags OUTPUT_VARIABLE EM_CFLAGS)

    set_target_properties(Life-controller-app PROPERTIES COMPILE_FLAGS "${EM_CFLAGS}")
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

# Process Abseil's CMake build system
add_subdirectory(abseil-cpp)
# add absl::strings to serialize markdown strings
target_link_libraries(Life-controller-app PRIVATE absl::strings)

# add doctest for unit testing
set(doctest_header_dir "${CMAKE_CURRENT_SOURCE_DIR}/doctest/doctest")
target_include_directories(Life-controller-app PRIVATE ${doctest_header_dir})

add_subdirectory(imlottie)
target_link_libraries(Life-controller-app PRIVATE imlottie)

set(IMLOTTIE_TEST ON CACHE BOOL "" FORCE)

option(IMLOTTIE_TEST "test imlottie" OFF)

if(IMLOTTIE_TEST)
    if(EMSCRIPTEN)
        add_custom_command(TARGET Life-controller-app PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${IMLOTTIE_DIR}/test ${CMAKE_CURRENT_LIST_DIR}/assets/imlottie_test)
    else()
        add_custom_command(TARGET Life-controller-app PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${IMLOTTIE_DIR}/test $<TARGET_FILE_DIR:Life-controller-app>/imlottie_test)
    endif()
endif()

if (NOT EMSCRIPTEN)
    # copy assets to target folder
    add_custom_command(TARGET Life-controller-app PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/assets/app_settings/icon.ico $<TARGET_FILE_DIR:Life-controller-app>/icon.ico)
    add_custom_command(TARGET Life-controller-app PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/assets/app_settings/icon.png $<TARGET_FILE_DIR:Life-controller-app>/icon.png)
else()
    # generate the *.dwp file to debug faster
    add_custom_command(TARGET Life-controller-app POST_BUILD COMMAND llvm-dwp -e $<TARGET_FILE_DIR:Life-controller-app>/Life-controller-app.wasm -o $<TARGET_FILE_DIR:Life-controller-app>/Life-controller-app.wasm.dwp)
endif()

# add playground for testing imgui features
if (MSVC)
    if(IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/hello_imgui_demodocking)
        add_subdirectory(hello_imgui_demodocking)
    endif()
    if(IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/hello_imgui_demo_classic)
        add_subdirectory(hello_imgui_demo_classic)
    endif()
endif()
