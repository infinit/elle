//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/core/Integer.cc
//
// created       julien quintard   [mon sep  5 15:40:43 2011]
// updated       julien quintard   [wed sep  7 10:16:36 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Integer.hh>

//
// ---------- operators -------------------------------------------------------
//

namespace std
{

  ///
  /// this method displays a Integer8 type. without it the stream naturally
  /// displays it as a character.
  ///
  ostream&		operator<<(ostream&			stream,
				   const elle::core::Integer8&	element)
  {
    // cast it into a number.
    stream << static_cast<elle::core::Integer32>(element);

    return (stream);
  }

}
