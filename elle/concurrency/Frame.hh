//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Frame.hh
//
// created       julien quintard   [tue mar 23 14:19:59 2010]
// updated       julien quintard   [tue mar 23 17:17:05 2010]
//

#ifndef ELLE_CONCURRENCY_FRAME_HH
#define ELLE_CONCURRENCY_FRAME_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>
#include <elle/misc/Misc.hh>

namespace elle
{
  using namespace core;
  using namespace misc;

  namespace concurrency
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a frame i.e a system stack.
    ///
    class Frame:
      public Dumpable
    {
    public:
      //
      // constructors & destructors
      //
      Frame();
      ~Frame();

      //
      // methods
      //
      Status		Create(const Natural32);

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Byte*		stack;
      Natural32		size;
    };

  }
}

#endif
