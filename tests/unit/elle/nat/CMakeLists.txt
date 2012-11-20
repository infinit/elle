set(NAT_LIB
    elle
    ${Boost_LIBRARIES})

simple_test(NAME nat_hole_ctor  PASS_REGEX "tests done." LIB ${NAT_LIB})
simple_test(NAME nat_hole_punch PASS_REGEX "tests done." LIB ${NAT_LIB})
