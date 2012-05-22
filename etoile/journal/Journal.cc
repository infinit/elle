
#include <cassert>
#include <stdexcept>

#include <etoile/journal/Journal.hh>

#include <nucleus/Nucleus.hh>

#include <etoile/depot/Depot.hh>

#include <elle/concurrency/Scheduler.hh>

#include <Infinit.hh>

namespace etoile
{
  namespace journal
  {
    std::set<gear::Scope*> Journal::_scopes;

    ///
    /// this method records a given scope so as to trigger the action
    /// later on.
    ///
    /// XXX the scope is orphan at this point!
    ///
    elle::Status        Journal::Record(gear::Scope*            scope)
    {
      assert(scope != nullptr);
      return Journal::_Record(scope);
      //try
      //  {
      //    Journal::_scopes.insert(scope);
      //    elle::concurrency::scheduler().mt_run<int>(
      //        __FUNCTION__,
      //        boost::bind(&Journal::_Record, scope)
      //    );
      //  }
      //catch (std::exception const& err)
      //  {
      //    Journal::_scopes.erase(scope);
      //    escape("Cannot spawn a new fiber: %s", err.what());
      //  }
      return elle::StatusOk;
    }

    elle::Status        Journal::_Record(gear::Scope*            scope)
    {
      nucleus::Transcript::Scoutor      scoutor;

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
          nucleus::Action*              action = *scoutor;

          // perform the action.
          switch (action->type)
            {
            case nucleus::Action::TypePush:
              {
                // store the block in the depot.
                if (depot::Depot::Push(action->address,
                                       *action->block) == elle::StatusError)
                  escape("unable to push the block in the depot");

                break;
              }
            case nucleus::Action::TypeWipe:
              {
                // wipe the block from the depot.
                if (depot::Depot::Wipe(action->address) == elle::StatusError)
                  escape("unable to wipe the block from the depot");

                break;
              }
            case nucleus::Action::TypeUnknown:
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

      Journal::_scopes.erase(scope);

      // bury the scope i.e a scope may have recorded itself; thus bury
      // it in order to avoid problems.
      bury(scope);

      return elle::StatusOk;
    }

    elle::Boolean Journal::get_block(nucleus::proton::Address const& address,
                                     nucleus::proton::Version const& version,
                                     nucleus::proton::Block& out_block)
    {
      foreach (gear::Scope* scope, Journal::_scopes)
        foreach (nucleus::Action* action, scope->context->transcript.container)
          {
            if (address != action->address)
              continue;

            if (version == nucleus::proton::Version::Any)
              {
                if (action->type == nucleus::Action::TypeWipe)
                  {
                    throw std::runtime_error("Block scheduled for deletion");
                  }
                out_block = *action->block;
                return true;
              }
            else
              {
                switch (address.family)
                  {
                  case nucleus::FamilyContentHashBlock:
                    throw std::runtime_error("version should be any for an immutable block");
                  case nucleus::FamilyPublicKeyBlock:
                  case nucleus::FamilyOwnerKeyBlock:
                  case nucleus::FamilyImprintBlock:
                    out_block = *action->block;
                    return true;
                  default:
                    throw std::runtime_error("Unkwown address family");
                  }
              }
          }
      return false;
    }

  }
}
