//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/misc/Function.hxx
//
// created       julien quintard   [thu feb  4 22:18:05 2010]
// updated       julien quintard   [fri feb  5 01:13:08 2010]
//

#ifndef ELLE_MISC_FUNCTION_HXX
#define ELLE_MISC_FNUCTION_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/misc/Callback.hh>

namespace elle
{
  namespace misc
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default constructor.
    ///
    template <typename... T>
    Function<T...>::Function(Function::Handler			handler):
      Callback::Callback(Callback::TypeFunction),

      handler(handler)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method actually calls the handler.
    ///
    template <typename... T>
    Status		Function<T...>::Call(T&...		parameters)
    {
      return (this->handler(parameters...));
    }

  }
}

#endif
