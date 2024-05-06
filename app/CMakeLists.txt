cmake_minimum_required(VERSION 3.24)

# set c++ standard
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

# set pthread for every target
if (EMSCRIPTEN)
    # should compile and link everything with thread support
    add_compile_options(-pthread)
    add_link_options(-pthread)
    add_link_options(-sPTHREAD_POOL_SIZE=8)
    set(HELLOIMGUI_EMSCRIPTEN_PTHREAD ON CACHE BOOL "" FORCE)
    set(HELLOIMGUI_EMSCRIPTEN_PTHREAD_ALLOW_MEMORY_GROWTH ON CACHE BOOL "" FORCE)
endif()

if(WIN32)
    add_compile_options("/utf-8")
    set(HELLOIMGUI_WIN32_NO_CONSOLE OFF CACHE BOOL "Disable console window on Windows" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF)
    set(GLFW_BUILD_TESTS OFF)
    set(GLFW_BUILD_DOCS OFF)
    set(GLFW_INSTALL ON)
    add_subdirectory(imgui_bundle/external/glfw/glfw)
endif(WIN32)

# Add imgui_bundle
add_subdirectory(imgui_bundle)

# Disable clip examples and tests
set(CLIP_EXAMPLES OFF CACHE BOOL "Compile clip examples")
set(CLIP_TESTS OFF CACHE BOOL "Compile clip tests")
set(CLIP_LIB_HEADER_FOLDER ${CMAKE_CURRENT_LIST_DIR}/clip CACHE STRING "Folder where to find clip library headers")

# In case that you have ${PNG_LIBRARY} set to support copy/paste images on Linux
#set(CLIP_X11_PNG_LIBRARY "${PNG_LIBRARY}")

# Add clip subdirectory to compile the library
add_subdirectory(clip)

# build LXGWWenKaiFont font
add_subdirectory(fonts)

file(GLOB sources *.hpp *.cpp *.c *.h)
file(GLOB font_sources)
# set app name and set emscripten idbfs folder name in shell.html
configure_file(../app_config.hpp.in app_config.hpp @ONLY)
imgui_bundle_add_app(${MY_APP_NAME} ${sources} ${font_sources})
target_include_directories(${MY_APP_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${MY_APP_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(${MY_APP_NAME} PUBLIC Life-controller-core)
target_link_libraries(${MY_APP_NAME} INTERFACE rapidjson)
target_link_libraries(${MY_APP_NAME} PRIVATE LXGWWenKaiFont)


if (EMSCRIPTEN)
    target_link_options(${MY_APP_NAME} PRIVATE -Wno-pthreads-mem-growth)
    hello_imgui_set_emscripten_target_initial_memory_megabytes(${MY_APP_NAME} 1024)

    # use asio so set boost headers
    target_compile_options(${MY_APP_NAME} PRIVATE -sUSE_BOOST_HEADERS=1)
    target_link_options(${MY_APP_NAME} PRIVATE -sFORCE_FILESYSTEM)

    # use IDBFS for emscripten storage presistence data
    target_link_options(${MY_APP_NAME} PUBLIC -lidbfs.js)

    # used for provide compile information to clangd
    execute_process(COMMAND em++ --cflags OUTPUT_VARIABLE EM_CFLAGS)
    set_target_properties(${MY_APP_NAME} PROPERTIES COMPILE_FLAGS "${EM_CFLAGS}")
endif()

if (NOT EMSCRIPTEN)
    find_package(asio CONFIG REQUIRED)
    target_link_libraries(${MY_APP_NAME} PRIVATE asio asio::asio)

    add_subdirectory(tray)
    target_link_libraries(${MY_APP_NAME} PRIVATE tray::tray)
    target_include_directories(${MY_APP_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/tray)
endif()

if(MSVC)
    target_link_options(${MY_APP_NAME} PRIVATE "/NODEFAULTLIB:library")
endif()

# Process Abseil's CMake build system
add_subdirectory(abseil-cpp)
# add absl::strings to serialize markdown strings
target_link_libraries(${MY_APP_NAME} PRIVATE absl::strings)

# add doctest for unit testing
target_include_directories(${MY_APP_NAME} PRIVATE ${doctest_header_dir})

add_subdirectory(imlottie)
target_link_libraries(${MY_APP_NAME} PRIVATE imlottie)

set(IMLOTTIE_TEST ON CACHE BOOL "" FORCE)

option(IMLOTTIE_TEST "test imlottie" OFF)

if(IMLOTTIE_TEST)
    if(EMSCRIPTEN)
        add_custom_command(TARGET ${MY_APP_NAME}
            PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${IMLOTTIE_DIR}/test
            ${CMAKE_CURRENT_LIST_DIR}/assets/imlottie_test)
    else()
        add_custom_command(TARGET ${MY_APP_NAME}
            PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${IMLOTTIE_DIR}/test
            $<TARGET_FILE_DIR:${MY_APP_NAME}>/imlottie_test)
    endif()
endif()

if (NOT EMSCRIPTEN)
    # copy assets to target folder
    add_custom_command(TARGET ${MY_APP_NAME}
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_LIST_DIR}/assets/app_settings/icon.ico
        $<TARGET_FILE_DIR:${MY_APP_NAME}>/icon.ico)
    add_custom_command(TARGET ${MY_APP_NAME}
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_LIST_DIR}/assets/app_settings/icon.png
        $<TARGET_FILE_DIR:${MY_APP_NAME}>/icon.png)
else()
    # generate the *.dwp file to debug faster
    add_custom_command(TARGET ${MY_APP_NAME}
        POST_BUILD COMMAND llvm-dwp -e $<TARGET_FILE_DIR:${MY_APP_NAME}>/${MY_APP_NAME}.wasm -o $<TARGET_FILE_DIR:${MY_APP_NAME}>/${MY_APP_NAME}.wasm.dwp)
endif()

# add playground for testing imgui features
if (WIN32)
    if(IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/hello_imgui_demodocking)
        add_subdirectory(hello_imgui_demodocking)
    endif()
    if(IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/hello_imgui_demo_classic)
        add_subdirectory(hello_imgui_demo_classic)
    endif()
    if(IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/demo)
        add_subdirectory(demo)
    endif()
    if(IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/utils)
    add_subdirectory(utils)
    endif()
endif()

add_subdirectory(texocr)
