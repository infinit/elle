//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/libraries/elle/core/Trace.hh
//
// created       julien quintard   [mon apr 26 21:04:49 2010]
// updated       julien quintard   [tue apr 27 11:38:29 2010]
//

#ifndef ELLE_CORE_TRACE_HH
#define ELLE_CORE_TRACE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Character.hh>
#include <elle/core/Void.hh>
#include <elle/core/Natural.hh>
#include <elle/core/Integer.hh>

#include <elle/miscellaneous/Status.hh>

#include <elle/io/Dumpable.hh>

#include <elle/idiom/Close.hh>
# include <list>
# include <cxxabi.h>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace miscellaneous;
  using namespace io;

  namespace core
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
      static const Natural32		Size;

      //
      // types
      //
      typedef std::list<String>			Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

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

      Container		symbols;
    };

  }
}

#endif
