//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/standalone/Standalone.hh
//
// created       julien quintard   [sun may  2 10:57:51 2010]
// updated       julien quintard   [mon may  3 20:57:34 2010]
//

#ifndef ELLE_STANDALONE_HH
#define ELLE_STANDALONE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/radix/Status.hh>

namespace elle
{
  using namespace radix;

  ///
  /// this namespace contains classes that must depend on nothing else
  /// as the whole Elle library relies on them.
  ///
  namespace standalone
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class manages the standalone module.
    ///
    class Standalone
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

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Region.hh>
#include <elle/standalone/Report.hh>

#endif
