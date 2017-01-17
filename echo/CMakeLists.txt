cmake_minimum_required(VERSION 3.3)
project(echo)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")

add_executable(single single.c)
add_executable(multi multi.c read_write.h)