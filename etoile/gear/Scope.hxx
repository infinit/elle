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
	escape("invalid context nature: got(%s) expected(%s)",
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

      // if actors remain i.e apart from the current one, return a null
      // callback.
      //
      // indeed, only the final actor will trigger the shutdown operation. this
      // way potential conflicts are prevents while expensive cryptographic
      // operations are performed only once.
      if (this->actors.size() > 1)
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
    /// XXX
    ///
    template <typename T>
    elle::Status	Scope::Refresh()
    {
      elle::Callback<
	elle::Status,
	elle::Parameters<
	  T&
	  >
	>		callback;
      T*		context;
      nucleus::Location	location;
      Actor*		actor;

      enterx(instance(actor),
	     instance(context));

      // locate the scope based on the chemin.
      if (this->chemin.Locate(location) == elle::StatusError)
	escape("unable to locate the scope");

      // allocate an actor.
      actor = new Actor(this);

      // attach the actor to the scope so that this scope does not
      // get relinquished until the refreshing process terminates.
      if (actor->Attach() == elle::StatusError)
	escape("unable to attach the actor to the scope");

      // set the state.
      this->state = Scope::StateRefresh;

      // allocate a context.
      context = new T;

      // load the object.
      if (T::A::Load(*context, location) == elle::StatusError)
	{
	  // reset the state.
	  this->state = Scope::StateNone;

	  // detach the actor.
	  if (actor->Detach() == elle::StatusError)
	    escape("unable to detach the actor");

	  escape("unable to load the object");
	}

      // verify that the context's state has not change in between.
      if (!((this->context->state == Context::StateInitialized) ||
	    (this->context->state == Context::StateDiscarded)))
	escape("the context's state has changed during the supervision");

      // delete the existing context.
      delete this->context;

      // set the new context.
      this->context = context;

      // waive.
      waive(context);

      // specify the closing operation performed by the actor.
      if (actor->Operate(OperationDiscard) == elle::StatusError)
	escape("this operation cannot be performed by this actor");

      // specify the closing operation performed on the scope.
      if (actor->scope->Operate(OperationDiscard) == elle::StatusError)
	escape("unable to specify the operation being performed "
	       "on the scope");

      // retrieve the shutdown callback.
      if (actor->scope->Shutdown(callback) == elle::StatusError)
	escape("unable to retrieve the shutdown callback");

      // trigger the closing callback.
      if (callback.Call(*context) == elle::StatusError)
	escape("unable to perform the closing operation");

      // detach the actor.
      if (actor->Detach() == elle::StatusError)
	escape("unable to detach the actor");

      // delete the actor.
      delete actor;

      // waive.
      waive(actor);

      // reset the state.
      this->state = Scope::StateNone;

      leave();
    }

  }
}

#endif
