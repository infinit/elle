#ifndef ELLE_SYSTEM_SYSTEM_HH
# define ELLE_SYSTEM_SYSTEM_HH

# include <elle/types.hh>

namespace elle
{

  ///
  /// this namespace contains system-related stuff especially useful
  /// when it comes to portability.
  ///
  namespace system
  {

    ///
    /// this class contains system-wide information.
    ///
    class System
    {
    public:
      //
      // enumerations
      //

      ///
      /// this enumeration represents the endianness.
      ///
      enum Order
        {
          OrderLittle = 0x1,
          OrderBig
        };

      //
      // constants
      //
      static Order                      Endianness;

      struct Path
      {
        static Character                Separator;
        static String                   Home;
        static String                   Root;
        static String                   Current;
      };

      //
      // static methods
      //
      static Status     Initialize();
      static Status     Clean();
    };

  }
}

#endif
