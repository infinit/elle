//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [fri jun 24 14:23:50 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/journal/Journal.hh>

#include <etoile/gear/Transcript.hh>
#include <etoile/gear/Action.hh>

#include <etoile/depot/Depot.hh>

#include <Infinit.hh>

namespace etoile
{
  namespace journal
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method records a given scope so as to trigger the action
    /// later on.
    ///
    /// XXX the scope is orphan at this point!
    ///
    elle::Status	Journal::Record(gear::Scope*		scope)
    {
      gear::Transcript::Scoutor		scoutor;

      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
	printf("[etoile] journal::Journal::Record()\n");

      // set the context's state.
      scope->context->state = gear::Context::StateJournaled;

      // XXX iterer d'abord sur les ajouts (chb puis pkb),
      // XXX puis sur les deletions
      // XXX en fait c'est deja bon je pense!

      // go through the transcript's actions.
      for (scoutor = scope->context->transcript.container.begin();
	   scoutor != scope->context->transcript.container.end();
	   scoutor++)
	{
	  gear::Action*			action = *scoutor;

	  // perform the action.
	  switch (action->type)
	    {
	    case gear::Action::TypePush:
	      {
		// store the block in the depot.
		if (depot::Depot::Push(action->address,
				       *action->block) == elle::StatusError)
		  escape("unable to push the block in the depot");

		break;
	      }
	    case gear::Action::TypeWipe:
	      {
		// wipe the block from the depot.
		if (depot::Depot::Wipe(action->address) == elle::StatusError)
		  escape("unable to wipe the block from the depot");

		break;
	      }
	    case gear::Action::TypeUnknown:
	      {
		escape("unknown action type");
	      }
	    }
	}

      // flush the transcript since the actions have been performed.
      if (scope->context->transcript.Flush() == elle::StatusError)
	escape("unable to clear the transcript");

      // set the context's state.
      scope->context->state = gear::Context::StateCleaned;

      // bury the scope i.e a scope may have recorded itself; thus bury
      // it in order to avoid problems.
      bury(scope);

      leave();
    }

  }
}
