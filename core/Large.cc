//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/core/Large.cc
//
// created       julien quintard   [thu feb  5 15:37:07 2009]
// updated       julien quintard   [fri mar  5 11:37:54 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Large.hh>

//
// ---------- operators -------------------------------------------------------
//

namespace std
{

  ///
  /// this method displays a Large number in its hexadecimal form.
  ///
  std::ostream&		operator<<(std::ostream&		stream,
				   const elle::core::Large&	element)
  {
    char*		hexadecimal;

    // transform the number into hexadecimal.
    hexadecimal = ::BN_bn2hex(&element);

    // put an hexadecimal representation of the big number into the stream.
    stream << hexadecimal;

    // free the hexadecimal.
    ::OPENSSL_free(hexadecimal);

    return (stream);
  }
}
