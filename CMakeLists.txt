cmake_minimum_required(VERSION 2.8)

project(mpicart)

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})
add_subdirectory(src)
add_subdirectory(testsrc)


