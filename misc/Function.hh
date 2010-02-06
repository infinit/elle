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
// updated       julien quintard   [fri feb  5 13:20:01 2010]
//

//
// ---------- includes --------------------------------------------------------
//

///
/// this include has been put here to avoid pre-processing issues.
///
#include <elle/misc/Callback.hh>

#ifndef ELLE_MISC_FUNCTION_HH
#define ELLE_MISC_FUNCTION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>

namespace elle
{
  using namespace core;

  namespace misc
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a function callback.
    ///
    template <typename... T>
    class Function:
      public Callback
    {
    public:
      //
      // types
      //
      typedef Status			(*Handler)(T&...);

      //
      // constructors & destructors
      //
      Function(Handler);

      //
      // methods
      //
      Status		Call(T&...);

      //
      // interfaces
      //

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
