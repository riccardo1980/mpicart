cmake_minimum_required(VERSION 2.8)

#project(mpicart)

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})
add_subdirectory(src)

if(MPI_COMPILE_FLAGS)
  set_target_properties(hello PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

