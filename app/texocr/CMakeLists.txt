if(MSVC)
    set(HELLOIMGUI_WIN32_NO_CONSOLE OFF CACHE BOOL "Disable console window on Windows" FORCE)
endif(MSVC)
hello_imgui_add_app(texocr texocr.cpp)
target_link_libraries(texocr PRIVATE clip)
target_include_directories(texocr PRIVATE ${CLIP_LIB_HEADER_FOLDER})

target_link_libraries(texocr PRIVATE LXGWWenKaiFont)