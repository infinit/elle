//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [wed feb 18 15:40:40 2009]
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
    /// this global variable is useful for avoiding the caller to declare a
    /// variable of this type.
    ///
    Null                        none = Nil;

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
  ostream&              operator<<(ostream&             stream,
                                   const elle::core::Null&)
  {
    stream << "(nil)";

    return (stream);
  }

}
