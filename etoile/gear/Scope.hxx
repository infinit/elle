//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [fri jul 29 12:54:19 2011]
//

#ifndef ETOILE_GEAR_SCOPE_HXX
#define ETOILE_GEAR_SCOPE_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/journal/Journal.hh>

#include <Infinit.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method returns the context associated with the scope
    /// by casting it with the given type.
    ///
    template <typename T>
    elle::Status	Scope::Use(T*&				context)
    {
      enter();

      // first, if the scope's context is null, allocate one.
      if (this->context == NULL)
	{
	  // allocate a context according to the nature.
	  this->context = new T;
	}

      // return the context by dynamically casting it.
      if ((context = dynamic_cast<T*>(this->context)) == NULL)
	escape("invalid context nature: scope's(%s) target(%s)",
	       NatureName(this->context->nature), NatureName(T::N));

      leave();
    }

    ///
    /// this method returns the shutdown callback to be carried out which
    /// is deduced from the current scope's state.
    ///
    /// note however that if actors are still working on the scope, the
    /// returned callback is null.
    ///
    template <typename T>
    elle::Status	Scope::Shutdown(elle::Callback<
				          elle::Status,
				          elle::Parameters<
				            T&
				          >
				        >&			callback)
    {
      enter();

      // if actors remain, return a null callback.
      //
      // indeed, only the final actor will trigger the shutdown operation. this
      // way potential conflicts are prevents while expensive cryptographic
      // operations are performed only once.
      if (this->actors.empty() == false)
	{
	  // return a null callback.
	  callback =
	    elle::Callback<
	      elle::Status,
	      elle::Parameters<
		T&
		>
	      >::Null;
	}
      else
	{
	  //
	  // otherwise, the current actor is the last one and is responsible
	  // for triggering the shutdown operation.
	  //
	  // therefore, returning a callback depending on the context's
	  // closing operation.
	  //

	  // depending on the closing operation...
	  switch (this->context->operation)
	    {
	    case OperationDiscard:
	      {
		// return the callback on the Disard method.
		callback = elle::Callback<>::Infer(&T::A::Discard);

		break;
	      }
	    case OperationStore:
	      {
		// return the callback on the Store method.
		callback = elle::Callback<>::Infer(&T::A::Store);

		break;
	      }
	    case OperationDestroy:
	      {
		// return the callback on the Destroy method.
		callback = elle::Callback<>::Infer(&T::A::Destroy);

		break;
	      }
	    case OperationUnknown:
	      {
		escape("unable to return a callback on the closing "
		       "operation '%u'\n",
		       this->context->operation);
	      }
	    }
	}

      leave();
    }

    ///
    /// this method is called whenever the scope needs to be refreshed i.e
    /// has lived long enough in main memory so that the risk of it having
    /// been updated on another computer is quite high.
    ///
    /// therefore, this refreshing process is triggered on a regular basis
    /// in order to make sure scopes which are always opened pick get
    /// updated.
    ///
    template <typename T>
    elle::Status	Scope::Refresh()
    {
      elle::Hurdle::S	section(
	elle::Hurdle::L(
	  elle::Hurdle::C(&elle::Hurdle::Lock, &this->hurdle),
	  elle::ModeWrite),
	elle::Hurdle::U(
	  elle::Hurdle::C(&elle::Hurdle::Unlock, &this->hurdle),
	  elle::ModeWrite));

      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
	printf("[etoile] gear::Scope::Refresh()\n");

      // lock the current scope in order to make sure it does not get
      // relinquished or simply modified.
      //
      // this is especially required since Load()ing may block the current
      // fiber.
      section.Enter();
      {
	elle::Callback<
	  elle::Status,
	  elle::Parameters<
	    T&
	    >
	  >		callback;
	T*		context;

	enterx(instance(context));

	// allocate a context.
	context = new T;

	// locate the context based on the current scope's chemin.
	if (this->chemin.Locate(context->location) == elle::StatusError)
	  escape("unable to locate the scope");

	// load the object.
	if (T::A::Load(*context) == elle::StatusError)
	  escape("unable to load the object");

	// check if the loaded object is indeed newer.
	if (context->object.version >
	    static_cast<T*>(this->context)->object.version)
	  {
	    //
	    // in this case, a newer version exists which has been loaded.
	    //
	    // replace the current one with the new one.
	    //

	    // delete the existing context.
	    delete this->context;

	    // set the new context.
	    this->context = context;
	  }
	else
	  {
	    //
	    // otherwise, the loaded object is of the same version as the
	    // current one.
	    //
	    // in this case, nothing is done.
	    //

	    // delete the retrieved context.
	    delete context;
	  }

	// waive.
	waive(context);

	// release.
	release();
      }
      section.Leave();

      leave();
    }

    ///
    /// this method does the opposite of the Refresh() method by disclosing,
    /// i.e storing, the modifications even though the scope has not been
    /// closed yet.
    ///
    /// such a process gets handy when scopes are opened and never closed
    /// by still modified. thanks to the regular disclosing mechanism, the
    /// modifications of scopes having lives for too much time in main
    /// memory are published by force in order to make sure other computers
    /// take notice of those.
    ///
    template <typename T>
    elle::Status	Scope::Disclose()
    {
      elle::Hurdle::S	section(
	elle::Hurdle::L(
	  elle::Hurdle::C(&elle::Hurdle::Lock, &this->hurdle),
	  elle::ModeWrite),
	elle::Hurdle::U(
	  elle::Hurdle::C(&elle::Hurdle::Unlock, &this->hurdle),
	  elle::ModeWrite));

      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
	printf("[etoile] gear::Scope::Disclose()\n");

      // protect the access to the current scope.
      section.Enter();
      {
	Scope*		scope;
	T*		context;
	Actor*		actor;

	enterx(instance(actor),
	       slab(scope, gear::Scope::Annihilate));

	//
	// create a scope, very much as for wall::*::Create(), except
	// that it works even for objects which cannot, obviously, be created.
	//
	{
	  // supply a scope i.e request a new anonymous scope.
	  if (gear::Scope::Supply(scope) == elle::StatusError)
	    escape("unable to supply a scope");

	  // retrieve the context.
	  if (scope->Use(context) == elle::StatusError)
	    escape("unable to retrieve the context");

	  // allocate an actor on the new scope, making the scope valid
	  // for triggering automata.
	  actor = new gear::Actor(scope);
	}

	//
	// swap the contexts.
	//
	{
	  // transfer the current scope's context to the new scope.
	  actor->scope->context = this->context;

	  // set the current scope's context with the new one.
	  this->context = context;
	}

	// store the object which now carries the modified context.
	if (T::W::Store(actor->identifier) == elle::StatusError)
	  escape("unable to store the object");

	// waive the actor.
	waive(actor);

	// waive the scope.
	waive(scope);

	//
	// at this point, a scope has been created, to which the modified
	// context has been transferred. this scope has been stored, hence
	// disclosing its modifications.
	//
	// finally, the current scope's context is allocated but initialized
	// and must therefore be loaded with a fresh version of the object.
	//

	// locate the object based on the current scope's chemin.
	if (this->chemin.Locate(context->location) == elle::StatusError)
	  escape("unable to locate the file");

	// load a fresh version of the object which should happen to be
	// the one stored above.
	if (T::A::Load(*context) == elle::StatusError)
	  escape("unable to load the object");

	// release.
	release();
      }
      section.Leave();

      leave();
    }

  }
}

#endif
