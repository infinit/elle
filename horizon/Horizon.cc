#include <horizon/Horizon.hh>

#include <elle/system/Platform.hh>

#if defined(INFINIT_LINUX)
# include <horizon/linux/Linux.hh>
#elif defined(INFINIT_MACOSX)
# include <horizon/macosx/MacOSX.hh>
#elif defined(INFINIT_WINDOWS)
# include <horizon/windows/Windows.hh>
#else
# error "unsupported platform"
#endif

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
      if (linux::Linux::Initialize() == elle::Status::Error)
        escape("unable to initialize the Linux implementation");
    }
#elif defined(INFINIT_MACOSX)
    {
      // initialize the MacOS X implementation.
      if (macosx::MacOSX::Initialize() == elle::Status::Error)
        escape("unable to initialize the MacOS X implementation");
    }
#elif defined(INFINIT_WINDOWS)
    {
      // initialize the Windows implementation.
      if (windows::Windows::Initialize() == elle::Status::Error)
        escape("unable to initialize the Windows implementation");
    }
#else
# error "unsupported platform"
#endif

    return elle::Status::Ok;
  }

  ///
  /// this method cleans the horizon.
  ///
  elle::Status          Horizon::Clean()
  {
#if defined(INFINIT_LINUX)
    {
      // clean the Linux implementation.
      if (linux::Linux::Clean() == elle::Status::Error)
        escape("unable to clean the Linux implementation");
    }
#elif defined(INFINIT_MACOSX)
    {
      // clean the MacOS X implementation.
      if (macosx::MacOSX::Clean() == elle::Status::Error)
        escape("unable to clean the MacOS X implementation");
    }
#elif defined(INFINIT_WINDOWS)
    {
      // clean the Windows implementation.
      if (windows::Windows::Clean() == elle::Status::Error)
        escape("unable to clean the Windows implementation");
    }
#else
# error "unsupported platform"
#endif

    return elle::Status::Ok;
  }

}
