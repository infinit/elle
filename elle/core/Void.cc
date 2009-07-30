//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/core/Void.cc
//
// created       julien quintard   [wed feb 18 15:40:40 2009]
// updated       julien quintard   [thu jul 30 13:28:52 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Void.hh>

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
    Void			vacuum;

  }
}
