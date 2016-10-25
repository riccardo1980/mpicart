
include_directories(${libmpicart_SOURCE_DIR}/src)

file(GLOB SRCS "*.hpp" "*.cpp" ) 
add_library(mpicart ${SRCS})
