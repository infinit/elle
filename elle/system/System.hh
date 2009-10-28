//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/system/System.hh
//
// created       julien quintard   [mon jan 26 20:21:10 2009]
// updated       julien quintard   [thu oct 15 20:07:21 2009]
//

#ifndef ELLE_SYSTEM_SYSTEM_HH
#define ELLE_SYSTEM_SYSTEM_HH

namespace elle
{
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
      enum Endianness
	{
	  EndiannessLittle = 0x1,
	  EndiannessBig
	};

      //
      // constants
      //

      ///
      /// this variable represents the endianness of the current machine.
      ///
      /// autoconf defines the WORDS_BIGENDIAN macro for us so that we know
      /// what is the current endianness.
      ///
      /// \todo XXX to fix cause not using autoconf any more
      ///
      static const Endianness		MachineEndianness;
    };

  }
}

#endif
