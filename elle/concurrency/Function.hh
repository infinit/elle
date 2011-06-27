//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Function.hh
//
// created       julien quintard   [thu feb  4 22:15:45 2010]
// updated       julien quintard   [mon jun 27 18:35:27 2011]
//

#ifndef ELLE_CONCURRENCY_FUNCTION_HH
#define ELLE_CONCURRENCY_FUNCTION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Object.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace radix;
  using namespace package;

  namespace concurrency
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a function.
    ///
    template <typename... T>
    class Function:
      public Object
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

      // object
      declare(Function);

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

#include <elle/concurrency/Function.hxx>

#endif
