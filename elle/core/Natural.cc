//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/core/Natural.cc
//
// created       julien quintard   [wed feb 18 15:40:40 2009]
// updated       julien quintard   [wed sep  7 10:16:31 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

//
// ---------- operators -------------------------------------------------------
//

namespace std
{

  ///
  /// this method displays a Natural8 type. without it the stream naturally
  /// displays it as a character.
  ///
  ostream&		operator<<(ostream&			stream,
				   const elle::core::Natural8&	element)
  {
    // cast it into a number.
    stream << static_cast<elle::core::Natural32>(element);

    return (stream);
  }

}
