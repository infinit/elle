#ifndef HORIZON_MACOSX_MACOSX_HH
# define HORIZON_MACOSX_MACOSX_HH

# include <elle/types.hh>

namespace horizon
{
  ///
  /// this namespace contains all the elements related to the MacOS X horizon
  /// i.e interface between the kernel and Infinit.
  ///
  namespace macosx
  {

    ///
    /// this class contains the general-purpose information related to the
    /// MacOS X horizon.
    ///
    class MacOSX
    {
    public:
      //
      // static methods
      //
      static elle::Status       Initialize();
      static elle::Status       Clean();
    };

  }
}

#endif
