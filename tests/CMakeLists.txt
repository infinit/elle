
enable_testing(true)

macro(simple_test test_name args regex srcs)
    list(APPEND ${test_name}_srcs "${srcs};${test_name}.cc")
    add_executable(test_${test_name} ${${test_name}_srcs})
    add_test(${test_name} test_${test_name} ${args})
    set_tests_properties(${test_name} PROPERTIES PASS_REGULAR_EXPRESSION ${regex})
    list(APPEND TEST_EXECUTABLES test_${test_name})
endmacro(simple_test)

###############################################################################
# testing whether or testing system works
simple_test(testing_tests "" "Tests are working.\n" "")


###############################################################################
add_custom_target(test COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS ${TEST_EXECUTABLES})

add_subdirectory(elle)
