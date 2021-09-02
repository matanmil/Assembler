cmake_minimum_required(VERSION 3.13)
project(assembler C)

set(CMAKE_C_STANDARD 90)

add_executable(assembler assembler.c Input.c Input.h system.c system.h compile.c compile.h OutputBuild.c OutputBuild.h)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -o -Wall -ansi -pedantic")