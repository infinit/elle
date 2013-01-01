.. :Authors: Glyn Matthews <glyn.matthews@gmail.com>
.. :Date: Jan 01, 2013
.. :Description: Source code for a proposed URI class to the C++ standard library.

####################
 C++1y URI Proposal
####################

This project contains the source code that will track the proposal for
a C++ URI at
http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3420.html .

This package provides:

    * A ``network::uri`` class that implements a generic URI parser,
      compatible with `RFC 3986`_ and `RFC 3987`_
    * Accessors to the underlying URI parts
    * An range-compatible interface
    * Methods to normalize and compare URIs
    * Percent encoding and decoding functions
    * A URI builder to build consistent URIs from parts, including
      case, percent encoding and path normalization

Building the project
====================

Configuring ``Boost``
---------------------

Boost can be downloaded from
http://www.boost.org/users/download/. This library requires at least
Boost 1.51.

Configuring ``google-test``
---------------------------

Building with CMake
-------------------

::
	$ mkdir _build
	$ cd _build
	$ cmake ..
	$ make -j4

Running the tests with CTest
----------------------------

::
	$ ctest

Examples
========



License
=======

This library is released under the Boost Software License (please see
http://boost.org/LICENSE_1_0.txt or the accompanying LICENSE_1_0.txt file
for the full text.

Contact
=======

Any questions about this library can be addressed to the cpp-netlib
`developers mailing list`_. Issues can be filed using Github at
http://github.com/cpp-netlib/cpp-netlib/issues.

.. _`developers mailing list`: cpp-netlib@googlegroups.com
