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
// updated       julien quintard   [fri apr 30 17:12:13 2010]
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
  /// this method displays a Natural8 type.
  ///
  std::ostream&		operator<<(std::ostream&		stream,
				   const elle::core::Natural8&	element)
  {
    // cast it into a number.
    stream << (elle::core::Natural32)element;

    return (stream);
  }

}
