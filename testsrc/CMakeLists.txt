
add_executable(2d_halo_scatter 2d_halo_scatter.cpp)
target_link_libraries(2d_halo_scatter LINK_PUBLIC mpicart ${MPI_C_LIBRARIES})

if(MPI_COMPILE_FLAGS)
  set_target_properties(2d_halo_scatter PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
  set_target_properties(2d_halo_scatter PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()

add_executable(mpicart_test mpicart.cpp)
target_link_libraries(mpicart_test LINK_PUBLIC mpicart ${MPI_C_LIBRARIES})

if(MPI_COMPILE_FLAGS)
  set_target_properties(mpicart_test PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
  set_target_properties(mpicart_test PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()

