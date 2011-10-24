//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [wed jun 15 13:09:29 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/gear/Scope.hh>
#include <etoile/gear/Gear.hh>
#include <etoile/gear/Directory.hh>
#include <etoile/gear/File.hh>
#include <etoile/gear/Link.hh>
#include <etoile/gear/Object.hh>

#include <etoile/automaton/Object.hh>
#include <etoile/automaton/File.hh>
#include <etoile/automaton/Directory.hh>
#include <etoile/automaton/Link.hh>

#include <Infinit.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this container holds the scopes accessible through a chemin.
    ///
    Scope::S::O::Container		Scope::Scopes::Onymous;

    ///
    /// this container holds the anonymous---i.e freshly author ---scopes.
    ///
    Scope::S::A::Container		Scope::Scopes::Anonymous;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the scope system.
    ///
    elle::Status	Scope::Initialize()
    {
      enter();

      // nothing to do.

      leave();
    }

    ///
    /// this method cleans the scope system.
    ///
    elle::Status	Scope::Clean()
    {
      enter();

      //
      // release the onymous scopes.
      //
      {
	Scope::S::O::Scoutor	scoutor;

	// go through the container.
	for (scoutor = Scope::Scopes::Onymous.begin();
	     scoutor != Scope::Scopes::Onymous.end();
	     scoutor++)
	  {
	    Scope*		scope = scoutor->second;

	    // delete the scope.
	    delete scope;
	  }

	// clear the container.
	Scope::Scopes::Onymous.clear();
      }

      //
      // release the anonymous scopes.
      //
      {
	Scope::S::A::Scoutor	scoutor;

	// go through the container.
	for (scoutor = Scope::Scopes::Anonymous.begin();
	     scoutor != Scope::Scopes::Anonymous.end();
	     scoutor++)
	  {
	    Scope*		scope = *scoutor;

	    // delete the scope.
	    delete scope;
	  }

	// clear the container.
	Scope::Scopes::Anonymous.clear();
      }

      leave();
    }

    ///
    /// this method tries to locate an existing scope given the chemin.
    ///
    /// if none exists, a scope is created and added to the container.
    ///
    elle::Status	Scope::Acquire(const path::Chemin&	chemin,
				       Scope*&			scope)
    {
      Scope::S::O::Scoutor	scoutor;

      enter();

      // find the entry.
      if ((scoutor = Scope::Scopes::Onymous.find(chemin)) ==
	  Scope::Scopes::Onymous.end())
	{
	  std::pair<Scope::S::O::Iterator, elle::Boolean>	result;
	  Scope*						s;

	  enterx(instance(s));

	  // allocate a new scope.
	  s = new Scope(chemin);

	  // create the scope.
	  if (s->Create() == elle::StatusError)
	    escape("unable to create the scope");

	  // insert the scope in the container.
	  result = Scope::Scopes::Onymous.insert(
		     std::pair<const path::Chemin, Scope*>(chemin, s));

	  // check the result.
	  if (result.second == false)
	    escape("unable to insert the scope in the container");

	  // return the scope.
	  scope = s;

	  // waive.
	  waive(s);
	}
      else
	{
	  // return the existing scope.
	  scope = scoutor->second;
	}

      leave();
    }

    ///
    /// this specific-version of the method creates a scope without
    /// looking for an existing one.
    ///
    elle::Status	Scope::Supply(Scope*&			scope)
    {
      Scope*		s;

      enterx(instance(s));

      // allocate a new scope.
      s = new Scope;

      // create the scope.
      if (s->Create() == elle::StatusError)
	escape("unable to create the scope");

      // insert the scope in the anonymous container.
      Scope::Scopes::Anonymous.push_back(s);

      // return the scope.
      scope = s;

      // waive.
      waive(s);

      leave();
    }

    ///
    /// this method removes the scope from its container making the scope
    /// orphan.
    ///
    /// therefore, it is the responsibility of the caller to delete the scope.
    ///
    elle::Status	Scope::Relinquish(Scope*		scope)
    {
      enter();

      // depending on the scope type.
      if (scope->chemin == path::Chemin::Null)
	{
	  Scope::S::A::Iterator	iterator;

	  //
	  // in this case the scope is anonymous.
	  //

	  Scope::Scopes::Anonymous.remove(scope);
	}
      else
	{
	  Scope::S::O::Iterator	iterator;

	  // find the entry.
	  if ((iterator =
	         Scope::Scopes::Onymous.find(scope->chemin)) ==
	      Scope::Scopes::Onymous.end())
	    escape("unable to locate the scope associated with the chemin");

	  // erase the entry.
	  Scope::Scopes::Onymous.erase(iterator);
	}

      leave();
    }

    ///
    /// should the given scope become unused---i.e no more actor is
    /// operating on it---, it is relinquished and deleted.
    ///
    elle::Status	Scope::Annihilate(Scope*		scope)
    {
      enter();

      // if no actor operates on it anymore.
      if (scope->actors.empty() == true)
	{
	  // relinquish the scope.
	  if (Scope::Relinquish(scope) == elle::StatusError)
	    escape("unable to relinquish the scope");

	  // and finally, delete it.
	  delete scope;
	}

      leave();
    }

    ///
    /// this method displays the containers.
    ///
    elle::Status	Scope::Show(const elle::Natural32	margin)
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Scope]" << std::endl;

      //
      // onymous scopes.
      //
      {
	Scope::S::O::Scoutor	scoutor;

	std::cout << alignment << elle::Dumpable::Shift
		  << "[Onymous]" << std::endl;

	// go through the onymous scopes.
	for (scoutor = Scope::Scopes::Onymous.begin();
	     scoutor != Scope::Scopes::Onymous.end();
	     scoutor++)
	  {
	    // dump the scope.
	    if (scoutor->second->Dump(margin + 4) == elle::StatusError)
	      escape("unable to dump the scope");
	  }
      }

      //
      // anonymous scopes.
      //
      {
	Scope::S::A::Scoutor	scoutor;

	std::cout << alignment << elle::Dumpable::Shift
		  << "[Anonymous]" << std::endl;

	// go through the anonymous scopes.
	for (scoutor = Scope::Scopes::Anonymous.begin();
	     scoutor != Scope::Scopes::Anonymous.end();
	     scoutor++)
	  {
	    // dump the scope.
	    if ((*scoutor)->Dump(margin + 4) == elle::StatusError)
	      escape("unable to dump the scope");
	  }
      }

      leave();
    }

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor especially useful for anonymous scopes.
    ///
    Scope::Scope():
      state(StateNone),
      context(NULL),
      chronicle(NULL)
    {
    }

    ///
    /// chemin-specific constructor.
    ///
    Scope::Scope(const path::Chemin&				chemin):
      state(StateNone),
      chemin(chemin),
      context(NULL),
      chronicle(NULL)
    {
    }

    ///
    /// destructor.
    ///
    Scope::~Scope()
    {
      Scope::A::Scoutor	scoutor;

      // stop the timer.
      this->timer.Stop();

      // delete the context.
      if (this->context != NULL)
	delete this->context;

      // delete the chronicle.
      if (this->chronicle != NULL)
	delete this->chronicle;

      // release the actors, if some remain.
      for (scoutor = this->actors.begin();
	   scoutor != this->actors.end();
	   scoutor++)
	{
	  Actor*	actor = *scoutor;

	  // delete the actor.
	  delete actor;
	}

      // clear the actors container.
      this->actors.clear();
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a scope.
    ///
    elle::Status	Scope::Create()
    {
      enter();

      // create the supervisor timer.
      if (this->timer.Create(
	    elle::Timer::ModeRepetition) == elle::StatusError)
	escape("unable to create the timer");

      // subscribe to the timer's signal.
      if (this->timer.signal.timeout.Subscribe(
	    elle::Callback<>::Infer(&Scope::Supervisor,
				    this)) == elle::StatusError)
	escape("unable to subscribe to the signal");

      // start the timer.
      if (this->timer.Start(
	    Infinit::Configuration.etoile.gear.containment) ==
	  elle::StatusError)
	escape("unable to start the timer");

      leave();
    }

    ///
    /// this method adds an actor to the scope's set of actors.
    ///
    elle::Status	Scope::Attach(Actor*			actor)
    {
      enter();

      // try to locate an existing actor.
      if (this->Locate(actor) == true)
	escape("this actor seems to have been already registered");

      // add the actor to the container.
      this->actors.push_back(actor);

      leave();
    }

    ///
    /// this method returns true if the actor has been located.
    ///
    elle::Status	Scope::Locate(Actor*			actor,
				      Scope::A::Iterator*	iterator)
    {
      Scope::A::Iterator	i;

      enter();

      // go through the set of actors.
      for (i = this->actors.begin();
	   i != this->actors.end();
	   i++)
	{
	  // try this actor.
	  if (actor == *i)
	    {
	      // return the iterator if necessary.
	      if (iterator != NULL)
		*iterator = i;

	      true();
	    }
	}

      false();
    }

    ///
    /// this method remvoes an actor from the scope's set of actors.
    ///
    elle::Status	Scope::Detach(Actor*			actor)
    {
      Scope::A::Iterator	iterator;

      enter();

      // try to locate an existing actor.
      if (this->Locate(actor, &iterator) == false)
	escape("no such actor seems to have been registered");

      // remove the actor.
      this->actors.erase(iterator);

      leave();
    }

    ///
    /// this method is called to indicate the operation being performed
    /// on the scope by the actor.
    ///
    /// note that since multiple actors operate on the same scope, one must
    /// assume that other actors may have modified or even destroy the
    /// scope's target.
    ///
    elle::Status	Scope::Operate(const Operation		operation)
    {
      enter();

      // update the context's closing operation according to its given
      // value and the given operation.
      switch (operation)
	{
	case OperationDiscard:
	  {
	    // depending on the current context's closing operation.
	    switch (this->context->operation)
	      {
	      case OperationUnknown:
		{
		  //
		  // in this case, the given closing operation is the first
		  // one to take place.
		  //
		  // thus, the context is marked as requiring to be discarded.
		  //

		  // set the context's closing operation.
		  this->context->operation = operation;

		  break;
		}
	      case OperationDiscard:
		{
		  //
		  // the given closing operation is the same as the current
		  // context's.
		  //
		  // thus, everything seems fine this way.
		  //

		  break;
		}
	      case OperationStore:
		{
		  //
		  // the context's modifications have been marked as requiring
		  // to be stored.
		  //
		  // therefore, the given operation does not imply any change
		  // of plan.
		  //

		  break;
		}
	      case OperationDestroy:
		{
		  //
		  // as for the OperationStore, in this case, the context
		  // has been marked for deletion.
		  //
		  // therefore, the discarding given operation does not
		  // change the scope's closing operation i.e Destroy.
		  //

		  break;
		}
	      }

	    break;
	  }
	case OperationStore:
	  {
	    // depending on the current context's closing operation.
	    switch (this->context->operation)
	      {
	      case OperationUnknown:
		{
		  //
		  // in this case, the given closing operation is the first
		  // one to take place.
		  //
		  // thus, the context is marked as requiring to be stored.
		  //

		  // set the context's closing operation.
		  this->context->operation = operation;

		  break;
		}
	      case OperationDiscard:
		{
		  //
		  // the given closing operation is of higher importance than
		  // the existing one.
		  //
		  // therefore, the closing operation is set to: Store.
		  //

		  // set the context's closing operation.
		  this->context->operation = operation;

		  break;
		}
	      case OperationStore:
		{
		  //
		  // the context's modifications have been marked as requiring
		  // to be stored.
		  //
		  // since the given operation is identical, the context's
		  // closing operation does not need to be changed.
		  //

		  break;
		}
	      case OperationDestroy:
		{
		  //
		  // in this case, the context has been marked for deletion.
		  //
		  // since the storing given operation is of lower importance,
		  // the closing operation is maintained.
		  //

		  break;
		}
	      }

	    break;
	  }
	case OperationDestroy:
	  {
	    // depending on the current context's closing operation.
	    switch (this->context->operation)
	      {
	      case OperationUnknown:
		{
		  //
		  // in this case, the given closing operation is the first
		  // one to take place.
		  //
		  // thus, the context is marked as requiring to be destroyed.
		  //

		  // set the context's closing operation.
		  this->context->operation = operation;

		  break;
		}
	      case OperationDiscard:
		{
		  //
		  // the given closing operation is of higher importance than
		  // the existing one.
		  //
		  // therefore, the closing operation is set to: Destroy.
		  //

		  // set the context's closing operation.
		  this->context->operation = operation;

		  break;
		}
	      case OperationStore:
		{
		  //
		  // in this case, although some actors may have modified
		  // the context---since the current closing operation is
		  // Store---, the operation is set to Destroy because the
		  // given operation superseeds the current one.
		  //

		  // set the context's closing operation.
		  this->context->operation = operation;

		  break;
		}
	      case OperationDestroy:
		{
		  //
		  // in this case, the context has already been marked
		  // for deletion.
		  //
		  // therefore, the closing operation is maintained.
		  //

		  break;
		}
	      }

	    break;
	  }
	case OperationUnknown:
	  {
	    escape("unable to process the closing operation '%u'\n",
		   operation);
	  }
	}

      leave();
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this callback is triggered whenever the scope is considered having
    /// expired.
    ///
    /// if the scope has reached a certain lifetime and has not been modified,
    /// the supervisor refreshes it by fetching a potentially new version of
    /// the object. this ensures that a scope being still in use because of
    /// an actor never releasing it, will, from time to time, be refreshed
    /// in order to provide the active actors the latest view.
    ///
    /// likewise, if the scope has reached a certain lifetime and has been
    /// modified, the supervisor forces the scope's modifications to be
    /// disclosed so that, although non-active actors remained attached
    /// to this scope, the network gets updated according to the modifications
    /// performed locally.
    ///
    elle::Status	Scope::Supervisor()
    {
      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
	printf("[etoile] gear::Scope::Supervisor()\n");

      // if the scope is already being taking care of, ignore this
      // supervision.
      if (this->state != Scope::StateNone)
	leave();

      // if this scope is anonymous, i.e has been created, there is no
      // need to refresh it nor too disclose its modifications since nobody
      // can load it but the actor having created it.
      if (this->chemin == path::Chemin::Null)
	leave();

      // depending on the context's state.
      switch (this->context->state)
	{
	case Context::StateUnknown:
	case Context::StateCleaned:
	  {
	    escape("unexpected state '%u'",
		   this->context->state);
	  }
	case Context::StateInitialized:
	case Context::StateDiscarded:
	  {
	    // set the state.
	    this->state = Scope::StateDisclose;

	    // perform the refreshing depending on the context's nature.
	    switch (this->context->nature)
	      {
	      case NatureUnknown:
		{
		  // reset the state.
		  this->state = Scope::StateNone;

		  escape("unknown context nature");
		}
	      case NatureObject:
		{
		  // refresh the scope.
		  if (this->Refresh<gear::Object>() == elle::StatusError)
		    {
		      // reset the state.
		      this->state = Scope::StateNone;

		      escape("unable to refresh the scope");
		    }

		  break;
		}
	      case NatureFile:
		{
		  // refresh the scope.
		  if (this->Refresh<gear::File>() == elle::StatusError)
		    {
		      // reset the state.
		      this->state = Scope::StateNone;

		      escape("unable to refresh the scope");
		    }

		  break;
		}
	      case NatureDirectory:
		{
		  // refresh the scope.
		  if (this->Refresh<gear::Directory>() == elle::StatusError)
		    {
		      // reset the state.
		      this->state = Scope::StateNone;

		      escape("unable to refresh the scope");
		    }

		  break;
		}
	      case NatureLink:
		{
		  // refresh the scope.
		  if (this->Refresh<gear::Link>() == elle::StatusError)
		    {
		      // reset the state.
		      this->state = Scope::StateNone;

		      escape("unable to refresh the scope");
		    }

		  break;
		}
	      }

	    // reset the state.
	    this->state = Scope::StateNone;

	    break;
	  }
	case Context::StateModified:
	case Context::StateStored:
	case Context::StateDestroyed:
	  {
	    // set the state.
	    this->state = Scope::StateRefresh;

	    // perform the disclosure depending on the context's nature.
	    switch (this->context->nature)
	      {
	      case NatureUnknown:
		{
		  // reset the state.
		  this->state = Scope::StateNone;

		  escape("unknown context nature");
		}
	      case NatureObject:
		{
		  // disclose the scope.
		  if (this->Disclose<gear::Object>() == elle::StatusError)
		    {
		      // reset the state.
		      this->state = Scope::StateNone;

		      escape("unable to disclose the scope");
		    }

		  break;
		}
	      case NatureFile:
		{
		  // disclose the scope.
		  if (this->Disclose<gear::File>() == elle::StatusError)
		    {
		      // reset the state.
		      this->state = Scope::StateNone;

		      escape("unable to disclose the scope");
		    }

		  break;
		}
	      case NatureDirectory:
		{
		  // disclose the scope.
		  if (this->Disclose<gear::Directory>() == elle::StatusError)
		    {
		      // reset the state.
		      this->state = Scope::StateNone;

		      escape("unable to disclose the scope");
		    }

		  break;
		}
	      case NatureLink:
		{
		  // disclose the scope.
		  if (this->Disclose<gear::Link>() == elle::StatusError)
		    {
		      // reset the state.
		      this->state = Scope::StateNone;

		      escape("unable to disclose the scope");
		    }

		  break;
		}
	      }

	    // reset the state.
	    this->state = Scope::StateNone;

	    break;
	  }
	}

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps a scope.
    ///
    elle::Status	Scope::Dump(const elle::Natural32	margin) const
    {
      elle::String	alignment(margin, ' ');
      Scope::A::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Scope]" << std::endl;

      // dump the chemin.
      if (this->chemin.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the chemin");

      // dump the context, if present.
      if (this->context != NULL)
	{
	  if (this->context->Dump(margin + 2) == elle::StatusError)
	    escape("unable to dump the context");
	}
      else
	{
	  std::cout << alignment << elle::Dumpable::Shift
		    << "[Context] " << elle::none << std::endl;
	}

      // dump the chronicle, if present.
      if (this->chronicle != NULL)
	{
	  if (this->chronicle->Dump(margin + 2) == elle::StatusError)
	    escape("unable to dump the chronicle");
	}
      else
	{
	  std::cout << alignment << elle::Dumpable::Shift
		    << "[Chronicle] " << elle::none << std::endl;
	}

      std::cout << alignment << elle::Dumpable::Shift
		<< "[Actors]" << std::endl;

      // go through the actors.
      for (scoutor = this->actors.begin();
	   scoutor != this->actors.end();
	   scoutor++)
	{
	  // dump the actor.
	  if ((*scoutor)->Dump(margin + 4) == elle::StatusError)
	    escape("unable to dump the actor");
	}

      leave();
    }

  }
}
