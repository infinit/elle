//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/cryptography/Random.hh
//
// created       julien quintard   [thu aug 11 16:08:47 2011]
// updated       julien quintard   [sat sep  3 22:14:01 2011]
//

#ifndef ELLE_CRYPTOGRAPHY_RANDOM_HH
#define ELLE_CRYPTOGRAPHY_RANDOM_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Boolean.hh>
#include <elle/core/Character.hh>
#include <elle/core/Real.hh>
#include <elle/core/Integer.hh>
#include <elle/core/Natural.hh>
#include <elle/core/Large.hh>
#include <elle/core/String.hh>

#include <elle/standalone/Region.hh>

#include <elle/radix/Status.hh>

namespace elle
{
  using namespace core;
  using namespace standalone;
  using namespace radix;

  namespace cryptography
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides functionalities for generating random values.
    ///
    /// \todo XXX note that the current implementation is Linux-specific
    ///           since it reads directly from '/dev/random'.
    class Random
    {
    public:
      //
      // constants
      //
      static const Natural32		Bits = 1024;
      static const Natural32		Length = 128;
      static const Natural32		Size = 512;

      //
      // static method
      //
      static Status	Initialize();
      static Status	Clean();

      static Status	Generate(Boolean&);
      static Status	Generate(Character&);
      static Status	Generate(Real&);
      static Status	Generate(Integer8&);
      static Status	Generate(Integer16&);
      static Status	Generate(Integer32&);
      static Status	Generate(Integer64&);
      static Status	Generate(Natural8&);
      static Status	Generate(Natural16&);
      static Status	Generate(Natural32&);
      static Status	Generate(Natural64&);
      static Status	Generate(Large&,
				 const Natural32 = Bits);
      static Status	Generate(String&,
				 const Natural32 = Length);
      static Status	Generate(Region&,
				 const Natural32 = Size);
    };

  }
}

#endif
