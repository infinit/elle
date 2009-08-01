//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/system/System.cc
//
// created       julien quintard   [mon jan 26 20:23:52 2009]
// updated       julien quintard   [sat aug  1 15:02:59 2009]
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

/// \todo
/// XXX this is no longer correct since we are not using autoconf anymore
///

#if defined(WORDS_BIGENDIAN)
    const System::Endianness	System::MachineEndianness =
      System::EndiannessBig;
#else
    const System::Endianness	System::MachineEndianness =
      System::EndiannessLittle;
#endif

  }
}
