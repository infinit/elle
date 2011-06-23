//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/core/Null.cc
//
// created       julien quintard   [wed feb 18 15:40:40 2009]
// updated       julien quintard   [thu jun 23 11:49:50 2011]
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
// ---------- globals ---------------------------------------------------------
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
  ostream&		operator<<(ostream&		stream,
				   const elle::core::Null&)
  {
    stream << "(nil)";

    return (stream);
  }

}
