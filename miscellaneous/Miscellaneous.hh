//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/miscellaneous/Miscellaneous.hh
//
// created       julien quintard   [thu feb  5 15:42:17 2009]
// updated       julien quintard   [thu apr  8 00:05:50 2010]
//

#ifndef ELLE_MISCELLANEOUS_MISCELLANEOUS_HH
#define ELLE_MISCELLANEOUS_MISCELLANEOUS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/miscellaneous/Status.hh>

namespace elle
{
  ///
  /// this namespace contains every general-purpose classes.
  ///
  namespace miscellaneous
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents the miscellaneous namespace.
    ///
    class Miscellaneous
    {
    public:
      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <elle/miscellaneous/Byte.hh>
#include <elle/miscellaneous/Callback.hh>
#include <elle/miscellaneous/Entrance.hh>
#include <elle/miscellaneous/Function.hh>
#include <elle/miscellaneous/Maid.hh>
#include <elle/miscellaneous/Method.hh>
#include <elle/miscellaneous/Region.hh>
#include <elle/miscellaneous/Report.hh>
#include <elle/miscellaneous/Routine.hh>

#endif
