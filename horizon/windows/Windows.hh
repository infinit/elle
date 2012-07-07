#ifndef HORIZON_WINDOWS_WINDOWS_HH
# define HORIZON_WINDOWS_WINDOWS_HH

# include <elle/types.hh>

namespace horizon
{
  ///
  /// this namespace contains all the elements related to the windows horizon
  /// i.e interface between the kernel and Infinit.
  ///
  namespace windows
  {

    ///
    /// this class contains the general-purpose information related to the
    /// windows horizon.
    ///
    class Windows
    {
    public:
      //
      // static methods
      //
      static elle::Status       Initialize();
      static elle::Status       Setup();
      static elle::Status       Clean();

      //
      // static attributes
      //
      // XXX
    };

  }
}

#endif
