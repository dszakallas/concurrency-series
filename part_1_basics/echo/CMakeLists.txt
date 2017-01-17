cmake_minimum_required(VERSION 2.8)
project(echo)

set(CMAKE_C_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}")

add_executable(single single.c read_write.h)

add_executable(multi multi.c read_write.h)
find_package (Threads REQUIRED)
target_link_libraries (multi ${CMAKE_THREAD_LIBS_INIT})

add_executable(bubble_sort bubble_sort.c)
add_executable(timeslice timeslice.c)

find_package (Libuv REQUIRED)
add_executable(single-uv single-uv.c)

target_include_directories(single-uv PRIVATE ${Libuv_INCLUDE_DIRS})
target_link_libraries (single-uv PRIVATE ${Libuv_LIBRARIES})