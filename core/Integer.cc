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
// updated       julien quintard   [mon sep  5 15:40:50 2011]
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
  /// this method displays a Integer8 type.
  ///
  ostream&		operator<<(ostream&			stream,
				   const elle::core::Integer8&	element)
  {
    // cast it into a number.
    stream << (elle::core::Integer32)element;

    return (stream);
  }

}
