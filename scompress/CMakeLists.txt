cmake_minimum_required(VERSION 2.8)


project(scompress)

set(C_STANDARD_REQUIRED 99)
set(CMAKE_C_FLAGS "--std=c99 -Wall -Werror -Wno-error=unused-function ${CMAKE_C_FLAGS}")

add_executable(scompress scompress.c ../src/compressions/alttpcompression.c ../src/mapping_lorom.c ../src/tile.c)

include_directories(../src/)