//
// ---------- header ----------------------------------------------------------
//
// project       horizon
//
// license       infinit
//
// author        julien quintard   [thu dec 22 16:44:01 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <horizon/Horizon.hh>

namespace horizon
{

//
// ---------- static methods --------------------------------------------------
//

  ///
  /// this method initializes the horizon.
  ///
  elle::Status          Horizon::Initialize()
  {
#if defined(INFINIT_LINUX)
    {
      // initialize the Linux implementation.
      if (linux::Linux::Initialize() == elle::StatusError)
        escape("unable to initialize the Linux implementation");
    }
#elif defined(INFINIT_MACOSX)
    {
      // initialize the MacOS X implementation.
      if (macosx::MacOSX::Initialize() == elle::StatusError)
        escape("unable to initialize the MacOS X implementation");
    }
#elif defined(INFINIT_WINDOWS)
    {
      // initialize the Windows implementation.
      if (windows::Windows::Initialize() == elle::StatusError)
        escape("unable to initialize the Windows implementation");
    }
#else
# error "unsupported platform"
#endif

    return elle::StatusOk;
  }

  ///
  /// this method cleans the horizon.
  ///
  elle::Status          Horizon::Clean()
  {
#if defined(INFINIT_LINUX)
    {
      // clean the Linux implementation.
      if (linux::Linux::Clean() == elle::StatusError)
        escape("unable to clean the Linux implementation");
    }
#elif defined(INFINIT_MACOSX)
    {
      // clean the MacOS X implementation.
      if (macosx::MacOSX::Clean() == elle::StatusError)
        escape("unable to clean the MacOS X implementation");
    }
#elif defined(INFINIT_WINDOWS)
    {
      // clean the Windows implementation.
      if (windows::Windows::Clean() == elle::StatusError)
        escape("unable to clean the Windows implementation");
    }
#else
# error "unsupported platform"
#endif
    return elle::StatusOk;
  }

}
