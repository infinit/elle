//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Actor.cc
//
// created       julien quintard   [thu jul 28 13:05:00 2011]
// updated       julien quintard   [sat jul 30 13:31:37 2011]
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
    /// XXX
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

    // XXX show static method

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Actor::Actor(Scope*						scope):
      scope(scope)
    {
      // generate an identifier.
      if (this->identifier.Generate() == elle::StatusError)
	fail("unable to generate the identifier");
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method attaches the actor to the scope.
    ///
    elle::Status	Actor::Attach()
    {
      enter();

      // register the actor.
      if (Actor::Add(this->identifier, this) == elle::StatusError)
	escape("unable to register the actor");

      // add the actor to the scope's set.
      if (scope->Attach(this) == elle::StatusError)
	escape("unable to attach the actor to the scope");

      leave();
    }

    ///
    /// this method detaches the actor from the scope.
    ///
    elle::Status	Actor::Detach()
    {
      enter();

      // remove the actor from the scope's set.
      if (scope->Detach(this) == elle::StatusError)
	escape("unable to detach the actor from the scope");

      // unregister the actor.
      if (Actor::Remove(this->identifier) == elle::StatusError)
	escape("unable to unregister the actor");

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

      leave();
    }

  }
}
