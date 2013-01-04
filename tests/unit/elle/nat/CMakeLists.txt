if (NOT WIN32)

set(NAT_LIB
    elle
    pthread
    asio-udt
    ${Boost_LIBRARIES})

endif()

simple_test(NAME nat_hole_ctor  PASS_REGEX "tests done." LIB ${NAT_LIB})
simple_test(NAME nat_hole_punch PASS_REGEX "tests done." LIB ${NAT_LIB})
