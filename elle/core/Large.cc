//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       GPL
//
// file          /home/mycure/infinit/elle/core/Large.cc
//
// created       julien quintard   [thu feb  5 15:37:07 2009]
// updated       julien quintard   [wed feb 18 15:59:37 2009]
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
    // put an hexadecimal representation of the big number into the stream.
    stream << BN_bn2hex(&element);

    return (stream);
  }
}
