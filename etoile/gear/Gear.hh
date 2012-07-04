#ifndef ETOILE_GEAR_GEAR_HH
# define ETOILE_GEAR_GEAR_HH

# include <elle/types.hh>

namespace etoile
{
  ///
  /// this namespace provides contexts for operating on the file system
  /// by applying automata in order to maintain consistency.
  ///
  namespace gear
  {

    ///
    /// this class represents the gear module.
    ///
    class Gear
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
