//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/user/Application.hxx
//
// created       julien quintard   [wed apr 21 20:59:52 2010]
// updated       julien quintard   [thu apr 22 16:06:15 2010]
//

#ifndef ETOILE_USER_APPLICATION_HXX
#define ETOILE_USER_APPLICATION_HXX

namespace etoile
{
  namespace user
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method adds the context to the container, making it usable
    /// by the application as long as it provides its identifier.
    ///
    template <typename T>
    Status		Application::Add(const context::Identifier& identifier,
					 T*			context)
    {
      std::pair<Application::Iterator, Boolean>	result;

      enter();

      // insert the context in the container.
      result = this->contexts.insert(
	         std::pair<const context::Identifier,
		           context::Context*>(identifier,
					      context));

      // check the result.
      if (result.second == false)
	escape("unable to insert the context in the container");

      leave();
    }

    ///
    /// this method retrieves a recorded context.
    ///
    template <typename T>
    Status		Application::Retrieve(const
					        context::Identifier&
					          identifier,
					      T*&		context)
    {
      Application::Scoutor	scoutor;

      enter();

      // find the entry.
      if ((scoutor = this->contexts.find(identifier)) == this->contexts.end())
	escape("unable to locate the context associated with the "
	       "given identifier");

      // retrieve the context.
      context = (T*)scoutor->second;

      // update the fiber. indeed, multiple fibers may access the same
      // context. in order to alwasy track which fiber is using the
      // context, this method sets the current fiber in the context so that,
      // should the application die while the fiber is blocked, the system
      // can retrieve and delete both the application and fiber.
      context->fiber = Fiber::Current;

      leave();
    }

  }
}

#endif
