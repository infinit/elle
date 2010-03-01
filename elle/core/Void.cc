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
// updated       julien quintard   [sun feb 28 10:51:02 2010]
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
    Void*			vacuum = NULL;

  }
}
