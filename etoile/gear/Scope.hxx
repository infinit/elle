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

      // allocate an actor.
      actor = new Actor(this);

      // allocate a context.
      context = new T;

      // locate the scope based on the chemin.
      if (this->chemin.Locate(context->location) == elle::StatusError)
	escape("unable to locate the scope");

      // load the object.
      if (T::A::Load(*context) == elle::StatusError)
	escape("unable to load the object");

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

      // delete the actor.
      delete actor;

      // waive.
      waive(actor);

      // specify the closing operation performed on the scope.
      if (this->Operate(OperationDiscard) == elle::StatusError)
	escape("unable to specify the operation being performed "
	       "on the scope");

      // retrieve the shutdown callback.
      if (this->Shutdown(callback) == elle::StatusError)
	escape("unable to retrieve the shutdown callback");

      // trigger the closing callback.
      if (callback.Call(*context) == elle::StatusError)
	escape("unable to perform the closing operation");

      // XXX record journal, pour la coherence!

      leave();
    }

    ///
    /// XXX
    ///
    template <typename T>
    elle::Status	Scope::Disclose()
    {
      std::pair<Scope::S::O::Iterator, elle::Boolean>	result;
      elle::Callback<
	elle::Status,
	elle::Parameters<
	  T&
	  >
	>						callback;
      T*						context;
      Scope*						scope;
      Actor*						actor;

      enterx(instance(actor),
	     instance(scope));

      // XXX
      leave();

      // XXX
      log("Disclose()");

      // allocate a new scope but do not register it yet.
      scope = new Scope(this->chemin);

      // create the scope.
      if (scope->Create() == elle::StatusError)
	escape("unable to create the scope");

      // move the current scope's context to the new scope.
      scope->context = this->context;

      // set the current scope's context to null, temporarily.
      this->context = NULL;

      // allocate an actor on the new scope in order to render it valid.
      actor = new Actor(scope);

      // retrieve the context.
      if (scope->Use(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // specify the closing operation performed by the actor.
      if (actor->Operate(OperationDiscard) == elle::StatusError)
	escape("this operation cannot be performed by this actor");

      // delete the actor.
      delete actor;

      // waive the actor.
      waive(actor);

      // specify the closing operation performed on the scope.
      if (scope->Operate(OperationDiscard) == elle::StatusError)
	escape("unable to specify the operation being performed on the scope");

      // retrieve the shutdown callback.
      if (scope->Shutdown(callback) == elle::StatusError)
	escape("unable to retrieve the shutdown callback");

      // trigger the closing callback.
      if (callback.Call(*context) == elle::StatusError)
	escape("unable to perform the closing operation");

      // record the scope in the journal.
      if (journal::Journal::Record(scope) == elle::StatusError)
	escape("unable to record the scope in the journal");

      // waive the scope.
      waive(scope);

      // allocate a new context for the current scope.
      if (this->Use(context) == elle::StatusError)
	escape("unable to allocate a new context");

      // locate the scope based on the chemin.
      if (this->chemin.Locate(context->location) == elle::StatusError)
	escape("unable to locate the scope");

      // XXX try to block.

      // reload the object, theoretically from the journal since it has
      // been recorded to storage just above.
      //
      // note that the fiber must not block or the current actors could
      // continue accessing the scope which, for now, contains an
      // uninitialized context.
      if (T::A::Load(*context) == elle::StatusError)
	escape("unable to load the object");

      // XXX make sure the call above retrieve from the journal.

      leave();
    }

  }
}

#endif
