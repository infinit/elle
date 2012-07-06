#ifndef NUCLEUS_PROTON_PROTON_HH
# define NUCLEUS_PROTON_PROTON_HH

# include <elle/types.hh>

namespace nucleus
{
  ///
  /// this namespace contains the physical elements of the file system
  /// logic.
  ///
  namespace proton
  {

    ///
    /// this class controls the proton classes.
    ///
    class Proton
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
