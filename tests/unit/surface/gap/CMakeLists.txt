simple_test(NAME gap
            PASS_REGEX "tests done."
            LIB gap)

simple_test(NAME state_ctor
            PASS_REGEX "tests done."
            LIB common gap pthread ${Boost_LIBRARIES})

simple_test(NAME Metrics
            PASS_REGEX "Test done."
            LIB common gap pthread ${Boost_LIBRARIES})

simple_test(NAME state_connect
            PASS_REGEX "Test done."
            LIB common gap pthread ${Boost_LIBRARIES})
