
file(GLOB common_srcs *.cc)
add_library(common ${common_srcs})
target_link_libraries(common elle ${Boost_LIBRARIES})

