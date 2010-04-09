//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       misc
//
// file          /home/mycure/infinit/elle/miscellaneous/Function.hh
//
// created       julien quintard   [thu feb  4 22:15:45 2010]
// updated       julien quintard   [tue mar 30 17:29:04 2010]
//

#ifndef ELLE_MISCELLANEOUS_FUNCTION_HH
#define ELLE_MISCELLANEOUS_FUNCTION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/miscellaneous/Status.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;

  namespace miscellaneous
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a function.
    ///
    template <typename... T>
    class Function:
      public Entity,
      public Dumpable
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
      Status		Call(T...);

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

#include <elle/miscellaneous/Function.hxx>

#endif
