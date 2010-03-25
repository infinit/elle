//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       misc
//
// file          /home/mycure/infinit/elle/misc/Function.hh
//
// created       julien quintard   [thu feb  4 22:15:45 2010]
// updated       julien quintard   [thu mar 25 00:39:11 2010]
//

#ifndef ELLE_MISC_FUNCTION_HH
#define ELLE_MISC_FUNCTION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/misc/Status.hh>
#include <elle/misc/Routine.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;

  namespace misc
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a function.
    ///
    template <typename... T>
    class Function:
      public Routine
    {
    public:
      //
      // types
      //
      typedef Status		(*Handler)(T...);

      //
      // constructors & destructors
      //
      Function(Handler);
      Function(const Function<T...>&);

      //
      // methods
      //
      Status		Trigger(T*...);
      Status		Call(T&...);

      //
      // interfaces
      //

      // entity
      declare(Entity, Function);
      // XXX

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Handler		handler;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/misc/Function.hxx>

#endif
