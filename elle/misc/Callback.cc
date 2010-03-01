//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       misc
//
// file          /home/mycure/infinit/elle/misc/Callback.cc
//
// created       julien quintard   [thu feb  4 22:05:00 2010]
// updated       julien quintard   [sun feb 28 15:10:19 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/misc/Callback.hh>

namespace elle
{
  namespace misc
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default constructor.
    ///
    Callback::Callback(const Callback::Type			type):
      type(type)
    {
    }

  }
}
