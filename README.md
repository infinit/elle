JSON Spirit
===========

JSON Spirit is a C++ JSON library using Boost Spirit for parsing.

This version is modified for use with Sirikata.

Compiling & Using
-----------------

To build and install the library independently of another project, use
the normal cmake procedure:

    cd json-spirit/build
    # Run CMake and check output for errors.
    cmake . [-DCMAKE_INSTALL_PREFIX=/path/to/install]
    make
    # Run some tests
    ./json_test
    # Optionally install
    make install

To use JSON Spirit within another CMake-based project, you can add a
command like the following to your CMakeLists.txt:

    ADD_SUBDIRECTORY(/path/to/json-spirit/build ${CMAKE_CURRENT_BINARY_DIR}/json-spirit)

This adds the json-spirit build as a subdirectory of your project. You
can use the json-spirit targets (e.g. json_spirit, json_test) in your
own CMake commands.

There are a couple of CMake settings which control which components
are built:

    JSON_SPIRIT_LIBRARY_TYPE - set to STATIC or SHARED
    JSON_SPIRIT_BUILD_DEMOS - set to FALSE to disable demos
    JSON_SPIRIT_BUILD_TESTS - set to FALSE to disable tests

Documentation
-------------
Hosted documentation:
http://sirikata.github.com/json-spirit/

You can generate documentation from the source code using doxygen:

    doxygen doc/Doxyfile

The output will be placed in doc/[type], e.g. doc/html if you
generated HTML documentation (the default).

References
----------

The original code for JSON Spirit can be found at:
http://www.codeproject.com/Articles/20027/JSON-Spirit-A-C-JSON-Parser-Generator-Implemented
