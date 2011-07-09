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
// updated       julien quintard   [sat jul  9 19:21:41 2011]
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
    /// note that this class exports fileable-like method but cannot
    /// implement the interface as this would generate too much
    /// inter-dependencies.
    ///
    class Trace:
      public virtual Dumpable
    {
    public:
      //
      // constants
      //
      static const Natural32		Capacity = 30;

      static const String		Location;

      //
      // constructors & destructors
      //
      Trace(Void*);

      //
      // methods
      //
      Status		Generate();

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Void*		address;

      Void*		frames[Capacity];
      Natural32		size;

      //
      // static methods
      //
      static Status	Store(Void*);
      static Status	Erase(Void*);

      static Status	Initialize();
      static Status	Clean();

      static Status	Show(const Natural32 = 0);
    };

  }
}

#endif
