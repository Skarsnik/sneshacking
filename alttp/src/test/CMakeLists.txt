cmake_minimum_required(VERSION 2.8)

#set(C_STANDARD_REQUIRED 99)
set(CMAKE_C_FLAGS "--std=c99 -Wall -Werror ${CMAKE_C_FLAGS}")

project(test)

add_executable(testdecompress CuTest.c testdecompress.c ../decompress.c)
add_executable(testtile CuTest.c testtile.c ../tile.c)
include_directories(../)

#install(TARGETS test RUNTIME DESTINATION bin)
