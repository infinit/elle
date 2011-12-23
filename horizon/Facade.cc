//
// ---------- header ----------------------------------------------------------
//
// project       facade
//
// license       infinit
//
// author        julien.quintard   [thu dec 22 16:44:01 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <facade/Facade.hh>

namespace facade
{

//
// ---------- static methods --------------------------------------------------
//

  ///
  /// this method initializes the facade.
  ///
  elle::Status		Facade::Initialize()
  {
    enter();

#if defined(INFINIT_UNIX)
    // initialize the UNIX implementation.
    if (unix::UNIX::Initialize() == elle::StatusError)
      escape("unable to initialize the UNIX implementation");

    // set up the UNIX implementation.
    if (unix::UNIX::Setup() == elle::StatusError)
      escape("unable to set up the UNIX implementation");
#elif defined(INFINIT_WIN32)
    // XXX todo: windows
#endif

    leave();
  }

  ///
  /// this method cleans the facade.
  ///
  elle::Status		Facade::Clean()
  {
    enter();

#if defined(INFINIT_UNIX)
    // clean the UNIX implementation.
    if (unix::UNIX::Clean() == elle::StatusError)
      escape("unable to clean the UNIX implementation");
#elif defined(INFINIT_WIN32)
    // XXX todo: windows
#endif

    leave();
  }

}
