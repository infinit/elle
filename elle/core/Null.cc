//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/core/Null.cc
//
// created       julien quintard   [wed feb 18 15:40:40 2009]
// updated       julien quintard   [sat aug  8 21:44:30 2009]
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
				   const elle::core::Null&)
  {
    stream << "(nil)";

    return (stream);
  }
}
