//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/context/Context.hxx
//
// created       julien quintard   [sun apr  4 12:50:13 2010]
// updated       julien quintard   [mon apr  5 23:18:59 2010]
//

#ifndef ETOILE_CONTEXT_CONTEXT_HXX
#define ETOILE_CONTEXT_CONTEXT_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/user/User.hh>

namespace etoile
{
  namespace context
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method retrieves a recorded context.
    ///
    /// this method has been templated so that the caller does not have
    /// to cast a subclass context into Context*&. although this is not
    /// strictly required, it makes using contexts a bit easier.
    ///
    template <typename T>
    Status		Context::Retrieve(const Identifier&	identifier,
					  T*&			context)
    {
      Context::Scoutor	scoutor;
      Context::Value	value;
      user::User*	user;

      enter();

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // find the entry.
      if ((scoutor = Context::Contexts.find(identifier)) ==
	  Context::Contexts.end())
	escape("unable to locate the context associated with the "
	       "given identifier");

      // retrieve the value.
      value = scoutor->second;

      // check if the context is related to the current user.
      if (value.first != user->client)
	escape("the user is trying to access a context that she does not own");

      // return the context.
      context = (T*)value.second;

      leave();
    }

  }
}

#endif
