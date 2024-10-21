cmake_minimum_required(VERSION 3.28)
project(tool_try)

set(CMAKE_CXX_STANDARD 23)
set(SYS_INCLUDE_DIR "/usr/include")
set(SYS_X64_INCLUDE_DIR "${SYS_INCLUDE_DIR}/x86_64-linux-gnu")


add_compile_definitions(__STDC_CONSTANT_MACROS)

find_package(PkgConfig REQUIRED)
find_package(SDL2 REQUIRED)

include_directories(
    "include"
    ${SYS_X64_INCLUDE_DIR}
    ${SDL2_INCLUDE_DIRS}
)

# 设置源代码目录
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

# 递归查找所有的 .cpp 文件
file(GLOB_RECURSE ALL_SOURCE_FILES "${SOURCE_DIR}/*.cpp")

# 遍历所有找到的文件，排除包含 "test" 目录的文件
foreach(FILE_PATH ${ALL_SOURCE_FILES})
    if (FILE_PATH MATCHES "/test/")
        list(REMOVE_ITEM ALL_SOURCE_FILES ${FILE_PATH})
    endif()
endforeach()

# 打印排除后的源文件列表（可选，用于调试）
message("Source files: ${ALL_SOURCE_FILES}")

add_executable(tool_try
     main.cpp
#    src/player/sdl_video_player.cpp
#    include/player/sdl_video_player.h
#    src/player/video_player.cpp
#    include/player/video_player.h
#    src/struct/packet_queue.cpp
#    include/struct/packet_queue.h
#    src/entity/packet.cpp
#    include/entity/packet.h
#    src/test/test_lib_queue.cpp
    ${ALL_SOURCE_FILES}
        include/entity/audio_simple_format.h
        include/util/mem/mem_alloc.h
)

pkg_check_modules(AVCODEC REQUIRED IMPORTED_TARGET libavcodec)
pkg_check_modules(AVFORMAT REQUIRED IMPORTED_TARGET libavformat)
pkg_check_modules(AVUTIL REQUIRED IMPORTED_TARGET libavutil)
pkg_check_modules(AVFILTER REQUIRED IMPORTED_TARGET libavfilter)
pkg_check_modules(SWRESAMPLE REQUIRED IMPORTED_TARGET libswresample)
pkg_check_modules(SWSCALE REQUIRED IMPORTED_TARGET libswscale)
pkg_check_modules(AVDEVICE REQUIRED IMPORTED_TARGET libavdevice)

target_link_libraries(tool_try
    m
    PkgConfig::AVFILTER
    PkgConfig::AVFORMAT
    PkgConfig::AVCODEC
    PkgConfig::AVUTIL
    PkgConfig::SWRESAMPLE
    PkgConfig::SWSCALE
    PkgConfig::AVDEVICE
    #    ${MY_LIB_DIR}/libavfilter.so
    #    ${MY_LIB_DIR}/libavformat.so
    #    ${MY_LIB_DIR}/libavcodec.so
    #    ${MY_LIB_DIR}/libavutil.so
    #    ${MY_LIB_DIR}/libswresample.so
    #    ${MY_LIB_DIR}/libswscale.so
    #    ${MY_LIB_DIR}/libavdevice.so
    ${SDL2_LIBRARIES}
)