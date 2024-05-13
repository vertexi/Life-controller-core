#ifdef __clang__
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

#define STRINGIZE_DETAIL(x) #x
#define STRINGIZE(x) STRINGIZE_DETAIL(x)

#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest_header.h"

#include <chrono>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <string>

#include "config.hpp"
#include <action.hpp>
#include <analysis.hpp>
#include <common.hpp>
#include <log_eval.hpp>

#ifndef __EMSCRIPTEN__
#include "systray.h"
#else
#include <emscripten.h>
#endif

#define IMGUI_DEFINE_MATH_OPERATORS
#include "hello_imgui/hello_imgui.h"
#include "hello_imgui/hello_imgui_assets.h"
#include "hello_imgui/icons_font_awesome_6.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_md_wrapper.h"
#include "immapp/immapp.h"
#include <fstream>

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "ImCoolBar/ImCoolbar.h"

#include "absl/strings/escaping.h"

#ifdef HELLOIMGUI_HAS_OPENGL3
#define HELLOIMGUI_HAS_IMLOTTIE
#define IMLOTTIE_OPENGL_IMPLEMENTATION
// #define IMLOTTIE_SIMPLE_IMPLEMENTATION
#define IMLOTTIE_DEMO
#include "imlottie.h"
#endif

#include "LXGWWenKaiMonoLite.hpp"

struct MyAppSettings
{
    char device_name[256] = "windows";
    bool counting = false;
    std::time_t start_time = 0;
    std::time_t stop_time = 0;

    bool always_on_top = false;
};

static void UnpackAccumulativeOffsetsIntoRanges(int base_codepoint,
                                                const short* accumulative_offsets,
                                                int accumulative_offsets_count,
                                                ImWchar* out_ranges)
{
    for (int n = 0; n < accumulative_offsets_count; n++, out_ranges += 2)
    {
        out_ranges[0] = out_ranges[1] = (ImWchar)(base_codepoint + accumulative_offsets[n]);
        base_codepoint += accumulative_offsets[n];
    }
    out_ranges[0] = 0;
}

const ImWchar* GetGlyphRangesChineseSimplifiedCommon()
{
    // Store 2500 regularly used characters for Simplified Chinese.
    // Sourced from
    // https://zh.wiktionary.org/wiki/%E9%99%84%E5%BD%95:%E7%8E%B0%E4%BB%A3%E6%B1%89%E8%AF%AD%E5%B8%B8%E7%94%A8%E5%AD%97%E8%A1%A8
    // This table covers 97.97% of all characters used during the month in July, 1987.
    // You can use ImFontGlyphRangesBuilder to create your own ranges derived from this, by merging existing
    // ranges or adding new characters. (Stored as accumulative offsets from the initial unicode codepoint
    // 0x4E00. This encoding is designed to helps us compact the source code size.)
    static const short accumulative_offsets_from_0x4E00[] = {
        0,  1,  2,   4,  1,   1,   1,   1,   2,   1,   3,   2,   1,   2,   2,  1,   1,  1,  1,  1,  5,   2,
        1,  2,  3,   3,  3,   2,   2,   4,   1,   1,   1,   2,   1,   5,   2,  3,   1,  2,  1,  2,  1,   1,
        2,  1,  1,   2,  2,   1,   4,   1,   1,   1,   1,   5,   10,  1,   2,  19,  2,  1,  2,  1,  2,   1,
        2,  1,  2,   1,  5,   1,   6,   3,   2,   1,   2,   2,   1,   1,   1,  4,   8,  5,  1,  1,  4,   1,
        1,  3,  1,   2,  1,   5,   1,   2,   1,   1,   1,   10,  1,   1,   5,  2,   4,  6,  1,  4,  2,   2,
        2,  12, 2,   1,  1,   6,   1,   1,   1,   4,   1,   1,   4,   6,   5,  1,   4,  2,  2,  4,  10,  7,
        1,  1,  4,   2,  4,   2,   1,   4,   3,   6,   10,  12,  5,   7,   2,  14,  2,  9,  1,  1,  6,   7,
        10, 4,  7,   13, 1,   5,   4,   8,   4,   1,   1,   2,   28,  5,   6,  1,   1,  5,  2,  5,  20,  2,
        2,  9,  8,   11, 2,   9,   17,  1,   8,   6,   8,   27,  4,   6,   9,  20,  11, 27, 6,  68, 2,   2,
        1,  1,  1,   2,  1,   2,   2,   7,   6,   11,  3,   3,   1,   1,   3,  1,   2,  1,  1,  1,  1,   1,
        3,  1,  1,   8,  3,   4,   1,   5,   7,   2,   1,   4,   4,   8,   4,  2,   1,  2,  1,  1,  4,   5,
        6,  3,  6,   2,  12,  3,   1,   3,   9,   2,   4,   3,   4,   1,   5,  3,   3,  1,  3,  7,  1,   5,
        1,  1,  1,   1,  2,   3,   4,   5,   2,   3,   2,   6,   1,   1,   2,  1,   7,  1,  7,  3,  4,   5,
        15, 2,  2,   1,  5,   3,   22,  19,  2,   1,   1,   1,   1,   2,   5,  1,   1,  1,  6,  1,  1,   12,
        8,  2,  9,   18, 22,  4,   1,   1,   5,   1,   16,  1,   2,   7,   10, 15,  1,  1,  6,  2,  4,   1,
        2,  4,  1,   6,  1,   1,   3,   2,   4,   1,   6,   4,   5,   1,   2,  1,   1,  2,  1,  10, 3,   1,
        3,  2,  1,   9,  3,   2,   5,   7,   2,   19,  4,   3,   6,   1,   1,  1,   1,  1,  4,  3,  2,   1,
        1,  1,  2,   5,  3,   1,   1,   1,   2,   2,   1,   1,   2,   1,   1,  2,   1,  3,  1,  1,  1,   3,
        7,  1,  4,   1,  1,   2,   1,   1,   2,   1,   2,   4,   4,   3,   8,  1,   1,  1,  2,  1,  3,   5,
        1,  3,  1,   3,  4,   6,   2,   2,   14,  4,   6,   6,   11,  9,   1,  15,  3,  1,  28, 5,  2,   5,
        5,  3,  1,   3,  4,   5,   4,   6,   14,  3,   2,   3,   5,   21,  2,  7,   20, 10, 1,  2,  19,  2,
        4,  28, 28,  2,  3,   2,   1,   14,  4,   1,   26,  28,  42,  12,  40, 3,   52, 79, 5,  14, 17,  3,
        2,  2,  11,  3,  4,   6,   3,   1,   8,   2,   23,  4,   5,   8,   10, 4,   2,  7,  3,  5,  1,   1,
        6,  3,  1,   2,  2,   2,   5,   28,  1,   1,   7,   7,   20,  5,   3,  29,  3,  17, 26, 1,  8,   4,
        27, 3,  6,   11, 23,  5,   3,   4,   6,   13,  24,  16,  6,   5,   10, 25,  35, 7,  3,  2,  3,   3,
        14, 3,  6,   2,  6,   1,   4,   2,   3,   8,   2,   1,   1,   3,   3,  3,   4,  1,  1,  13, 2,   2,
        4,  5,  2,   1,  14,  14,  1,   2,   2,   1,   4,   5,   2,   3,   1,  14,  3,  12, 3,  17, 2,   16,
        5,  1,  2,   1,  8,   9,   3,   19,  4,   2,   2,   4,   17,  25,  21, 20,  28, 75, 1,  10, 29,  103,
        4,  1,  2,   1,  1,   4,   2,   4,   1,   2,   3,   24,  2,   2,   2,  1,   1,  2,  1,  3,  8,   1,
        1,  1,  2,   1,  1,   3,   1,   1,   1,   6,   1,   5,   3,   1,   1,  1,   3,  4,  1,  1,  5,   2,
        1,  5,  6,   13, 9,   16,  1,   1,   1,   1,   3,   2,   3,   2,   4,  5,   2,  5,  2,  2,  3,   7,
        13, 7,  2,   2,  1,   1,   1,   1,   2,   3,   3,   2,   1,   6,   4,  9,   2,  1,  14, 2,  14,  2,
        1,  18, 3,   4,  14,  4,   11,  41,  15,  23,  15,  23,  176, 1,   3,  4,   1,  1,  1,  1,  5,   3,
        1,  2,  3,   7,  3,   1,   1,   2,   1,   2,   4,   4,   6,   2,   4,  1,   9,  7,  1,  10, 5,   8,
        16, 29, 1,   1,  2,   2,   3,   1,   3,   5,   2,   4,   5,   4,   1,  1,   2,  2,  3,  3,  7,   1,
        6,  10, 1,   17, 1,   44,  4,   6,   2,   1,   1,   6,   5,   4,   2,  10,  1,  6,  9,  2,  8,   1,
        24, 1,  2,   13, 7,   8,   8,   2,   1,   4,   1,   3,   1,   3,   3,  5,   2,  5,  10, 9,  4,   9,
        12, 2,  1,   6,  1,   10,  1,   1,   7,   7,   4,   10,  8,   3,   1,  13,  4,  3,  1,  6,  1,   3,
        5,  2,  1,   2,  17,  16,  5,   2,   16,  6,   1,   4,   2,   1,   3,  3,   6,  8,  5,  11, 11,  1,
        3,  3,  2,   4,  6,   10,  9,   5,   7,   4,   7,   4,   7,   1,   1,  4,   2,  1,  3,  6,  8,   7,
        1,  6,  11,  5,  5,   3,   24,  9,   4,   2,   7,   13,  5,   1,   8,  82,  16, 61, 1,  1,  1,   4,
        2,  2,  16,  10, 3,   8,   1,   1,   6,   4,   2,   1,   3,   1,   1,  1,   4,  3,  8,  4,  2,   2,
        1,  1,  1,   1,  1,   6,   3,   5,   1,   1,   4,   6,   9,   2,   1,  1,   1,  2,  1,  7,  2,   1,
        6,  1,  5,   4,  4,   3,   1,   8,   1,   3,   3,   1,   3,   2,   2,  2,   2,  3,  1,  6,  1,   2,
        1,  2,  1,   3,  7,   1,   8,   2,   1,   2,   1,   5,   2,   5,   3,  5,   10, 1,  2,  1,  1,   3,
        2,  5,  11,  3,  9,   3,   5,   1,   1,   5,   9,   1,   2,   1,   5,  7,   9,  9,  8,  1,  3,   3,
        3,  6,  8,   2,  3,   2,   1,   1,   32,  6,   1,   2,   15,  9,   3,  7,   13, 1,  3,  10, 13,  2,
        14, 1,  13,  10, 2,   1,   3,   10,  4,   15,  2,   15,  15,  10,  1,  3,   9,  6,  9,  32, 25,  26,
        47, 7,  3,   2,  3,   1,   6,   3,   4,   3,   2,   8,   5,   4,   1,  9,   4,  2,  2,  19, 10,  6,
        2,  3,  8,   1,  2,   2,   4,   2,   1,   9,   4,   4,   4,   6,   4,  8,   9,  2,  3,  1,  1,   1,
        1,  3,  5,   5,  1,   3,   8,   4,   6,   2,   1,   4,   12,  1,   5,  3,   7,  13, 2,  5,  8,   1,
        6,  1,  2,   5,  14,  6,   1,   5,   2,   4,   8,   15,  5,   1,   23, 6,   62, 2,  10, 1,  1,   8,
        1,  2,  2,   10, 4,   2,   2,   9,   2,   1,   1,   3,   2,   3,   1,  5,   3,  3,  2,  1,  3,   8,
        1,  1,  1,   11, 3,   1,   1,   4,   3,   7,   1,   14,  1,   2,   3,  12,  5,  2,  5,  1,  6,   7,
        5,  7,  14,  11, 1,   3,   1,   8,   9,   12,  2,   1,   11,  8,   4,  4,   2,  6,  10, 9,  13,  1,
        1,  3,  1,   5,  1,   3,   2,   4,   4,   1,   18,  2,   3,   14,  11, 4,   29, 4,  2,  7,  1,   3,
        13, 9,  2,   2,  5,   3,   5,   20,  7,   16,  8,   5,   72,  34,  6,  4,   22, 12, 12, 28, 45,  36,
        9,  7,  39,  9,  191, 1,   1,   1,   4,   11,  8,   4,   9,   2,   3,  22,  1,  1,  1,  1,  4,   17,
        1,  7,  7,   1,  11,  31,  10,  2,   4,   8,   2,   3,   2,   1,   4,  2,   16, 4,  32, 2,  3,   19,
        13, 4,  9,   1,  5,   2,   14,  8,   1,   1,   3,   6,   19,  6,   5,  1,   16, 6,  2,  10, 8,   5,
        1,  2,  3,   1,  5,   5,   1,   11,  6,   6,   1,   3,   3,   2,   6,  3,   8,  1,  1,  4,  10,  7,
        5,  7,  7,   5,  8,   9,   2,   1,   3,   4,   1,   1,   3,   1,   3,  3,   2,  6,  16, 1,  4,   6,
        3,  1,  10,  6,  1,   3,   15,  2,   9,   2,   10,  25,  13,  9,   16, 6,   2,  2,  10, 11, 4,   3,
        9,  1,  2,   6,  6,   5,   4,   30,  40,  1,   10,  7,   12,  14,  33, 6,   3,  6,  7,  3,  1,   3,
        1,  11, 14,  4,  9,   5,   12,  11,  49,  18,  51,  31,  140, 31,  2,  2,   1,  5,  1,  8,  1,   10,
        1,  4,  4,   3,  24,  1,   10,  1,   3,   6,   6,   16,  3,   4,   5,  2,   1,  4,  2,  57, 10,  6,
        22, 2,  22,  3,  7,   22,  6,   10,  11,  36,  18,  16,  33,  36,  2,  5,   5,  1,  1,  1,  4,   10,
        1,  4,  13,  2,  7,   5,   2,   9,   3,   4,   1,   7,   43,  3,   7,  3,   9,  14, 7,  9,  1,   11,
        1,  1,  3,   7,  4,   18,  13,  1,   14,  1,   3,   6,   10,  73,  2,  2,   30, 6,  1,  11, 18,  19,
        13, 22, 3,   46, 42,  37,  89,  7,   3,   16,  34,  2,   2,   3,   9,  1,   7,  1,  1,  1,  2,   2,
        4,  10, 7,   3,  10,  3,   9,   5,   28,  9,   2,   6,   13,  7,   3,  1,   3,  10, 2,  7,  2,   11,
        3,  6,  21,  54, 85,  2,   1,   4,   2,   2,   1,   39,  3,   21,  2,  2,   5,  1,  1,  1,  4,   1,
        1,  3,  4,   15, 1,   3,   2,   4,   4,   2,   3,   8,   2,   20,  1,  8,   7,  13, 4,  1,  26,  6,
        2,  9,  34,  4,  21,  52,  10,  4,   4,   1,   5,   12,  2,   11,  1,  7,   2,  30, 12, 44, 2,   30,
        1,  1,  3,   6,  16,  9,   17,  39,  82,  2,   2,   24,  7,   1,   7,  3,   16, 9,  14, 44, 2,   1,
        2,  1,  2,   3,  5,   2,   4,   1,   6,   7,   5,   3,   2,   6,   1,  11,  5,  11, 2,  1,  18,  19,
        8,  1,  3,   24, 29,  2,   1,   3,   5,   2,   2,   1,   13,  6,   5,  1,   46, 11, 3,  5,  1,   1,
        5,  8,  2,   10, 6,   12,  6,   3,   7,   11,  2,   4,   16,  13,  2,  5,   1,  1,  2,  2,  5,   2,
        28, 5,  2,   23, 10,  8,   4,   4,   22,  39,  95,  38,  8,   14,  9,  5,   1,  13, 5,  4,  3,   13,
        12, 11, 1,   9,  1,   27,  37,  2,   5,   4,   4,   63,  211, 95,  2,  2,   2,  1,  3,  5,  2,   1,
        1,  2,  2,   1,  1,   1,   3,   2,   4,   1,   2,   1,   1,   5,   2,  2,   1,  1,  2,  3,  1,   3,
        1,  1,  1,   3,  1,   4,   2,   1,   3,   6,   1,   1,   3,   7,   15, 5,   3,  2,  5,  3,  9,   11,
        4,  2,  22,  1,  6,   3,   8,   7,   1,   4,   28,  4,   16,  3,   3,  25,  4,  4,  27, 27, 1,   4,
        1,  2,  2,   7,  1,   3,   5,   2,   28,  8,   2,   14,  1,   8,   6,  16,  25, 3,  3,  3,  14,  3,
        3,  1,  1,   2,  1,   4,   6,   3,   8,   4,   1,   1,   1,   2,   3,  6,   10, 6,  2,  3,  18,  3,
        2,  5,  5,   4,  3,   1,   5,   2,   5,   4,   23,  7,   6,   12,  6,  4,   17, 11, 9,  5,  1,   1,
        10, 5,  12,  1,  1,   11,  26,  33,  7,   3,   6,   1,   17,  7,   1,  5,   12, 1,  11, 2,  4,   1,
        8,  14, 17,  23, 1,   2,   1,   7,   8,   16,  11,  9,   6,   5,   2,  6,   4,  16, 2,  8,  14,  1,
        11, 8,  9,   1,  1,   1,   9,   25,  4,   11,  19,  7,   2,   15,  2,  12,  8,  52, 7,  5,  19,  2,
        16, 4,  36,  8,  1,   16,  8,   24,  26,  4,   6,   2,   9,   5,   4,  36,  3,  28, 12, 25, 15,  37,
        27, 17, 12,  59, 38,  5,   32,  127, 1,   2,   9,   17,  14,  4,   1,  2,   1,  1,  8,  11, 50,  4,
        14, 2,  19,  16, 4,   17,  5,   4,   5,   26,  12,  45,  2,   23,  45, 104, 30, 12, 8,  3,  10,  2,
        2,  3,  3,   1,  4,   20,  7,   2,   9,   6,   15,  2,   20,  1,   3,  16,  4,  11, 15, 6,  134, 2,
        5,  59, 1,   2,  2,   2,   1,   9,   17,  3,   26,  137, 10,  211, 59, 1,   2,  4,  1,  4,  1,   1,
        1,  2,  6,   2,  3,   1,   1,   2,   3,   2,   3,   1,   3,   4,   4,  2,   3,  3,  1,  4,  3,   1,
        7,  2,  2,   3,  1,   2,   1,   3,   3,   3,   2,   2,   3,   2,   1,  3,   14, 6,  1,  3,  2,   9,
        6,  15, 27,  9,  34,  145, 1,   1,   2,   1,   1,   1,   1,   2,   1,  1,   1,  1,  2,  2,  2,   3,
        1,  2,  1,   1,  1,   2,   3,   5,   8,   3,   5,   2,   4,   1,   3,  2,   2,  2,  12, 4,  1,   1,
        1,  10, 4,   5,  1,   20,  4,   16,  1,   15,  9,   5,   12,  2,   9,  2,   5,  4,  2,  26, 19,  7,
        1,  26, 4,   30, 12,  15,  42,  1,   6,   8,   172, 1,   1,   4,   2,  1,   1,  11, 2,  2,  4,   2,
        1,  2,  1,   10, 8,   1,   2,   1,   4,   5,   1,   2,   5,   1,   8,  4,   1,  3,  4,  2,  1,   6,
        2,  1,  3,   4,  1,   2,   1,   1,   1,   1,   12,  5,   7,   2,   4,  3,   1,  1,  1,  3,  3,   6,
        1,  2,  2,   3,  3,   3,   2,   1,   2,   12,  14,  11,  6,   6,   4,  12,  2,  8,  1,  7,  10,  1,
        35, 7,  4,   13, 15,  4,   3,   23,  21,  28,  52,  5,   26,  5,   6,  1,   7,  10, 2,  7,  53,  3,
        2,  1,  1,   1,  2,   163, 532, 1,   10,  11,  1,   3,   3,   4,   8,  2,   8,  6,  2,  2,  23,  22,
        4,  2,  2,   4,  2,   1,   3,   1,   3,   3,   5,   9,   8,   2,   1,  2,   8,  1,  10, 2,  12,  21,
        20, 15, 105, 2,  3,   1,   1,   3,   2,   3,   1,   1,   2,   5,   1,  4,   15, 11, 19, 1,  1,   1,
        1,  5,  4,   5,  1,   1,   2,   5,   3,   5,   12,  1,   2,   5,   1,  11,  1,  1,  15, 9,  1,   4,
        5,  3,  26,  8,  2,   1,   3,   1,   1,   15,  19,  2,   12,  1,   2,  5,   2,  7,  2,  19, 2,   20,
        6,  26, 7,   5,  2,   2,   7,   34,  21,  13,  70,  2,   128, 1,   1,  2,   1,  1,  2,  1,  1,   3,
        2,  2,  2,   15, 1,   4,   1,   3,   4,   42,  10,  6,   1,   49,  85, 8,   1,  2,  1,  1,  4,   4,
        2,  3,  6,   1,  5,   7,   4,   3,   211, 4,   1,   2,   1,   2,   5,  1,   2,  4,  2,  2,  6,   5,
        6,  10, 3,   4,  48,  100, 6,   2,   16,  296, 5,   27,  387, 2,   2,  3,   7,  16, 8,  5,  38,  15,
        39, 21, 9,   10, 3,   7,   59,  13,  27,  21,  47,  5,   21,  6};
    static ImWchar base_ranges[] =  // not zero-terminated
        {
            0x0020,
            0x00FF,  // Basic Latin + Latin Supplement
            0x2000,
            0x206F,  // General Punctuation
            0x3000,
            0x30FF,  // CJK Symbols and Punctuations, Hiragana, Katakana
            0x31F0,
            0x31FF,  // Katakana Phonetic Extensions
            0xFF00,
            0xFFEF,  // Half-width characters
            0xFFFD,
            0xFFFD  // Invalid
        };
    static ImWchar
        full_ranges[IM_ARRAYSIZE(base_ranges) + IM_ARRAYSIZE(accumulative_offsets_from_0x4E00) * 2 + 1] = {0};
    if (!full_ranges[0])
    {
        memcpy(full_ranges, base_ranges, sizeof(base_ranges));
        UnpackAccumulativeOffsetsIntoRanges(0x4E00,
                                            accumulative_offsets_from_0x4E00,
                                            IM_ARRAYSIZE(accumulative_offsets_from_0x4E00),
                                            full_ranges + IM_ARRAYSIZE(base_ranges));
    }
    return &full_ranges[0];
}

#ifdef EMSCRIPTEN
#include "boost/asio.hpp"
namespace asio = boost::asio;
#else
#include "asio.hpp"
#endif

asio::io_service io_service;

#include "windowBehaviour.h"

struct AppState
{
    MyAppSettings myAppSettings;

    ImFont* TitleFont = nullptr;
    ImFont* ColorFont = nullptr;
    ImFont* EmojiFont = nullptr;
    ImFont* LargeIconFont = nullptr;
    ImFont* TimeFont = nullptr;
    ImFont* TimeFontSmall = nullptr;
    ImFont* ChineseFont = nullptr;
};

std::string MyAppSettingsToString(const MyAppSettings& myAppSettings)
{
    rapidjson::Document d;
    d.SetObject();
    auto& allocator = d.GetAllocator();
    rapidjson::Value deviceName(myAppSettings.device_name, allocator);
    rapidjson::Value counting(myAppSettings.counting);
    rapidjson::Value startTime((int64_t)myAppSettings.start_time);
    rapidjson::Value always_on_top(myAppSettings.always_on_top);

    d.AddMember("deviceName", deviceName, allocator);
    d.AddMember("counting", counting, allocator);
    d.AddMember("startTime", startTime, allocator);
    d.AddMember("always_on_top", always_on_top, allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    d.Accept(writer);
    return buffer.GetString();
}
MyAppSettings StringToMyAppSettings(const std::string& s)
{
    MyAppSettings myAppSettings;
    rapidjson::Document d;
    d.Parse(s.c_str());
    if (d.IsObject())
    {
        if (d.HasMember("deviceName"))
        {
            strcpy(myAppSettings.device_name, d["deviceName"].GetString());
        }
        if (d.HasMember("counting"))
        {
            myAppSettings.counting = d["counting"].GetBool();
        }
        if (d.HasMember("startTime"))
        {
            myAppSettings.start_time = d["startTime"].GetInt64();
        }
        if (d.HasMember("always_on_top"))
        {
            myAppSettings.always_on_top = d["always_on_top"].GetBool();
        }
    }
    return myAppSettings;
}

// Note: LoadUserSettings() and SaveUserSettings() will be called in the callbacks `PostInit` and
// `BeforeExit`:
//     runnerParams.callbacks.PostInit = [&appState]   { LoadMyAppSettings(appState);};
//     runnerParams.callbacks.BeforeExit = [&appState] { SaveMyAppSettings(appState);};
void LoadMyAppSettings(AppState& appState)  //
{
    appState.myAppSettings = StringToMyAppSettings(HelloImGui::LoadUserPref("MyAppSettings"));
}
void SaveMyAppSettings(const AppState& appState)
{
    HelloImGui::SaveUserPref("MyAppSettings", MyAppSettingsToString(appState.myAppSettings));
}

void StatusBarGui(AppState& app_state)
{
    ImGui::Text("Using backend: %s", HelloImGui::GetBackendDescription().c_str());
}

void LoadFonts(AppState& appState)  // This is called by runnerParams.callbacks.LoadAdditionalFonts
{
    HelloImGui::GetRunnerParams()->callbacks.defaultIconFont = HelloImGui::DefaultIconFont::FontAwesome6;
    // First, load the default font (the default font should be loaded first)
    HelloImGui::ImGuiDefaultSettings::LoadDefaultFont_WithFontAwesomeIcons();
    // Then load the other fonts
    appState.TitleFont = HelloImGui::LoadFont("fonts/DroidSans.ttf", 18.f);

    appState.TimeFont = HelloImGui::LoadFont("fonts/Roboto/Roboto-BoldItalic.ttf", 80.f);

    appState.TimeFontSmall = HelloImGui::LoadFont("fonts/Roboto/Roboto-BoldItalic.ttf", 30.f);

    HelloImGui::FontLoadingParams fontLoadingParamsChinese;
    fontLoadingParamsChinese.mergeFontAwesome = true;
    fontLoadingParamsChinese.glyphRanges =
        HelloImGui::ImWchar2ImWcharPairs(GetGlyphRangesChineseSimplifiedCommon());
    appState.ChineseFont = HelloImGui::LoadFont((void*)get_LXGWWenKaiMonoLite_Regular(),
                                                get_LXGWWenKaiMonoLite_RegularItalic_size(),
                                                30.f,
                                                fontLoadingParamsChinese);
}
#ifdef EMSCRIPTEN
void updateTime(const boost::system::error_code& ec, asio::steady_timer* t, char* time_str, size_t bufsize)
#else
void updateTime(const asio::error_code& ec, asio::steady_timer* t, char* time_str, size_t bufsize)
#endif
{
    if (ec)
    {
        std::cout << "timer error: " << ec.message() << "\n";
        return;
    }
    std::time_t now = std::time(nullptr);
    std::strftime(time_str, 20, "%H:%M:%S", std::localtime(&now));
    t->expires_at(t->expiry() + asio::chrono::seconds(1));
    t->async_wait(std::bind(updateTime, std::placeholders::_1, t, time_str, bufsize));
}
#ifdef EMSCRIPTEN
void updateTimer(const boost::system::error_code& ec,
                 asio::steady_timer* t,
                 std::time_t start_time,
                 char* time_str,
                 size_t bufsize)
#else
void updateTimer(
    const asio::error_code& ec, asio::steady_timer* t, std::time_t start_time, char* time_str, size_t bufsize)
#endif
{
    if (ec)
    {
        std::cout << "timer error: " << ec.message() << "\n";
        return;
    }
    auto duaration_str = get_duration_str(start_time, std::time(nullptr));
    strcpy(time_str, duaration_str.c_str());
    t->expires_at(t->expiry() + asio::chrono::seconds(1));
    t->async_wait(std::bind(updateTimer, std::placeholders::_1, t, start_time, time_str, bufsize));
}

bool ButtonCenteredOnLine(const char* label, float alignment = 0.5f)
{
    ImGuiStyle& style = ImGui::GetStyle();

    float size = ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f;
    float avail = ImGui::GetContentRegionAvail().x;

    float off = (avail - size) * alignment;
    if (off > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

    return ImGui::Button(label);
}

void TextCenteredOnLine(const char* label, float alignment = 0.5f)
{
    ImGuiStyle& style = ImGui::GetStyle();

    float size = ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f;
    float avail = ImGui::GetContentRegionAvail().x;

    float off = (avail - size) * alignment;
    if (off > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

    ImGui::Text("%s", label);
}

void AlignForWidth(float width, float alignment = 0.5f)
{
    ImGuiStyle& style = ImGui::GetStyle();
    float avail = ImGui::GetContentRegionAvail().x;
    float off = (avail - width) * alignment;
    if (off > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
}

enum SCREEN_STATE
{
    Time_S,
    Timer_S,
    Setting_S,
    Timer_End_S,
    Event_List_S,
    Event_Edit_S
};

void DemoCoolBar(enum SCREEN_STATE& current_screen)
{
    auto ShowCoolBarButton = [](const std::string& anim) -> bool
    {
#ifdef EMSCRIPTEN
        static std::string imlottie_base_dir = HelloImGui::AssetFileFullPath("imlottie_test/");
#else
        static std::string imlottie_base_dir = "imlottie_test/";
#endif
        auto _ = [](const std::string& anim) { return imlottie_base_dir + anim + ".json"; };
        float w = ImGui::GetCoolBarItemWidth();

        // Display transparent image and check if clicked
        ImLottie::LottieAnimation(_(anim).c_str(), ImVec2(64, 64), ImVec2(w, w), true, 0);
        // HelloImGui::ImageFromAsset("images/world.png", ImVec2(w, w));
        bool clicked = ImGui::IsItemHovered() && ImGui::IsMouseClicked(0);

        // // Optional: add a label on the image
        // {
        //     ImVec2 topLeftCorner = ImGui::GetItemRectMin();
        //     ImVec2 textPos(topLeftCorner.x + ImmApp::EmSize(1.f), topLeftCorner.y + ImmApp::EmSize(1.f));
        //     ImGui::GetForegroundDrawList()->AddText(textPos, 0xFFFFFFFF, anim.c_str());
        // }

        return clicked;
    };

    std::vector<std::string> buttonLabels{"speaker", "cubes", "emojilove", "car", "seeu", "freeside"};

    ImGui::ImCoolBarConfig coolBarConfig;
    coolBarConfig.anchor = ImVec2(0.5f, 0.07f);  // position in the window (ratio of window size)
    coolBarConfig.normal_size = 10.0f;
    coolBarConfig.hovered_size = 60.0f;
    coolBarConfig.anim_step = 0.05f;
    coolBarConfig.effect_strength = 0.8f;
    if (ImGui::BeginCoolBar("##CoolBarMain", ImCoolBarFlags_Horizontal, coolBarConfig))
    {
        for (const std::string& label : buttonLabels)
        {
            if (ImGui::CoolBarItem())
            {
                if (ShowCoolBarButton(label))
                {
                    if (label == "speaker")
                    {
                        current_screen = Event_List_S;
                    }
                    else if (label == "cubes")
                    {
                        current_screen = Setting_S;
                    }
                    else
                    {
                        current_screen = Time_S;
                    }
                }
            }
        }
        ImGui::EndCoolBar();
    }

    ImGui::NewLine();
    ImGui::NewLine();
}

#define TIME_STR_LEN 20
void guiFunction(AppState& appState)
{
#ifndef __EMSCRIPTEN__
    if (tray_loop(0) == -1)
    {
        HelloImGui::GetRunnerParams()->appShallExit = true;
    }
#endif

    static bool first_time = true;
    static enum SCREEN_STATE current_screen = Time_S;

    static char time_str[TIME_STR_LEN] = "00:00:00";
    if (first_time)
    {
        always_on_top(appState.myAppSettings.always_on_top);
        std::time_t now = std::time(nullptr);
        std::strftime(time_str, TIME_STR_LEN, "%H:%M:%S", std::localtime(&now));
        static asio::steady_timer t(io_service, asio::chrono::seconds(1));
        t.async_wait(std::bind(updateTime, std::placeholders::_1, &t, time_str, sizeof(time_str)));

        if (appState.myAppSettings.counting)
        {
            current_screen = Timer_S;
        }
    }
    first_time = false;

    ImGui::NewLine();

    ImGuiStyle& style = ImGui::GetStyle();

    switch (current_screen)
    {
        case Time_S:
        {
            static bool first_time = true;

            first_time = false;

            DemoCoolBar(current_screen);

            // ImVec2 dragSize(ImGui::GetMainViewport()->Size.x, ImGui::GetFontSize() * 1.5f);
            // ImRect dragArea(ImGui::GetMainViewport()->Pos, ImGui::GetMainViewport()->Pos + dragSize);
            // ImVec2 mousePos = ImGui::GetMousePos();

            // if (ImGui::BeginPopupContextItem("ContextMenu"))
            // {
            //     ImGui::PushFont(appState.TitleFont);
            //     if (ImGui::Selectable("Settings"))
            //     {
            //         current_screen = Setting_S;
            //     }
            //     ImGui::PopFont();
            //     ImGui::EndPopup();
            // }
            // if (dragArea.Contains(mousePos) && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
            // {
            //     ImGui::OpenPopup("ContextMenu");
            // }

            float avail = ImGui::GetContentRegionAvail().y;
            ImGui::SetCursorPosY(avail * 0.3f + style.FramePadding.y);
            ImGui::PushFont(appState.TimeFont);
            TextCenteredOnLine(time_str);
            ImGui::PopFont();
            ImGui::PushFont(appState.TimeFontSmall);
            if (ButtonCenteredOnLine("Start"))
            {
                appState.myAppSettings.counting = true;
                current_screen = Timer_S;
                std::time_t current = std::time(nullptr);
                appState.myAppSettings.start_time = current;
                printf("Counting started!\n");
                printf("Start time: %s", std::ctime(&current));
            }
            ImGui::PopFont();
        }
        break;
        case Timer_S:
        {
            static bool first_time = true;
            static char timer_str[TIME_STR_LEN] = "00:00:00";
            static asio::steady_timer t(io_service, asio::chrono::seconds(1));
            if (first_time)
            {
                std::time_t start_time = std::time(nullptr);
                if (appState.myAppSettings.counting)
                {
                    start_time = appState.myAppSettings.start_time;
                }
                auto duaration_str = get_duration_str(start_time, std::time(nullptr));
                strcpy(timer_str, duaration_str.c_str());
                t.expires_from_now(asio::chrono::seconds(1));
                t.async_wait(std::bind(
                    updateTimer, std::placeholders::_1, &t, start_time, timer_str, sizeof(timer_str)));
            }
            first_time = false;

            float avail = ImGui::GetContentRegionAvail().y;
            ImGui::SetCursorPosY(avail * 0.3f);
            ImGui::PushFont(appState.TimeFontSmall);
            TextCenteredOnLine(time_str);
            ImGui::PopFont();
            ImGui::PushFont(appState.TimeFont);
            TextCenteredOnLine(timer_str);
            ImGui::PopFont();
            ImGui::PushFont(appState.TimeFontSmall);
            if (ButtonCenteredOnLine("Stop"))
            {
                strcpy(timer_str, "00:00:00");
                appState.myAppSettings.counting = false;
                appState.myAppSettings.stop_time = std::time(nullptr);
                current_screen = Timer_End_S;
                printf("Counting stopped!\n");
                printf("Start time: %s", std::ctime(&appState.myAppSettings.start_time));
                t.cancel();
                first_time = true;
            }
            ImGui::PopFont();
        }
        break;
        case Setting_S:
        {
            static bool first_time = true;
            static bool always_on_top = appState.myAppSettings.always_on_top;
            ImGui::PushFont(appState.TitleFont);
            ImGui::Checkbox("Always on top", &always_on_top);
            if (always_on_top != appState.myAppSettings.always_on_top)
            {
                ::always_on_top(always_on_top);
                appState.myAppSettings.always_on_top = always_on_top;
            }
            static char device_name[256] = "";
            if (first_time)
            {
                strcpy(device_name, appState.myAppSettings.device_name);
            }
            first_time = false;
            ImGui::Text("device name");
            ImGui::SameLine();
            ImGui::InputText("##", device_name, sizeof(device_name));

            float width = 0.0f;
            width += HelloImGui::EmSize(5.0f);
            width += style.ItemSpacing.x;
            width += HelloImGui::EmSize(5.0f);
            width += style.ItemSpacing.x;
            width += HelloImGui::EmSize(5.0f);
            AlignForWidth(width);

            if (ImGui::Button("OK", HelloImGui::EmToVec2(5.0f, 0.0f)))
            {
                strcpy(appState.myAppSettings.device_name, device_name);
                current_screen = Time_S;
                first_time = true;
            }
            ImGui::SameLine(0, HelloImGui::EmSize(5.0f));
            if (ImGui::Button("CANCEL", HelloImGui::EmToVec2(5.0f, 0.0f)))
            {
                current_screen = Time_S;
                first_time = true;
            }
            ImGui::PopFont();
        }
        break;
        case Timer_End_S:
        {
            static bool first_time = true;
            static bool markdownEdit = true;
            static bool event_create = false;
            static char event_name[256] = "";
            static char MarkdownInput[1024] = "";
            static std::time_t duration_time, duration_sec, duration_min, duration_hour = 0;
            static std::set<std::string> event_names;
            static auto choosen_event = event_names.end();

            auto reset_duration_time = [&]()
            {
                duration_time = appState.myAppSettings.stop_time - appState.myAppSettings.start_time;
                duration_sec = duration_time;
                duration_min = duration_sec / 60;
                duration_sec = duration_sec % 60;
                duration_hour = duration_min / 60;
                duration_min = duration_min % 60;
            };
            if (first_time)
            {
                markdownEdit = true;
                event_create = false;
                get_event_names(event_names);
                choosen_event = event_names.end();
                reset_duration_time();
                MarkdownInput[0] = '\0';
            }
            first_time = false;

            const float drag_speed = 1.0f;
            const ImU8 hour_min = 0;
            const ImU8 hour_max = 24;
            const ImU8 min_min = 0;
            const ImU8 min_max = 60;

            ImGui::PushFont(appState.TimeFontSmall);
            static auto stopTimeTextWidth = ImGui::CalcTextSize("88m").x;
            ImGuiStyle& style = ImGui::GetStyle();
            float avail = ImGui::GetContentRegionAvail().x;
            static float itemsWidth = (stopTimeTextWidth + style.FramePadding.x) * 2 +
                                      ImGui::CalcTextSize("Duration ").x + ImGui::CalcTextSize("reset").x +
                                      style.FramePadding.x * 4;
            float off = (avail - itemsWidth) * 0.5f;
            if (off > 0.0f)
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
            ImGui::Text("Duration ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(stopTimeTextWidth);
            ImGui::DragScalar(
                "##stop_hour", ImGuiDataType_U8, &duration_hour, drag_speed, &hour_min, &hour_max, "%dh");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(stopTimeTextWidth);
            ImGui::DragScalar(
                "##stop_min", ImGuiDataType_U8, &duration_min, drag_speed, &min_min, &min_max, "%dm");
            ImGui::SameLine();
            if (ImGui::Button("reset"))
            {
                reset_duration_time();
            }

            std::string combo_preview_value =
                choosen_event == event_names.end() ? "" : choosen_event->c_str();
            ImGui::Text("Event name");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(HelloImGui::EmSize(7.0f));
            if (!event_create)
            {
                if (ImGui::BeginCombo("##event name", combo_preview_value.c_str()))
                {
                    for (auto it = event_names.begin(); it != event_names.end(); it++)
                    {
                        const bool is_selected = (choosen_event == it);
                        if (ImGui::Selectable(it->c_str(), is_selected))
                            choosen_event = it;

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation
                        // focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                ImGui::SameLine();
                if (ImGui::Button("Create",
                                  ImVec2(ImGui::CalcTextSize("Create").x + style.FramePadding.x * 2, 0.0f)))
                {
                    event_create = true;
                }
            }
            else
            {
                ImGui::InputText("##event name", event_name, sizeof(event_name));
                ImGui::SameLine();
                if (ImGui::Button("Confirm",
                                  ImVec2(ImGui::CalcTextSize("Confirm").x + style.FramePadding.x * 2, 0.0f)))
                {
                    if (strlen(event_name) > 0)
                    {
                        create_event_log(event_name, MarkdownInput);
                        eval_log_line_str((char*)life_controller_core::get_last_append_line().c_str());
                    }
                    first_time = true;
                }
            }

            if (markdownEdit)
            {
                ImGui::PushFont(appState.ChineseFont);
                ImGui::InputTextMultiline("##Markdown Input",
                                          MarkdownInput,
                                          sizeof(MarkdownInput),
                                          HelloImGui::EmToVec2(40.f, 5.f));
                ImGui::PopFont();
                if (ButtonCenteredOnLine("Render"))
                {
                    markdownEdit = false;
                }
            }
            else
            {
                ImGui::PushFont(appState.ChineseFont);
                ImGuiMd::RenderUnindented(MarkdownInput);
                ImGui::PopFont();
                if (ButtonCenteredOnLine("Edit"))
                {
                    markdownEdit = true;
                }
            }

            float width = 0.0f;
            width += HelloImGui::EmSize(4.0f);
            width += style.ItemSpacing.x;
            width += HelloImGui::EmSize(3.0f);
            width += style.ItemSpacing.x;
            width += HelloImGui::EmSize(4.0f);
            AlignForWidth(width);

            if (ImGui::Button("STORE", HelloImGui::EmToVec2(4.0f, 0.0f)))
            {
                if (combo_preview_value.empty())
                {
                    printf("Event name shouldn't be empty!\n");
                    ImGui::OpenPopup("Event shouldn't empty");
                }
                else
                {
                    do_event_log(appState.myAppSettings.start_time,
                                 appState.myAppSettings.stop_time,
                                 combo_preview_value,
                                 absl::CEscape(MarkdownInput));
                    eval_log_line_str((char*)life_controller_core::get_last_append_line().c_str());
                    current_screen = Time_S;
                    first_time = true;
                }
            }
            {
                ImGui::PushFont(appState.TitleFont);
                // Always center this window when appearing
                ImVec2 center = ImGui::GetMainViewport()->GetCenter();
                ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

                if (ImGui::BeginPopupModal(
                        "Event shouldn't empty", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::Text("Please choose one event.");
                    ImGui::Separator();

                    static bool dont_ask_me_next_time = false;
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                    ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
                    ImGui::PopStyleVar();

                    if (ImGui::Button("OK", ImVec2(120, 0)))
                    {
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SetItemDefaultFocus();
                    ImGui::SameLine();
                    if (ImGui::Button("Cancel", ImVec2(120, 0)))
                    {
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
                ImGui::PopFont();
            }
            ImGui::SameLine(0, HelloImGui::EmSize(3.0f));
            if (ImGui::Button("CANCEL", HelloImGui::EmToVec2(4.0f, 0.0f)))
            {
                current_screen = Time_S;
                first_time = true;
            }

            ImGui::PopFont();
        }
        break;
        case Event_List_S:
        {
            static bool first_time = true;
            static std::vector<event_do_log> events = {};
            if (first_time) {
                events = std::move(get_all_events());
                first_time = false;
            }
            DemoCoolBar(current_screen);
            if (current_screen != Event_List_S) {
                first_time = true;
            }
            static ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
                                           ImGuiTableFlags_Hideable | ImGuiTableFlags_BordersOuter |
                                           ImGuiTableFlags_BordersV | ImGuiTableFlags_RowBg;
            static const char* headers[] = {"Event", "start time", "duration", "description"};
            if (ImGui::BeginTable("table1", IM_ARRAYSIZE(headers), flags))
            {
                // Submit columns name with TableSetupColumn() and call TableHeadersRow() to create a row with
                // a header in each column. (Later we will show how TableSetupColumn() has other uses,
                // optional flags, sizing weight etc.)
                for (int column = 0; column < IM_ARRAYSIZE(headers); column++)
                {
                    ImGui::TableSetupColumn(headers[column]);
                }
                ImGui::TableHeadersRow();
                static int selected = -1;
                for (int row = 0; row < events.size(); row++)
                {
                    ImGui::TableNextRow();
                    int column = 0;
                    ImGui::TableSetColumnIndex(column++);
                    ImGuiSelectableFlags selectable_flags = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap;
                    char name[256];
                    snprintf(name, IM_ARRAYSIZE(name), "%s##%d", events[row].event_name, row);
                    if (ImGui::Selectable(name, selected==row, selectable_flags, ImVec2(0,0))) {
                        selected = row;
                    }
                    if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
                    {
                        // TODO: implement a screen SCREEN_STATE stack, jump into event viewer and jump
                        if (ImGui::Button("View")) {

                        }
                        if (ImGui::Button("Delete")) {

                        }
                        if (ImGui::Button("Close"))
                            ImGui::CloseCurrentPopup();
                        ImGui::EndPopup();
                    }
                    ImGui::TableSetColumnIndex(column++);
                    ImGui::Text("%s", events[row].start_time.c_str());
                    ImGui::TableSetColumnIndex(column++);
                    ImGui::Text("%s", events[row].duration.c_str());
                    ImGui::TableSetColumnIndex(column++);
                    ImGui::Text("%s", events[row].event_data);
                }
                ImGui::EndTable();
            }
        }
        break;
        case Event_Edit_S:
        {
            // TODO: implement event edit screen
            static bool first_time = true;
        }
        break;
        default:
            break;
    }
}

void AppPoll() { io_service.poll(); }

#ifdef EMSCRIPTEN
extern "C"
{
    void EMSCRIPTEN_KEEPALIVE clean_stuff()
    {
        HelloImGui::GetRunnerParams()->callbacks.BeforeExit();
        SyncEmscriptenToIndexDB();
    }
}
#endif

#include <fplus/fplus.hpp>

std::string linesUnion(const std::string& input1, const std::string& input2)
{
    using namespace fplus;
    return join(std::string("\n"), nub(append(split_lines(false, input1), split_lines(false, input2))));
}

TEST_CASE("linesUnion")
{
    std::string input1 = R"(a,b,c,d
e,f,g,h
)";
    std::string input2 = R"(a,b,c,d
k,l,m,n
)";
    std::string result = linesUnion(input1, input2);
    CHECK(result == std::string(R"(a,b,c,d
e,f,g,h
k,l,m,n)"));
}

TEST_CASE("file reading")
{
    std::ifstream file(HelloImGui::AssetFileFullPath("hello.txt"));
    CHECK(file.good());
    std::string content{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
    CHECK(content == std::string("file reading test."));
    file.close();
}

#include "app_config.hpp"

int main(int argc, char** argv)
{
    doctest::Context context;

    // !!! THIS IS JUST AN EXAMPLE SHOWING HOW DEFAULTS/OVERRIDES ARE SET !!!

    // defaults
    context.addFilter("test-case-exclude", "*math*");  // exclude test cases with "math" in their name
    context.setOption("abort-after", 5);               // stop test execution after 5 failed assertions
    context.setOption("order-by", "name");             // sort the test cases by their name

    context.applyCommandLine(argc, argv);

    // overrides
    context.setOption("no-breaks", true);  // don't break in the debugger when assertions fail

    int res = context.run();  // run

    if (context.shouldExit())  // important - query flags (and --exit) rely on the user doing this
        return res;            // propagate the result of the tests

    int client_stuff_return_code = 0;

    auto startup_time = std::chrono::steady_clock::now();
    // your program - if the testing framework is integrated in your production code
    printf("hello world!\n");

    set_log_base_dir(LOG_BASE_DIR);

    AppState appState;

    // std::time_t start = std::time(nullptr);
    // std::time_t end = start + 10 * 60 * 60;
    // create_event_log("test", "test event");
    // create_goal_log("test", true, 10, start, end);
    // create_goal_log("test", true, 99999, start, end);
    // create_goal_log("test", false, 9999999, start, end);
    // do_event_log(start, end, "test", "test" STRINGIZE(__COUNTER__));
    CSVReader csv_reader(LOG_BASE_DIR "/" LOGFILE_NAME);
    eval_log_init(csv_reader.csv_reader);
    eval_log();

    HelloImGui::RunnerParams runnerParams;
    runnerParams.appWindowParams.windowTitle = "Life-controller";
    runnerParams.imGuiWindowParams.menuAppTitle = "Docking Demo";
    runnerParams.appWindowParams.windowGeometry.size = {465, 345};
    runnerParams.appWindowParams.restorePreviousGeometry = true;

    runnerParams.appWindowParams.borderless = true;
    runnerParams.appWindowParams.borderlessMovable = true;
    runnerParams.appWindowParams.borderlessResizable = true;
    runnerParams.appWindowParams.borderlessClosable = true;
    runnerParams.appWindowParams.borderlessHideable = true;

    runnerParams.fpsIdling.enableIdling = false;

    // Load additional font
    runnerParams.callbacks.LoadAdditionalFonts = [&appState]() { LoadFonts(appState); };

    //
    // Load user settings at `PostInit` and save them at `BeforeExit`
    //
    runnerParams.callbacks.PostInit = [&appState, &startup_time]
    {
        std::cout << "Program has been running for "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() -
                                                                           startup_time)
                         .count()
                  << " milliseconds" << std::endl;
        LoadMyAppSettings(appState);
#ifndef __EMSCRIPTEN__
        if (tray_init(&tray) < 0)
        {
            printf("failed to create tray\n");
            HelloImGui::GetRunnerParams()->appShallExit = true;
        }
#endif
#ifdef HELLOIMGUI_HAS_IMLOTTIE
        ImLottie::init();
#endif
    };

    runnerParams.callbacks.BeforeImGuiRender = [&appState]()
    {
#ifdef HELLOIMGUI_HAS_IMLOTTIE
        ImLottie::sync();
#endif
    };

    runnerParams.callbacks.BeforeExit = [&appState] { SaveMyAppSettings(appState); };
    runnerParams.callbacks.AfterSwap = AppPoll;

    runnerParams.callbacks.HideWindow = [] { hide_window(); };

    runnerParams.imGuiWindowParams.showStatusBar = true;
    // uncomment next line in order to hide the FPS in the status bar
    runnerParams.imGuiWindowParams.showStatus_Fps = true;
    runnerParams.callbacks.ShowStatus = [&appState]() { StatusBarGui(appState); };

    runnerParams.imGuiWindowParams.showMenuBar = false;

    //
    // Change style
    //
    // 1. Change theme
    auto& tweakedTheme = runnerParams.imGuiWindowParams.tweakedTheme;
    tweakedTheme.Theme = ImGuiTheme::ImGuiTheme_MaterialFlat;
    tweakedTheme.Tweaks.Rounding = 10.f;
    // 2. Customize ImGui style at startup
    runnerParams.callbacks.SetupImGuiStyle = []()
    {
        // Reduce spacing between items ((8, 4) by default)
        ImGui::GetStyle().ItemSpacing = ImVec2(6.f, 4.f);
    };

    runnerParams.callbacks.ShowGui = [&] { guiFunction(appState); };

#if EMSCRIPTEN
    runnerParams.iniFolderType = HelloImGui::IniFolderType::CustomFolder;
    runnerParams.iniFolder = LOG_BASE_DIR;
#else
    runnerParams.iniFolderType = HelloImGui::IniFolderType::AppExecutableFolder;
#endif

    // HelloImGui::DeleteIniSettings(runnerParams);

    // Optional: choose the backend combination
    // ----------------------------------------
    // runnerParams.platformBackendType = HelloImGui::PlatformBackendType::Sdl;
    // runnerParams.rendererBackendType = HelloImGui::RendererBackendType::Vulkan;

    // HelloImGui::Run(runnerParams); // Note: with ImGuiBundle, it is also possible to use ImmApp::Run(...)

    ImmApp::AddOnsParams addOnsParams;
    addOnsParams.withMarkdown = true;
    addOnsParams.withNodeEditor = true;
    addOnsParams.withImplot = true;
    addOnsParams.withTexInspect = true;

    ImGuiMd::MarkdownOptions markdownOptions;
    // markdownOptions.fontOptions.fontBasePath = "fonts/LXGWWenKaiMonoLite";
    markdownOptions.fontOptions.regularSize = 24.0f;
    markdownOptions.fontOptions.glyphRanges =
        HelloImGui::ImWchar2ImWcharPairs(GetGlyphRangesChineseSimplifiedCommon());
    markdownOptions.fontOptions.markdownEmphasisTofontMemory = {
        {ImGuiMd::ImGuiMdFonts::MarkdownEmphasis{.italic = false, .bold = false},
         {get_LXGWWenKaiMonoLite_Regular(), get_LXGWWenKaiMonoLite_RegularItalic_size()}},
        {ImGuiMd::ImGuiMdFonts::MarkdownEmphasis{.italic = true, .bold = false},
         {get_LXGWWenKaiMonoLite_RegularItalic(), get_LXGWWenKaiMonoLite_RegularItalic_size()}},
        {ImGuiMd::ImGuiMdFonts::MarkdownEmphasis{.italic = false, .bold = true},
         {get_LXGWWenKaiMonoLite_Bold(), get_LXGWWenKaiMonoLite_Bold_size()}},
        {ImGuiMd::ImGuiMdFonts::MarkdownEmphasis{.italic = true, .bold = true},
         {get_LXGWWenKaiMonoLite_BoldItalic(), get_LXGWWenKaiMonoLite_BoldItalic_size()}},
    };

    addOnsParams.withMarkdownOptions = markdownOptions;

    runnerParams.imGuiWindowParams.showStatusBar = true;
    ImmApp::Run(runnerParams, addOnsParams);

    return res + client_stuff_return_code;  // the result from doctest is propagated here as well
}
