cmake_minimum_required(VERSION 3.24)

# set vcpkg use static libs
if(CMAKE_TOOLCHAIN_FILE)
    if("${CMAKE_TOOLCHAIN_FILE}" MATCHES "vcpkg.cmake")
        set(VCPKG_TARGET_TRIPLET x64-windows-static)
    endif()
endif()

project(Life-controller-core VERSION 0.0)

set(doctest_header_dir "${CMAKE_CURRENT_SOURCE_DIR}/app/doctest/doctest")
set(MY_APP_NAME Life-controller-app CACHE STRING "App name" FORCE)

add_subdirectory(src)
add_subdirectory(app)
