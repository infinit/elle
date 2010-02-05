//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/misc/Method.hxx
//
// created       julien quintard   [thu feb  4 23:08:34 2010]
// updated       julien quintard   [fri feb  5 01:14:34 2010]
//

#ifndef ELLE_MISC_METHOD_HXX
#define ELLE_MISC_METHOD_HXX

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
    template <typename X>
    Method<T...>::Method(X*				instance,
			 Status				(X::*handler)(T&...)):
      Callback::Callback(Callback::TypeMethod),

      instance(static_cast<Callable*>(instance)),
      handler(static_cast<Method::Handler>(handler))
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method actually calls the handler.
    ///
    template <typename... T>
    Status		Method<T...>::Call(T&...		parameters)
    {
      return ((this->instance->*this->handler)(parameters...));
    }

  }
}

#endif
