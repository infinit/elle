#ifndef ELLE_SYSTEM_SYSTEM_HH
# define ELLE_SYSTEM_SYSTEM_HH

# include <elle/types.hh>

# include <elle/system/platform.hh>

namespace elle
{

  /// This namespace contains system-related stuff especially useful
  /// when it comes to portability.
  namespace system
  {

    /*-------------.
    | Enumerations |
    `-------------*/
    enum class Endianness
    {
      little = 0x1,
      big,
      middle,
    };

    /*----------.
    | Constants |
    `----------*/
#if defined(BOOST_BIG_ENDIAN)
    static Endianness const endianness(Endianness::big);
#elif defined(BOOST_LITTLE_ENDIAN)
    static Endianness const endianness(Endianness::little);
#elif defined(BOOST_PDP_ENDIAN)
    static Endianness const endianness(Endianness::middle);
#endif

    namespace path
    {
      static Character const separator('/');
    };

  }
}

#endif
