//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [fri aug 26 17:08:22 2011]
//

#ifndef ELLE_STANDALONE_MORGUE_HXX
#define ELLE_STANDALONE_MORGUE_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

#include <elle/concurrency/Callback.hh>

namespace elle
{
  using namespace standalone;
  using namespace concurrency;

  namespace standalone
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method registers an address to delete.
    ///
    /// note that should have the timer not yet been allocated and set up,
    /// this is done in this method.
    ///
    template <typename T>
    Status
    Morgue::Register(T*         instance)
    {
      this->container.push_back(static_cast<Meta*>(instance));
      _corpses_available.signal();
      return StatusOk;
    }

  }
}

#endif
