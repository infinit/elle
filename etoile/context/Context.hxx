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
// updated       julien quintard   [fri apr 16 11:27:00 2010]
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
    /// this method adds the context to the container, making it usable
    /// by external applications.
    ///
    template <typename T>
    Status		Context::Add(T*				context)
    {
      std::pair<Context::Iterator, Boolean>	result;
      user::User*				user;
      Identifier				identifier;

      enter();

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // generate an identifier.
      if (identifier.Generate() == StatusError)
	escape("unable to generate an identifier");

      // identify the context.
      if (context->Identify(identifier) == StatusError)
	escape("unable to identify the context");

      // insert the context in the container.
      result = Context::Contexts.insert(
		 std::pair<const Identifier,
		           Context::Value>(identifier,
					   Context::Value(user->client,
							  context)));

      // check the result.
      if (result.second == false)
	escape("unable to insert the context in the container");

      leave();
    }

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

    ///
    /// this method removes a recorded context.
    ///
    template <typename T>
    Status		Context::Remove(T*			context)
    {
      Context::Iterator	iterator;
      Context::Value	value;
      user::User*	user;

      enter();

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // find the entry.
      if ((iterator = Context::Contexts.find(context->identifier)) ==
	  Context::Contexts.end())
	escape("unable to locate the context associated with the "
	       "given identifier");

      // retrieve the value.
      value = iterator->second;

      // check if the context is related to the current user.
      if (value.first != user->client)
	escape("the user is trying to remove a context that she "
	       "does not own");

      // erase the entry.
      Context::Contexts.erase(iterator);

      leave();
    }

  }
}

#endif
