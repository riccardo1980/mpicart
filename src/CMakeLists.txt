
include_directories(${libmpicart_SOURCE_DIR}/src)

file(GLOB SRCS "*.hpp" "*.cpp" ) 
add_library(mpicart ${SRCS})

if(MPI_COMPILE_FLAGS)
  set_target_properties(hello PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

target_include_directories(mpicart PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
