#include <etoile/abstract/Group.hh>
#include <etoile/automaton/Group.hh>
#include <etoile/automaton/Rights.hh>
#include <etoile/gear/Group.hh>
#include <etoile/gear/Guard.hh>
#include <etoile/gear/Operation.hh>
#include <etoile/gear/Scope.hh>
#include <etoile/journal/Journal.hh>
#include <etoile/wall/Group.hh>

#include <nucleus/proton/Revision.hh>
#include <nucleus/neutron/Index.hh>
#include <nucleus/neutron/Size.hh>
#include <nucleus/neutron/Range.hh>
#include <nucleus/neutron/Fellow.hh>

#include <elle/log.hh>
#include <elle/concurrency/Scheduler.hh>

#include <Infinit.hh>

ELLE_LOG_COMPONENT("infinit.etoile.wall.Group");

namespace etoile
{
  namespace wall
  {

//
// ---------- methods ---------------------------------------------------------
//

    std::pair<nucleus::neutron::Group::Identity, gear::Identifier>
    Group::Create(elle::String const& description)
    {
      ELLE_TRACE_FUNCTION(description);

      gear::Scope* scope;
      gear::Group* context;

      std::pair<nucleus::neutron::Group::Identity, gear::Identifier> res;

      // acquire the scope.
      if (gear::Scope::Supply(scope) == elle::Status::Error)
        throw reactor::Exception("unable to supply the scope");

      gear::Guard guard(scope);

      // Declare a critical section.
      {
        reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex.write());

        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          throw reactor::Exception("unable to retrieve the context");

        guard.actor(new gear::Actor(scope));

        // return the identifier.
        res.second = guard.actor()->identifier;

        // apply the create automaton on the context.
        if (automaton::Group::Create(*context,
                                     description,
                                     res.first) == elle::Status::Error)
          throw reactor::Exception("unable to create the group");

        guard.actor()->state = gear::Actor::StateUpdated;

        if (guard.Release() == elle::Status::Error)
          throw reactor::Exception("unable to release the guard");
      }
      return res;
    }

    gear::Identifier
    Group::Load(typename nucleus::neutron::Group::Identity const& identity)
    {
      ELLE_TRACE_FUNCTION(identity);

      gear::Scope* scope;
      gear::Group* context;

      // XXX[change this so as to scope the groups i.e in order for the groups
      //     to be opened by multiple actors concurrently]
      if (gear::Scope::Supply(scope) == elle::Status::Error)
        throw reactor::Exception("unable to supplyire the scope");

      gear::Guard guard(scope);

      // Declare a critical section.
      {
        reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex.write());

        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          throw reactor::Exception("unable to retrieve the context");

        guard.actor(new gear::Actor(scope));

        // return the identifier.
        gear::Identifier identifier = guard.actor()->identifier;

        context->location =
          nucleus::proton::Location(identity,
                                    nucleus::proton::Revision::Last);

        // apply the load automaton on the context.
        if (automaton::Group::Load(*context) == elle::Status::Error)
          throw reactor::Exception("unable to load the group");

        // waive the actor and the scope.
        if (guard.Release() == elle::Status::Error)
          throw reactor::Exception("unable to release the guard");

        return identifier;
      }
    }

    abstract::Group
    Group::Information(gear::Identifier const& identifier)
    {
      ELLE_TRACE_FUNCTION(identifier);

      gear::Actor* actor;
      gear::Scope* scope;
      gear::Group* context;

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        throw reactor::Exception("unable to select the actor");

      scope = actor->scope;

      // Declare a critical section.
      {
        reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex);

        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          throw reactor::Exception("unable to retrieve the context");

        // apply the information automaton on the context.
        abstract::Group abstract;
        if (automaton::Group::Information(*context,
                                          abstract) == elle::Status::Error)
          throw reactor::Exception("unable to retrieve general information on the group");
        return abstract;
      }
    }

    void
    Group::Add(gear::Identifier const& identifier,
               nucleus::neutron::Subject const& subject)
    {
      ELLE_TRACE_FUNCTION(identifier, subject);

      gear::Actor* actor;
      gear::Scope* scope;
      gear::Group* context;

      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        throw reactor::Exception("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // Declare a critical section.
      {
        reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex.write());

        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          throw reactor::Exception("unable to retrieve the context");

        // apply the add automaton on the context.
        if (automaton::Group::Add(*context, subject) == elle::Status::Error)
          throw reactor::Exception("unable to add the subject to the group");

        // set the actor's state.
        actor->state = gear::Actor::StateUpdated;
      }
    }

    nucleus::neutron::Fellow
    Group::Lookup(gear::Identifier const& identifier,
                  nucleus::neutron::Subject const& subject)
    {
      ELLE_TRACE_FUNCTION(identifier, subject);

      gear::Actor* actor;
      gear::Scope* scope;
      gear::Group* context;

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        throw reactor::Exception("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // Declare a critical section.
      {
        reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex);

        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          throw reactor::Exception("unable to retrieve the context");

        // apply the lookup automaton on the context.
        const nucleus::neutron::Fellow* fellow = nullptr;
        if (automaton::Group::Lookup(*context,
                                     subject,
                                     fellow) == elle::Status::Error)
          throw reactor::Exception("unable to lookup the subject in the group");
        if (fellow)
          return *fellow;
        else
          return nucleus::neutron::Fellow::null();
      }
    }

    nucleus::neutron::Range<nucleus::neutron::Fellow>
    Group::Consult(gear::Identifier const& identifier,
                   nucleus::neutron::Index const& index,
                   nucleus::neutron::Size const& size)
    {
      ELLE_TRACE_FUNCTION(identifier, index, size);

      gear::Actor* actor;
      gear::Scope* scope;
      gear::Group* context;

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        throw reactor::Exception("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // Declare a critical section.
      {
        reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex);

        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          throw reactor::Exception("unable to retrieve the context");

        // apply the consult automaton on the context.
        nucleus::neutron::Range<nucleus::neutron::Fellow> range;
        if (automaton::Group::Consult(*context,
                                      index,
                                      size,
                                      range) == elle::Status::Error)
          throw reactor::Exception("unable to consult the group");
        return range;
      }
    }

    void
    Group::Remove(gear::Identifier const& identifier,
                  nucleus::neutron::Subject const& subject)
    {
      ELLE_TRACE_FUNCTION(identifier, subject);

      gear::Actor* actor;
      gear::Scope* scope;
      gear::Group* context;

      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        throw reactor::Exception("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // Declare a critical section.
      {
        reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex.write());

        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          throw reactor::Exception("unable to retrieve the context");

        // apply the remove automaton on the context.
        if (automaton::Group::Remove(*context, subject) == elle::Status::Error)
          throw reactor::Exception("unable to remove the subject from the group");

        // set the actor's state.
        actor->state = gear::Actor::StateUpdated;
      }
    }

    void
    Group::Discard(gear::Identifier const& identifier)
    {
      ELLE_TRACE_FUNCTION(identifier);

      gear::Actor* actor;
      gear::Scope* scope;
      gear::Group* context;

      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        throw reactor::Exception("unable to select the actor");

      gear::Guard guard(actor);

      // retrieve the scope.
      scope = actor->scope;

      // Declare a critical section.
      {
        reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex.write());

        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          throw reactor::Exception("unable to retrieve the context");

        // check the permissions before performing the operation in
        // order not to alter the scope should the operation not be
        // allowed.
        if (automaton::Rights::Operate(
              *context,
              gear::OperationDiscard) == elle::Status::Error)
          throw reactor::Exception("the user does not seem to have the necessary permission for "
                                   "discarding this group");

        // specify the closing operation performed by the actor.
        if (actor->Operate(gear::OperationDiscard) == elle::Status::Error)
          throw reactor::Exception("this operation cannot be performed by this actor");

        // delete the actor.
        guard.actor(nullptr);

        // specify the closing operation performed on the scope.
        if (scope->Operate(gear::OperationDiscard) == elle::Status::Error)
          throw reactor::Exception("unable to specify the operation being performed "
                                   "on the scope");

        // trigger the shutdown.
        try
          {
            if (scope->Shutdown() == elle::Status::Error)
              throw elle::Exception("unable to trigger the shutdown");
          }
        catch (elle::Exception const& e)
          {
            ELLE_ERR("unable to shutdown the scope: '%s'", e.what());
            return;
          }
      }

      // depending on the context's state.
      switch (context->state)
        {
          case gear::Context::StateDiscarded:
          case gear::Context::StateStored:
          case gear::Context::StateDestroyed:
          {
            //
            // if the group has been sealed, i.e there is no more actor
            // operating on it, record it in the journal.
            //

            // relinquish the scope: at this point we know there is no
            // remaining actor.
            if (gear::Scope::Relinquish(scope) == elle::Status::Error)
              throw reactor::Exception("unable to relinquish the scope");

            // record the scope in the journal.
            if (journal::Journal::Record(scope) == elle::Status::Error)
              throw reactor::Exception("unable to record the scope in the journal");

            break;
          }
          default:
          {
            //
            // otherwise, some actors are probably still working on it.
            //

            break;
          }
        }
    }

    void
    Group::Store(gear::Identifier const& identifier)
    {
      ELLE_TRACE_FUNCTION(identifier);

      gear::Actor* actor;
      gear::Scope* scope;
      gear::Group* context;

      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        throw reactor::Exception("unable to select the actor");

      gear::Guard guard(actor);

      // retrieve the scope.
      scope = actor->scope;

      // Declare a critical section.
      {
        reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex.write());

        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          throw reactor::Exception("unable to retrieve the context");

        // check the permissions before performing the operation in
        // order not to alter the scope should the operation not be
        // allowed.
        if (automaton::Rights::Operate(
              *context,
              gear::OperationStore) == elle::Status::Error)
          throw reactor::Exception("the user does not seem to have the necessary permission for "
                                   "storing this group");

        // specify the closing operation performed by the actor.
        if (actor->Operate(gear::OperationStore) == elle::Status::Error)
          throw reactor::Exception("this operation cannot be performed by this actor");

        // delete the actor.
        guard.actor(nullptr);

        // specify the closing operation performed on the scope.
        if (scope->Operate(gear::OperationStore) == elle::Status::Error)
          throw reactor::Exception("unable to specify the operation being performed "
                                   "on the scope");

        // trigger the shutdown.
        try
          {
            if (scope->Shutdown() == elle::Status::Error)
              throw elle::Exception("unable to trigger the shutdown");
          }
        catch (elle::Exception const& e)
          {
            ELLE_ERR("unable to shutdown the scope: '%s'", e.what());
            return;
          }
      }

      // depending on the context's state.
      switch (context->state)
        {
          case gear::Context::StateDiscarded:
          case gear::Context::StateStored:
          case gear::Context::StateDestroyed:
          {
            //
            // if the group has been sealed, i.e there is no more actor
            // operating on it, record it in the journal.
            //

            // relinquish the scope: at this point we know there is no
            // remaining actor.
            if (gear::Scope::Relinquish(scope) == elle::Status::Error)
              throw reactor::Exception("unable to relinquish the scope");

            // record the scope in the journal.
            if (journal::Journal::Record(scope) == elle::Status::Error)
              throw reactor::Exception("unable to record the scope in the journal");

            break;
          }
          default:
          {
            //
            // otherwise, some actors are probably still working on it.
            //

            break;
          }
        }
    }

    void
    Group::Destroy(gear::Identifier const& identifier)
    {
      ELLE_TRACE_FUNCTION(identifier);

      gear::Actor* actor;
      gear::Scope* scope;
      gear::Group* context;

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        throw reactor::Exception("unable to select the actor");

      gear::Guard guard(actor);

      // retrieve the scope.
      scope = actor->scope;

      // Declare a critical section.
      {
        reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex.write());

        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          throw reactor::Exception("unable to retrieve the context");

        // check the permissions before performing the operation in
        // order not to alter the scope should the operation not be
        // allowed.
        if (automaton::Rights::Operate(
              *context,
              gear::OperationDestroy) == elle::Status::Error)
          throw reactor::Exception("the user does not seem to have the necessary permission for "
                                   "destroying this group");

        // specify the closing operation performed by the actor.
        if (actor->Operate(gear::OperationDestroy) == elle::Status::Error)
          throw reactor::Exception("this operation cannot be performed by this actor");

        // delete the actor.
        guard.actor(nullptr);

        // specify the closing operation performed on the scope.
        if (scope->Operate(gear::OperationDestroy) == elle::Status::Error)
          throw reactor::Exception("unable to specify the operation being performed "
                                   "on the scope");

        // trigger the shutdown.
        try
          {
            if (scope->Shutdown() == elle::Status::Error)
              throw elle::Exception("unable to trigger the shutdown");
          }
        catch (elle::Exception const& e)
          {
            ELLE_ERR("unable to shutdown the scope: '%s'", e.what());
            return;
          }
      }

      // depending on the context's state.
      switch (context->state)
        {
          case gear::Context::StateDiscarded:
          case gear::Context::StateStored:
          case gear::Context::StateDestroyed:
          {
            //
            // if the group has been sealed, i.e there is no more actor
            // operating on it, record it in the journal.
            //

            // relinquish the scope: at this point we know there is no
            // remaining actor.
            if (gear::Scope::Relinquish(scope) == elle::Status::Error)
              throw reactor::Exception("unable to relinquish the scope");

            // record the scope in the journal.
            if (journal::Journal::Record(scope) == elle::Status::Error)
              throw reactor::Exception("unable to record the scope in the journal");

            break;
          }
          default:
          {
            //
            // otherwise, some actors are probably still working on it.
            //

            break;
          }
        }
    }
  }
}
