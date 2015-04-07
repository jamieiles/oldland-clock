cmake_minimum_required(VERSION 2.6)
project(OldlandClock ASM C)

set(CMAKE_SYSTEM_NAME none)
set(CMAKE_SYSTEM_PROCESSOR oldland)
set(CMAKE_C_COMPILER oldland-elf-gcc)
set(CMAKE_ASM_COMPILER oldland-elf-as)

set(CMAKE_C_FLAGS "-Wall -Werror -O2 -ggdb3")

set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "-mnewlib")

add_executable(oldland-clock
	       clock.c
	       vectors.S)
