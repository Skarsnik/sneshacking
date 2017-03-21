cmake_minimum_required(VERSION 2.8)


project(test)

set(C_STANDARD_REQUIRED 99)
set(CMAKE_C_FLAGS "--std=c99 -Wall -Werror ${CMAKE_C_FLAGS}")

add_executable(testdecompress CuTest.c testdecompress.c ../decompress.c)
add_executable(testtile CuTest.c testtile.c ../tile.c)
add_executable(testrommaping CuTest.c testrommapping.c ../mapping_lorom.c ../rommapping.c ../mapping_hirom.c)

include_directories(../)

#install(TARGETS test RUNTIME DESTINATION bin)
