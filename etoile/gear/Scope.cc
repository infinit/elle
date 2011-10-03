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

	  enter(instance(s));

	  // allocate a new scope.
	  s = new Scope(chemin);

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

      enter(instance(s));

      // allocate a new scope.
      s = new Scope;

      // add the scope.
      Scope::Scopes::Anonymous.push_back(s);

      // return the scope.
      scope = s;

      // waive.
      waive(s);

      leave();
    }

    ///
    /// this method deletes a scope if no actor is operating on it anymore.
    ///
    elle::Status	Scope::Relinquish(Scope*		scope)
    {
      enter();

      // if actors remain, keep the scope.
      if (scope->actors.empty() == false)
	leave();

      //
      // otherwise, the scope can be removed.
      //

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

      // delete the scope.
      delete scope;

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
      context(NULL),
      chronicle(NULL)
    {
    }

    ///
    /// chemin-specific constructor.
    ///
    Scope::Scope(const path::Chemin&				chemin):
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
      // delete the context.
      if (this->context != NULL)
	delete this->context;

      // delete the chronicle.
      if (this->chronicle != NULL)
	delete this->chronicle;
    }

//
// ---------- methods ---------------------------------------------------------
//

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
    /// note that since multiple actors operate on the same scope,
    /// one must assume that the other actors may modify or even
    /// destroy the scope's target.
    ///
    elle::Status	Scope::Operate(const Operation		operation)
    {
      enter();

      // process the operation.
      switch (operation)
	{
	case OperationDiscard:
	  {
	    //
	    // nothing to do since no modification has been performed.
	    //

	    break;
	  }
	case OperationStore:
	  {
	    //
	    // the target is to be stored; therefore, the previously
	    // transcripted actions must be cleared since no longer relevant.
	    //
	    // note however that should the current state be Destroyed---i.e
	    // a previous actor destroyed the context---, the Store
	    // operation would simply be ignored.

	    // check the current state and return if necessary.
	    if (this->context->state == Context::StateDestroyed)
	      leave();

	    // clear the transcript from about-to-be-pushed blocks.
	    this->context->transcript.Clear(Action::TypePush);

	    // XXX ici meme chose qu'en bas pour parer le scenar suivant:
	    // XXX un remove detruit le bloc de donnees. puis un autre
	    // XXX actor modifie les meta donnees par exemple et le store.
	    // XXX si on flush toutes les modifs precedentes, on va perdre
	    // XXX de vue le block qui a ete marque pour destruction.

	    printf("FLUSH 1\n");

	    break;
	  }
	case OperationDestroy:
	  {
	    //
	    // the target is to be destroyed; therefore, the previously
	    // transcripted actions must be flushed since no longer relevant.
	    //
	    // note that should have a previous Destroy operation been invoked,
	    // flushing the transcript would be needless; thus the flushing
	    // is triggered only if necessary.
	    //

	    // check the state.
	    if (this->context->state == Context::StateDestroyed)
	      leave();

	    // flush the transcript.
	    this->context->transcript.Clear(Action::TypePush);

	    // XXX ici flush que les blocs modifies puisqu'ils vont etre
	    // XXX detruits mais gardes les blocs deja detruits.
	    // XXX on evite les problemes du scenario: un actor remove
	    // XXX quelque chose -> destruction du block de donnees
	    // XXX puis l'objet est detruit et on flush tout en oubliant
	    // XXX le block deja marque pour deletion => il ne faut pas
	    // XXX flusher les blocks wiped.

	    printf("FLUSH 2\n");

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
