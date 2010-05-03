//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/radix/Trace.hh
//
// created       julien quintard   [mon apr 26 21:04:49 2010]
// updated       julien quintard   [mon may  3 21:00:04 2010]
//

#ifndef ELLE_RADIX_TRACE_HH
#define ELLE_RADIX_TRACE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Character.hh>
#include <elle/core/Void.hh>
#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>

#include <elle/io/Dumpable.hh>

#include <elle/idiom/Close.hh>
# include <list>
# include <cxxabi.h>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace io;

  namespace radix
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a trace i.e a sequence of function calls
    /// performed within a stack.
    ///
    class Trace:
      public Dumpable
    {
    public:
      //
      // constants
      //
      static const Natural32		Size = 30;

      //
      // constructors & destructors
      //
      Trace(const Void*);

      //
      // methods
      //
      Void		Generate();

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      const Void*	address;

      Void*		frames[Size];
      Natural32		size;
    };

  }
}

#endif
