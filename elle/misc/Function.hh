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
// updated       julien quintard   [wed mar 10 20:32:22 2010]
//

#ifndef ELLE_MISC_FUNCTION_HH
#define ELLE_MISC_FUNCTION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/misc/Status.hh>
#include <elle/misc/Report.hh>
#include <elle/misc/Maid.hh>
#include <elle/misc/Callback.hh>

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
