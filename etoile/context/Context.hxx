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
// updated       julien quintard   [tue apr 27 21:06:40 2010]
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
    /// this method generates a new context.
    ///
    template <typename T>
    Status		Context::New(T*&			context)
    {
      enter();

      // first, allocate a new context.
      context = new T;

      // then, create the context.
      if (context->Create() == StatusError)
	escape("unable to create the context");

      leave();
    }

    ///
    /// this method deletes a context.
    ///
    template <typename T>
    Status		Context::Delete(T*			context)
    {
      enter();

      // delete the context.
      delete context;

      leave();
    }

    ///
    /// this method exports a context by storing it in the
    /// appropriate container.
    ///
    template <typename T>
    Status		Context::Export(T*			context)
    {
      enter();

      // mark the context as being exported.
      context->type = Context::TypeExternal;

      // store the context in the application.
      if (context->application->Add(context->identifier,
				    context) == StatusError)
	escape("unable to add the context");

      leave();
    }

    ///
    /// this method removes the context from the exported list, if
    /// necessary.
    ///
    template <typename T>
    Status		Context::Import(T*			context)
    {
      enter();

      // if the context is internal, skip.
      if (context->type == Context::TypeInternal)
	leave();

      // otherwise, remove the context from the application's container.
      if (context->application->Remove(context->identifier) == StatusError)
	escape("unable to remove the context");

      leave();
    }

  }
}

#endif
