//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       GPL
//
// file          /home/mycure/infinit/elle/core/Null.cc
//
// created       julien quintard   [wed feb 18 15:40:40 2009]
// updated       julien quintard   [mon mar  9 12:30:58 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Null.hh>

namespace elle
{
  namespace core
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this global variable is useful for avoiding caller to declare a
    /// variable of this type.
    ///
    Null			none = Nil;

  }
}

//
// ---------- operators -------------------------------------------------------
//

namespace std
{

  ///
  /// this method displays a Null type i.e the Nil value.
  ///
  std::ostream&		operator<<(std::ostream&		stream,
				   const elle::core::Null&	element)
  {
    stream << "(nil)";

    return (stream);
  }
}
