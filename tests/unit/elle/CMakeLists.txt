set(ELLE_LIB
    elle
    ${Boost_LIBRARIES}
    )

simple_test(NAME print PASS_REGEX "tests done." LIB ${ELLE_LIB})
simple_test(NAME operator PASS_REGEX "tests done." LIB ${ELLE_LIB})
simple_test(NAME log PASS_REGEX "tests done." LIB ${ELLE_LIB})
simple_test(NAME finally PASS_REGEX "tests done.\nlambda" LIB ${ELLE_LIB})

add_subdirectory(nat)
add_subdirectory(cryptography)
add_subdirectory(format)
add_subdirectory(network)
add_subdirectory(io)
add_subdirectory(serialize)
add_subdirectory(system)
add_subdirectory(utility)
