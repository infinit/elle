//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [thu jul 28 13:05:00 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/gear/Actor.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this data structure holds all the living actors.
    ///
    Actor::Container		Actor::Actors;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method retrieves an actor according to its identifier.
    ///
    elle::Status	Actor::Add(const Identifier&		identifier,
				   Actor*			actor)
    {
      std::pair<Actor::Iterator, elle::Boolean>	result;

      enter();

      // check if this identifier has already been recorded.
      if (Actor::Actors.find(identifier) != Actor::Actors.end())
	escape("this actor seems to have already been registered");

      // insert the actor in the container.
      result = Actor::Actors.insert(
	         std::pair<const Identifier, Actor*>(identifier, actor));

      // check the result.
      if (result.second == false)
	escape("unable to insert the actor in the container");

      leave();
    }

    ///
    /// this method returns the actor associated with the given identifier.
    ///
    elle::Status	Actor::Select(const Identifier&		identifier,
				      Actor*&			actor)
    {
      Actor::Scoutor	scoutor;

      enter();

      // find the entry.
      if ((scoutor = Actor::Actors.find(identifier)) == Actor::Actors.end())
	escape("unable to locate the actor associated with the identifier");

      // return the actor.
      actor = scoutor->second;

      leave();
    }

    ///
    /// this method removes an actor from the container.
    ///
    elle::Status	Actor::Remove(const Identifier&		identifier)
    {
      Actor::Iterator	iterator;

      enter();

      // find the entry.
      if ((iterator = Actor::Actors.find(identifier)) == Actor::Actors.end())
	escape("unable to locate the actor associated with the identifier");

      // erase the entry.
      Actor::Actors.erase(iterator);

      leave();
    }

    ///
    /// this method displays the actors data structure.
    ///
    elle::Status	Actor::Show(const elle::Natural32	margin)
    {
      elle::String	alignment(margin, ' ');
      Actor::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Actor]" << std::endl;

      // go through the container.
      for (scoutor = Actor::Actors.begin();
	   scoutor != Actor::Actors.end();
	   scoutor++)
	{
	  // dump the identifier.
	  if (scoutor->first.Dump(margin + 2) == elle::StatusError)
	    escape("unable to dump the identifier");

	  // dump the actor.
	  if (scoutor->second->Dump(margin + 2) == elle::StatusError)
	    escape("unable to dump the actor");

	}

      leave();
    }

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Actor::Actor(Scope*						scope):
      scope(scope),
      state(Actor::StateClean)
    {
      enter();

      // generate an identifier.
      if (this->identifier.Generate() == elle::StatusError)
	yield(_(), "unable to generate the identifier");

      // register the actor.
      if (Actor::Add(this->identifier, this) == elle::StatusError)
	yield(_(), "unable to register the actor");

      // add the actor to the scope's set.
      if (scope->Attach(this) == elle::StatusError)
	yield(_(), "unable to attach the actor to the scope");

      release();
    }

    ///
    /// destructor.
    ///
    Actor::~Actor()
    {
      enter();

      // remove the actor from the scope's set.
      if (scope->Detach(this) == elle::StatusError)
	yield(_(), "unable to detach the actor from the scope");

      // unregister the actor.
      if (Actor::Remove(this->identifier) == elle::StatusError)
	yield(_(), "unable to unregister the actor");

      release();
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method is called to indicate the operation being performed
    /// by the actor.
    ///
    /// let us recall that multiple actors operate on the same scope.
    ///
    /// therefore, since modifications are directly applied on the context
    /// when requested, an actor cannot perform modifications and finally
    /// decide to discard them.
    ///
    /// this method therefore checks that the operation is consistent
    /// regarding the previous requests i.e the actor's state.
    ///
    elle::Status	Actor::Operate(const Operation		operation)
    {
      enter();

      // process the operation.
      switch (operation)
	{
	case OperationDiscard:
	  {
	    //
	    // the actor is discarding the context.
	    //
	    // thus, the actor must never have performed a modification
	    // on the context.
	    //

	    // check the state.
	    if (this->state != Actor::StateClean)
	      escape("unable to discard previously performed modifications");

	    break;
	  }
	case OperationStore:
	  {
	    //
	    // the actor is storing the potentially modified context.
	    //
	    // there is nothing to do here: should the actor have updated
	    // the context or not, the store operation can be requested.
	    //

	    break;
	  }
	case OperationDestroy:
	  {
	    //
	    // the actor is destroying the context, even though it
	    // has been modified.
	    //
	    // as for the store operation, this operation is valid
	    // no matter the actor's state.
	    //

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
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps an actor.
    ///
    elle::Status	Actor::Dump(const elle::Natural32	margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Actor]" << std::endl;

      // dump the identifier.
      if (this->identifier.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the identifier");

      // dump the scope's address.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Scope] " << std::hex << this->scope << std::endl;

      // dump the state.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[State] " << std::dec << this->state << std::endl;

      leave();
    }

  }
}
