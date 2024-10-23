cmake_minimum_required(VERSION 3.29)
project(tetris C)

set(CMAKE_C_STANDARD 11)

FILE(GLOB MyCSources *.c)
add_executable(tetris ${MyCSources}
        shape.c
        shape.h
        array.c
        helpers.h)

target_link_libraries(tetris -lOpenGL32 -lfreeGLUT -static -static-libgcc)
