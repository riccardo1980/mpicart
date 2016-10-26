

foreach( test_name 2d_halo_scatter mpicart )
  add_executable( ${test_name}_test ${test_name}.cpp)
  target_link_libraries( ${test_name}_test LINK_PUBLIC mpicart 
    ${MPI_C_LIBRARIES})
  
  if(MPI_COMPILE_FLAGS)
    set_target_properties( ${test_name}_test PROPERTIES
      COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
  endif()

  if(MPI_LINK_FLAGS)
    set_target_properties(${test_name}_test PROPERTIES
      LINK_FLAGS "${MPI_LINK_FLAGS}")
  endif()

endforeach( test_name )

