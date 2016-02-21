.. :Authors: Glyn Matthews <glyn.matthews@gmail.com>
.. :Date: Jan 01, 2013
.. :Description: Source code for the cpp-netlib URI class.

################
 C++ Network URI
################

.. image:: https://travis-ci.org/cpp-netlib/uri.png?branch=master
  :target: https://travis-ci.org/cpp-netlib/uri

This project contains the source code that was originally meant to
track the proposal for a C++ URI at
http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3975.html

This package provides:

    * A ``network::uri`` class that implements a generic URI parser,
      compatible with `RFC 3986`_ and `RFC 3987`_
    * Accessors to the underlying URI parts
    * A range-compatible interface
    * Methods to normalize and compare URIs
    * Percent encoding and decoding functions
    * A URI builder to build consistent URIs from parts, including
      case, percent encoding and path normalization

.. _`RFC 3986`: http://tools.ietf.org/html/rfc3986
.. _`RFC 3987`: http://tools.ietf.org/html/rfc3987

Building the project
====================

Building with ``CMake``
-----------------------

::

	$ mkdir _build
	$ cd _build
	$ cmake ..
	$ make -j4

Running the tests with ``CTest``
--------------------------------

::

	$ ctest

License
=======

This library is released under the Boost Software License (please see
http://boost.org/LICENSE_1_0.txt or the accompanying LICENSE_1_0.txt
file for the full text.

Contact
=======

Any questions about this library can be addressed to the cpp-netlib
`developers mailing list`_. Issues can be filed using Github at
http://github.com/cpp-netlib/uri/issues.

.. _`developers mailing list`: cpp-netlib@googlegroups.com
