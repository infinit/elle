project(group CXX)

add_executable(8group
  Group.cc
)

target_link_libraries(
    8group
    infinit lune etoile hole
    ${Boost_LIBRARIES}
)
