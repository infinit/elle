#include <elle/log.hh>

#include <etoile/journal/Journal.hh>
#include <etoile/depot/Depot.hh>
#include <etoile/gear/Scope.hh>

#include <nucleus/proton/Transcript.hh>
#include <nucleus/proton/Action.hh>

#include <elle/standalone/Log.hh>
#include <elle/standalone/Morgue.hh>
#include <elle/concurrency/Scheduler.hh>

#include <Infinit.hh>

#include <elle/idiom/Close.hh>
# include <cassert>
# include <stdexcept>
# include <boost/foreach.hpp>
#include <elle/idiom/Open.hh>

ELLE_LOG_COMPONENT("infinit.etoile.journal.Journal");

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
      ELLE_TRACE_SCOPE("Journal::Record(%s)", *scope);

      assert(scope != nullptr);
      return Journal::_Record(scope);
      // XXX
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
      return elle::Status::Ok;
    }

    elle::Status        Journal::_Record(gear::Scope*            scope)
    {
      nucleus::proton::Transcript::Scoutor      scoutor;

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] journal::Journal::Record()\n");

      // set the context's state.
      scope->context->state = gear::Context::StateJournaled;

      // XXX[to improve in the future]

      // Go through the blocks which needs to be pushed.
      for (scoutor = scope->context->transcript.container.begin();
           scoutor != scope->context->transcript.container.end();
           scoutor++)
        {
          nucleus::proton::Action* action = *scoutor;

          // perform the action.
          switch (action->type)
            {
            case nucleus::proton::Action::TypePush:
              {
                // store the block in the depot.
                if (depot::Depot::Push(action->address,
                                       *action->block) == elle::Status::Error)
                  escape("unable to push the block in the depot");

                break;
              }
            case nucleus::proton::Action::TypeWipe:
              {
                // Ignore these actions for now.

                break;
              }
            case nucleus::proton::Action::TypeUnknown:
              {
                escape("unknown action type");
              }
            }
        }

      // Then, process the blocks to wipe.
      for (scoutor = scope->context->transcript.container.begin();
           scoutor != scope->context->transcript.container.end();
           scoutor++)
        {
          nucleus::proton::Action* action = *scoutor;

          // perform the action.
          switch (action->type)
            {
            case nucleus::proton::Action::TypePush:
              {
                break;
              }
            case nucleus::proton::Action::TypeWipe:
              {
                // wipe the block from the depot.
                if (depot::Depot::Wipe(action->address) == elle::Status::Error)
                  escape("unable to wipe the block from the depot");

                break;
              }
            case nucleus::proton::Action::TypeUnknown:
              {
                escape("unknown action type");
              }
            }
        }

      // flush the transcript since the actions have been performed.
      if (scope->context->transcript.Flush() == elle::Status::Error)
        escape("unable to clear the transcript");

      // set the context's state.
      scope->context->state = gear::Context::StateCleaned;

      Journal::_scopes.erase(scope);

      return elle::Status::Ok;
    }

    elle::Boolean Journal::get_block(nucleus::proton::Address const& address,
                                     nucleus::proton::Version const& version,
                                     nucleus::proton::Block& out_block)
    {
      BOOST_FOREACH(gear::Scope* scope, Journal::_scopes)
        BOOST_FOREACH(nucleus::proton::Action* action, scope->context->transcript.container)
          {
            if (address != action->address)
              continue;

            if (version == nucleus::proton::Version::Any)
              {
                if (action->type == nucleus::proton::Action::TypeWipe)
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
                  case nucleus::proton::FamilyContentHashBlock:
                    throw std::runtime_error("version should be any for an immutable block");
                  case nucleus::proton::FamilyPublicKeyBlock:
                  case nucleus::proton::FamilyOwnerKeyBlock:
                  case nucleus::proton::FamilyImprintBlock:
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
