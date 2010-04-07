//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/context/Context.cc
//
// created       julien quintard   [wed mar 31 02:37:45 2010]
// updated       julien quintard   [tue apr  6 14:36:41 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/context/Context.hh>

namespace etoile
{
  namespace context
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this container holds the contexts in use.
    ///
    Context::Container		Context::Contexts;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Context::Context(const Format&				format):
      format(format)
    {
    }

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method adds a context to the container.
    ///
    Status		Context::Add(const Identifier&		identifier,
				     Context*			context)
    {
      std::pair<Context::Iterator, Boolean>	result;
      user::User*	user;

      enter();

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

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
    /// this method removes a recorded context.
    ///
    Status		Context::Remove(const Identifier&	identifier)
    {
      user::User*	user;
      Context::Iterator	iterator;
      Context::Value	value;

      enter();

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // find the entry.
      if ((iterator = Context::Contexts.find(identifier)) ==
	  Context::Contexts.end())
	escape("unable to locate the context associated with the "
	       "given identifier");

      // retrieve the value.
      value = iterator->second;

      // check if the context is related to the current user.
      if (value.first != user->client)
	escape("the user is trying to remove a context that she does not own");

      // erase the entry.
      Context::Contexts.erase(iterator);

      leave();
    }

  }
}
