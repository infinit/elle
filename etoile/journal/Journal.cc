#include <etoile/journal/Journal.hh>
#include <etoile/depot/Depot.hh>
#include <etoile/gear/Scope.hh>
#include <etoile/gear/Transcript.hh>
#include <etoile/gear/Action.hh>

#include <elle/concurrency/Scheduler.hh>
#include <elle/log.hh>

#include <nucleus/factory.hh>

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
    /*------------------.
    | Static Attributes |
    `------------------*/

    std::set<gear::Transcript*> Journal::_queue;

    /*---------------.
    | Static Methods |
    `---------------*/

    elle::Status
    Journal::Record(gear::Scope*            scope)
    {
      ELLE_TRACE_SCOPE("Journal::Record(%s)", *scope);

      ELLE_FINALLY_ACTION_DELETE(scope);

      // Ignore empty scope' transcripts.
      if (scope->context->transcript().empty() == true)
        {
          ELLE_TRACE("ignore this scope because it has an empty transcript");

          return elle::Status::Ok;
        }

      // Retrieve the transcript from the context.
      gear::Transcript* transcript = scope->context->cede();

      try
       {
         // Insert the transcript in the journal's queue.
         Journal::_queue.insert(transcript);

         // Spawn a thread and do not wait for it to complete since
         // we want the processing to occur in the background as it
         // may take some time.
         new reactor::Thread(elle::concurrency::scheduler(),
                             "journal process",
                             boost::bind(&Journal::_process, transcript),
                             true);

       }
      catch (std::exception const& err)
        {
          // Remove the transcript since something went wrong.
          Journal::_queue.erase(transcript);

          delete transcript;

          escape("unable to spawn a new thread; reason: '%s'", err.what());
        }

      // Update the context's state.
      scope->context->state = gear::Context::StateJournaled;

      ELLE_FINALLY_ABORT(scope);

      // Finally, delete the scope.
      delete scope;

      return elle::Status::Ok;
    }

    std::unique_ptr<nucleus::proton::Block>
    Journal::retrieve(nucleus::proton::Address const& address,
                      nucleus::proton::Revision const& revision)
    {
      ELLE_TRACE_FUNCTION(address, revision);

      for (auto transcript: Journal::_queue)
        {
          ELLE_TRACE_SCOPE("exploring transcript %s", *transcript);

          for (auto action: *transcript)
            {
              switch (action->type())
                {
                case gear::Action::Type::push:
                  {
                    ELLE_ASSERT(
                      dynamic_cast<gear::action::Push const*>(action) !=
                      nullptr);
                    auto _action =
                      static_cast<gear::action::Push const*>(action);

                    // Ignore non-matching addresses.
                    if (_action->address() != address)
                      continue;

                    if (revision == nucleus::proton::Revision::Any)
                      {
                        ELLE_TRACE("cloning the block associated with the "
                                   "action %s", *_action);

                        return (Journal::_clone(address.component(),
                                                _action->block()));
                      }
                    else
                      {
                        auto _block =
                          dynamic_cast<nucleus::proton::MutableBlock const*>(
                            &_action->block());

                        // Ignore non-mutable-blocks and non-matching revisions.
                        if ((_block == nullptr) ||
                            (_block->revision() != revision))
                          continue;

                        ELLE_TRACE("cloning the mutable block associated "
                                   "with the action %s", *_action);

                        return (Journal::_clone(address.component(),
                                                _action->block()));
                      }

                    break;
                  }
                case gear::Action::Type::wipe:
                  {
                    ELLE_ASSERT(
                      dynamic_cast<gear::action::Wipe const*>(action) !=
                      nullptr);
                    auto _action =
                      static_cast<gear::action::Wipe const*>(action);

                    // Ignore non-matching addresses.
                    if (_action->address() != address)
                      continue;

                    // If the requested block is about to be wiped,
                    // throw an error.
                    throw elle::Exception("this block has been scheduled "
                                          "for deletion");
                  }
                }
            }
        }

      ELLE_TRACE("the requested block is not present in the journal");

      return (std::unique_ptr<nucleus::proton::Block>(nullptr));
    }

    void
    Journal::_process(gear::Transcript* transcript)
    {
      ELLE_TRACE_FUNCTION(transcript);

      ELLE_TRACE("pushing the blocks")
        {
          // XXX[to improve in the future]
          // Go through the blocks which needs to be pushed.
          for (auto action: *transcript)
            {
              switch (action->type())
                {
                case gear::Action::Type::push:
                  {
                    action->apply<depot::Depot>();
                    break;
                  }
                case gear::Action::Type::wipe:
                  break;
                }
            }
        }

      ELLE_TRACE("wiping the blocks")
        {
          // Then, process the blocks to wipe.
          for (auto action: *transcript)
            {
              switch (action->type())
                {
                case gear::Action::Type::push:
                  break;
                case gear::Action::Type::wipe:
                  {
                    action->apply<depot::Depot>();
                    break;
                  }
                }
            }
        }

      // Remove the transcript from the queue.
      Journal::_queue.erase(transcript);

      delete transcript;
    }

    std::unique_ptr<nucleus::proton::Block>
    Journal::_clone(nucleus::neutron::Component const component,
                    nucleus::proton::Block const& block)
    {
      ELLE_TRACE_FUNCTION(component, block);

      // XXX[the method below is temporary since expensive:
      //     the block is serialized and deserialized instead
      //     of copying the block]

      std::stringstream stream(std::ios_base::in |
                               std::ios_base::out |
                               std::ios_base::binary);

      // Serialize the block.
      // XXX[to improve: contact Raphael or cf. hole/storage/]
      static_cast<
        elle::serialize::Serializable<
          elle::serialize::BinaryArchive>
        const&>(block).serialize(stream);

      // Allocate a new block.
      nucleus::proton::Block* _block{
        nucleus::factory::block().allocate<
          nucleus::proton::ImmutableBlock>(component)};

      ELLE_FINALLY_ACTION_DELETE(_block);

      // Deserialize the archive into a new block.
      // XXX[to improve: contact Raphael or cf. hole/storage/]
      static_cast<
        elle::serialize::Serializable<
          elle::serialize::BinaryArchive>
        *>(_block)->deserialize(stream);

      ELLE_FINALLY_ABORT(_block);

      return (std::unique_ptr<nucleus::proton::Block>(_block));
    }
  }
}
