set(EC_DEPENDENCIES
        cryptography
        elle
        comet
        ${INFINIT_OPENSSL_SHARED_LIBRARIES}
        ${Boost_LIBRARIES})

#simple_test(ec_PrivateKey "" "tests done." "")
simple_test(NAME ec_PrivateKey
            PASS_REGEX "tests done."
            LIB ${EC_DEPENDENCIES})

simple_test(NAME ec_SecretKey
            PASS_REGEX "tests done."
            LIB ${EC_DEPENDENCIES})

simple_test(NAME ec_oneway
            PASS_REGEX "tests done."
            LIB ${EC_DEPENDENCIES})

simple_test(NAME ec_all
            PASS_REGEX "tests done."
            LIB ${EC_DEPENDENCIES})
