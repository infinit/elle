//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/libraries/elle/core/Meta.hh
//
// created       julien quintard   [sun nov 29 19:31:55 2009]
// updated       julien quintard   [tue apr 27 17:02:51 2010]
//

#ifndef ELLE_CORE_META_HH
#define ELLE_CORE_META_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Void.hh>
#include <elle/core/Natural.hh>
#include <elle/core/Boolean.hh>
#include <elle/core/Trace.hh>

#include <elle/miscellaneous/Status.hh>

#include <elle/idiom/Close.hh>
# include <list>
# include <stdlib.h>
# include <execinfo.h>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace miscellaneous;

  namespace core
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
      static Container	Traces;

      //
      // operators
      //
      Void*			operator new(Natural32);
      Void*			operator new(Natural32,
					     Void*);
      Void			operator delete(Void*);
    };

  }
}

#endif
