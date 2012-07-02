#ifndef ELLE_RADIX_RADIX_HH
# define ELLE_RADIX_RADIX_HH

# include <elle/types.hh>

namespace elle
{
  ///
  /// the radix namespace contains fundamental classes on which all
  /// the other classes rely such as Object, Entity, Parameters etc.
  ///
  namespace radix
  {

    ///
    /// this class provides methods for managing the radix module.
    ///
    class Radix
    {
    public:
      //
      // static methods
      //
      static Status     Initialize();
      static Status     Clean();
    };

  }
}

#endif
