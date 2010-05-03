//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/system/System.hh
//
// created       julien quintard   [mon jan 26 20:21:10 2009]
// updated       julien quintard   [mon may  3 21:07:53 2010]
//

#ifndef ELLE_SYSTEM_SYSTEM_HH
#define ELLE_SYSTEM_SYSTEM_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/String.hh>
#include <elle/core/Character.hh>

#include <elle/radix/Status.hh>

namespace elle
{
  using namespace core;
  using namespace radix;

  ///
  /// this namespace contains system-related stuff.
  ///
  namespace system
  {

//
// ---------- classes ---------------------------------------------------------
//

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
      static Order			Endianness;

      struct Path
      {
	static Character		Separator;
	static String			Home;
	static String			Root;
	static String			Current;
      };

      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();
    };

  }
}

#endif
