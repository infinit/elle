//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/radix/Meta.hh
//
// created       julien quintard   [sun nov 29 19:31:55 2009]
// updated       julien quintard   [mon may  3 20:59:53 2010]
//

#ifndef ELLE_RADIX_META_HH
#define ELLE_RADIX_META_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Void.hh>
#include <elle/core/Natural.hh>
#include <elle/core/Boolean.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Trace.hh>

#include <elle/idiom/Close.hh>
# include <list>
# include <stdlib.h>
# include <execinfo.h>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;

  namespace radix
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents the root of the hierarchy. every class
    /// should directly or indirectly derive this class.
    ///
    /// note that the container stores plain Trace objects, process
    /// which involves copying data. this is required in order to avoid
    /// allocating memory, which would obviously have the impact of
    /// calling back the Meta allocator.
    ///
    class Meta
    {
    public:
      //
      // constants
      //
      static const Boolean		Debug;

      //
      // types
      //
      typedef std::list<Trace*>			Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();

      static Status	Flush();

      static Status	Show(const Natural32 = 0);

      //
      // static attributes
      //
      static Container		Traces;

      //
      // operators
      //
      Void*		operator new(Natural32);
      Void*		operator new(Natural32,
				     Void*);
      Void		operator delete(Void*);
    };

  }
}

#endif
