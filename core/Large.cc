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
// updated       julien quintard   [thu jun 23 11:49:23 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Large.hh>
#include <elle/core/Natural.hh>
#include <elle/core/String.hh>

#include <elle/idiom/Close.hh>
# include <string.h>
#include <elle/idiom/Open.hh>

//
// ---------- operators -------------------------------------------------------
//

namespace std
{

  ///
  /// this method displays a Large number in its hexadecimal form.
  ///
  ostream&		operator<<(ostream&		stream,
				   const elle::core::Large&	element)
  {
    static const elle::core::Natural32	Length = 50;
    char*				hexadecimal;

    // transform the number into hexadecimal.
    hexadecimal = ::BN_bn2hex(&element);

    // display the string, depending on its length.
    if (::strlen(hexadecimal) < Length)
      {
	// if the string is short enough, display it in its entirety.
	stream << hexadecimal;
      }
    else
      {
	elle::core::String		string(hexadecimal);

	// otherwise chop it and display the begining and the end only.
	stream << string.substr(0, Length / 2) << "..."
	       << string.substr(string.length() - (Length / 2));
      }

    // free the hexadecimal.
    ::OPENSSL_free(hexadecimal);

    return (stream);
  }

}
