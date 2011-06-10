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
// updated       julien quintard   [fri jun  3 15:27:10 2011]
//

#ifndef ELLE_CONCURRENCY_FRAME_HH
#define ELLE_CONCURRENCY_FRAME_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>
#include <elle/core/Byte.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Meta.hh>
#include <elle/radix/Entity.hh>

namespace elle
{
  using namespace core;
  using namespace radix;

  namespace concurrency
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a frame i.e a system stack.
    ///
    class Frame:
      public Entity
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
