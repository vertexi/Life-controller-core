cmake_minimum_required(VERSION 3.24)

project(Life-controller-core VERSION 0.0)

set(doctest_header_dir "${CMAKE_CURRENT_SOURCE_DIR}/app/doctest/doctest")
set(MY_APP_NAME Life-controller-app CACHE STRING "App name" FORCE)

add_subdirectory(src)
add_subdirectory(app)
