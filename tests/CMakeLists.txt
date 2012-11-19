enable_testing(true)

include(CMakeParseArguments)
function(simple_test)
        set(options)
        set(oneValueArgs NAME PASS_REGEX)
        set(multipleValueArgs SOURCE ARGS LIB)
        cmake_parse_arguments(S_TEST
                "${options}"
                "${oneValueArgs}"
                "${multipleValueArgs}"
                "${ARGN}")

        if (NOT S_TEST_NAME)
                set(S_TEST_NAME "${ARGV0}")
        endif()
        set(_TARGET "test_${S_TEST_NAME}")
        list(APPEND ${S_TEST_NAME}_srcs
                "${S_TEST_SOURCE};${S_TEST_NAME}.cc")
        add_executable(${_TARGET} ${${S_TEST_NAME}_srcs})
        add_test(${S_TEST_NAME}
                ${_TARGET}
                ${S_TEST_ARGS})
        if (S_TEST_LIB)
                message(STATUS "${_TARGET}: linking with ${S_TEST_LIB}")
                target_link_libraries(${_TARGET} ${S_TEST_LIB})
        endif()
        if (S_TEST_PASS_REGEX)
                message(STATUS
                        "${_TARGET}: passing regex set to \"${S_TEST_PASS_REGEX}\"")
                set_tests_properties(${S_TEST_NAME}
                        PROPERTIES PASS_REGULAR_EXPRESSION
                        "${S_TEST_PASS_REGEX}")
        endif()
        list(APPEND TEST_EXECUTABLES ${_TARGET})
endfunction(simple_test)

###############################################################################
# testing whether or testing system works
simple_test(testing_tests "" "Tests are working.\n" "")

###############################################################################
add_custom_target(test COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS ${TEST_EXECUTABLES})

add_subdirectory(unit)
