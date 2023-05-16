cmake_minimum_required(VERSION 3.20)
project(SICSA C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fexec-charset=GBK")
add_executable(SICSA main.c test/testConsole.c test/testConsole.h)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fexec-charset=GBK")
