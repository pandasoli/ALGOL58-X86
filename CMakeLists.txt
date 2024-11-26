cmake_minimum_required(VERSION 3.28.1)

project(A58Lang VERSION 1.0.0)
set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb -ansi")
set(CMAKE_CTEST_ARGUMENTS "--output-on-failure")

add_executable(
	a58

	src/lexer/lexer.c
	src/lexer/token.c

	src/location.c
	src/str.c
	src/utils.c

	src/main.c)

target_include_directories(
	a58
	PUBLIC include)
