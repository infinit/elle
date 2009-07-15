//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       GPL
//
// file          /home/mycure/infinit/elle/system/System.cc
//
// created       julien quintard   [mon jan 26 20:23:52 2009]
// updated       julien quintard   [fri feb  6 21:01:24 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/system/System.hh>

namespace elle
{
  namespace system
  {

//
// ---------- definitions -----------------------------------------------------
//

#if defined(WORDS_BIGENDIAN)
    const System::Endianness	System::MachineEndianness =
      System::EndiannessBig;
#else
    const System::Endianness	System::MachineEndianness =
      System::EndiannessLittle;
#endif

  }
}
