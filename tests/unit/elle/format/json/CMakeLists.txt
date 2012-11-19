
set(FORMAT_LIB
    elle
    ${Boost_LIBRARIES})

simple_test(NAME Integer PASS_REGEX "tests done." LIB ${FORMAT_LIB})
simple_test(NAME Bool PASS_REGEX "tests done." LIB ${FORMAT_LIB})
simple_test(NAME Null PASS_REGEX "tests done." LIB ${FORMAT_LIB})
simple_test(NAME Float PASS_REGEX "tests done." LIB ${FORMAT_LIB})
simple_test(NAME Dictionary PASS_REGEX "tests done." LIB ${FORMAT_LIB})
simple_test(NAME Array PASS_REGEX "tests done." LIB ${FORMAT_LIB})
simple_test(NAME String PASS_REGEX "tests done." LIB ${FORMAT_LIB})
