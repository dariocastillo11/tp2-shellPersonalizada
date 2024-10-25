# Verfy cmake version
cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)



set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fprofile-arcs -ftest-coverage --coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgcov --coverage")
