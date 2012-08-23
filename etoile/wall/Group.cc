#include <etoile/wall/Group.hh>
#include <etoile/gear/Identifier.hh>
#include <etoile/gear/Scope.hh>
#include <etoile/gear/Group.hh>
#include <etoile/gear/Operation.hh>
#include <etoile/gear/Guard.hh>
#include <etoile/automaton/Group.hh>
#include <etoile/automaton/Rights.hh>
#include <etoile/journal/Journal.hh>

#include <nucleus/proton/Version.hh>
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

    elle::Status
    Group::Create(elle::String const& description,
                  typename nucleus::neutron::Group::Identity& identity,
                  gear::Identifier& identifier)
    {
      gear::Scope* scope;
      gear::Group* context;

      ELLE_TRACE_SCOPE("Create()");

      // acquire the scope.
      if (gear::Scope::Supply(scope) == elle::Status::Error)
        escape("unable to supply the scope");

      gear::Guard guard(scope);

      // declare a critical section.
      reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex.write());
      {
        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          escape("unable to retrieve the context");

        guard.actor(new gear::Actor(scope));

        // return the identifier.
        identifier = guard.actor()->identifier;

        // apply the create automaton on the context.
        if (automaton::Group::Create(*context,
                                     description,
                                     identity) == elle::Status::Error)
          escape("unable to create the group");

        guard.actor()->state = gear::Actor::StateUpdated;

        if (guard.Release() == elle::Status::Error)
          escape("unable to release the guard");
      }

      return elle::Status::Ok;
    }

    elle::Status
    Group::Load(typename nucleus::neutron::Group::Identity const& identity,
                gear::Identifier& identifier)
    {
      gear::Scope* scope;
      gear::Group* context;

      ELLE_TRACE_SCOPE("Load()");

      // XXX[change this so as to scope the groups i.e in order for the groups
      //     to be opened by multiple actors concurrently]
      if (gear::Scope::Supply(scope) == elle::Status::Error)
        escape("unable to supplyire the scope");

      gear::Guard guard(scope);

      // declare a critical section.
      reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex.write());
      {
        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          escape("unable to retrieve the context");

        guard.actor(new gear::Actor(scope));

        // return the identifier.
        identifier = guard.actor()->identifier;

        if (context->location.Create(
              identity,
              nucleus::proton::Version::Last) == elle::Status::Error)
          escape("unable to create the location");

        // apply the load automaton on the context.
        if (automaton::Group::Load(*context) == elle::Status::Error)
          escape("unable to load the group");

        // waive the actor and the scope.
        if (guard.Release() == elle::Status::Error)
          escape("unable to release the guard");
      }

      return elle::Status::Ok;
    }

    elle::Status
    Group::Information(gear::Identifier const& identifier,
                       abstract::Group& abstract)
    {
      gear::Actor* actor;
      gear::Scope* scope;
      gear::Group* context;

      ELLE_TRACE("Information()");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        escape("unable to select the actor");

      scope = actor->scope;

      // declare a critical section.
      reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex);
      {
        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          escape("unable to retrieve the context");

        // apply the information automaton on the context.
        if (automaton::Group::Information(*context,
                                          abstract) == elle::Status::Error)
          escape("unable to retrieve general information on the group");
      }

      return elle::Status::Ok;
    }

    elle::Status
    Group::Add(gear::Identifier const& identifier,
               nucleus::neutron::Subject const& subject)
    {
      gear::Actor* actor;
      gear::Scope* scope;
      gear::Group* context;

      ELLE_TRACE_SCOPE("Add()");

      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex.write());
      {
        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          escape("unable to retrieve the context");

        // apply the add automaton on the context.
        if (automaton::Group::Add(*context, subject) == elle::Status::Error)
          escape("unable to add the subject to the group");

        // set the actor's state.
        actor->state = gear::Actor::StateUpdated;
      }

      return elle::Status::Ok;
    }

    elle::Status
    Group::Lookup(gear::Identifier const& identifier,
                  nucleus::neutron::Subject const& subject,
                  nucleus::neutron::Fellow const*& fellow)
    {
      gear::Actor* actor;
      gear::Scope* scope;
      gear::Group* context;

      ELLE_TRACE_SCOPE("Lookup()");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex);
      {
        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          escape("unable to retrieve the context");

        // apply the lookup automaton on the context.
        if (automaton::Group::Lookup(*context,
                                     subject,
                                     fellow) == elle::Status::Error)
          escape("unable to lookup the subject in the group");
      }

      return elle::Status::Ok;
    }

    elle::Status
    Group::Consult(gear::Identifier const& identifier,
                   nucleus::neutron::Index const& index,
                   nucleus::neutron::Size const& size,
                   nucleus::neutron::Range<nucleus::neutron::Fellow>& range)
    {
      gear::Actor* actor;
      gear::Scope* scope;
      gear::Group* context;

      ELLE_TRACE_SCOPE("Consult(%s, %s, %s, %s)",
                           identifier, index, size, range);

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex);
      {
        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          escape("unable to retrieve the context");

        // apply the consult automaton on the context.
        if (automaton::Group::Consult(*context,
                                      index,
                                      size,
                                      range) == elle::Status::Error)
          escape("unable to consult the group");
      }

      return elle::Status::Ok;
    }

    elle::Status
    Group::Remove(gear::Identifier const& identifier,
                  nucleus::neutron::Subject const& subject)
    {
      gear::Actor* actor;
      gear::Scope* scope;
      gear::Group* context;

      ELLE_TRACE_SCOPE("Remove()");

      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex.write());
      {
        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          escape("unable to retrieve the context");

        // apply the remove automaton on the context.
        if (automaton::Group::Remove(*context, subject) == elle::Status::Error)
          escape("unable to remove the subject from the group");

        // set the actor's state.
        actor->state = gear::Actor::StateUpdated;
      }

      return elle::Status::Ok;
    }

    elle::Status
    Group::Discard(gear::Identifier const& identifier)
    {
      gear::Actor* actor;
      gear::Scope* scope;
      gear::Group* context;

      ELLE_TRACE_SCOPE("Discard()");

      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        escape("unable to select the actor");

      gear::Guard guard(actor);

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex.write());
      {
        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          escape("unable to retrieve the context");

        // check the permissions before performing the operation in
        // order not to alter the scope should the operation not be
        // allowed.
        if (automaton::Rights::Operate(
              *context,
              gear::OperationDiscard) == elle::Status::Error)
          escape("the user does not seem to have the necessary permission for "
                 "discarding this group");

        // specify the closing operation performed by the actor.
        if (actor->Operate(gear::OperationDiscard) == elle::Status::Error)
          escape("this operation cannot be performed by this actor");

        // delete the actor.
        guard.actor(nullptr);

        // specify the closing operation performed on the scope.
        if (scope->Operate(gear::OperationDiscard) == elle::Status::Error)
          escape("unable to specify the operation being performed "
                 "on the scope");

        // trigger the shutdown.
        if (scope->Shutdown() == elle::Status::Error)
          escape("unable to trigger the shutdown");
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
              escape("unable to relinquish the scope");

            // record the scope in the journal.
            if (journal::Journal::Record(scope) == elle::Status::Error)
              escape("unable to record the scope in the journal");

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

      return elle::Status::Ok;
    }

    elle::Status
    Group::Store(gear::Identifier const& identifier)
    {
      gear::Actor* actor;
      gear::Scope* scope;
      gear::Group* context;

      ELLE_TRACE_SCOPE("Store()");

      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        escape("unable to select the actor");

      gear::Guard guard(actor);

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex.write());
      {
        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          escape("unable to retrieve the context");

        // check the permissions before performing the operation in
        // order not to alter the scope should the operation not be
        // allowed.
        if (automaton::Rights::Operate(
              *context,
              gear::OperationStore) == elle::Status::Error)
          escape("the user does not seem to have the necessary permission for "
                 "storing this group");

        // specify the closing operation performed by the actor.
        if (actor->Operate(gear::OperationStore) == elle::Status::Error)
          escape("this operation cannot be performed by this actor");

        // delete the actor.
        guard.actor(nullptr);

        // specify the closing operation performed on the scope.
        if (scope->Operate(gear::OperationStore) == elle::Status::Error)
          escape("unable to specify the operation being performed "
                 "on the scope");

        // trigger the shutdown.
        if (scope->Shutdown() == elle::Status::Error)
          escape("unable to trigger the shutdown");
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
              escape("unable to relinquish the scope");

            // record the scope in the journal.
            if (journal::Journal::Record(scope) == elle::Status::Error)
              escape("unable to record the scope in the journal");

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

      return elle::Status::Ok;
    }

    elle::Status
    Group::Destroy(gear::Identifier const& identifier)
    {
      gear::Actor* actor;
      gear::Scope* scope;
      gear::Group* context;

      ELLE_TRACE_SCOPE("Destroy()");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        escape("unable to select the actor");

      gear::Guard guard(actor);

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex.write());
      {
        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          escape("unable to retrieve the context");

        // check the permissions before performing the operation in
        // order not to alter the scope should the operation not be
        // allowed.
        if (automaton::Rights::Operate(
              *context,
              gear::OperationDestroy) == elle::Status::Error)
          escape("the user does not seem to have the necessary permission for "
                 "destroying this group");

        // specify the closing operation performed by the actor.
        if (actor->Operate(gear::OperationDestroy) == elle::Status::Error)
          escape("this operation cannot be performed by this actor");

        // delete the actor.
        guard.actor(nullptr);

        // specify the closing operation performed on the scope.
        if (scope->Operate(gear::OperationDestroy) == elle::Status::Error)
          escape("unable to specify the operation being performed "
                 "on the scope");

        // trigger the shutdown.
        if (scope->Shutdown() == elle::Status::Error)
          escape("unable to trigger the shutdown");
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
              escape("unable to relinquish the scope");

            // record the scope in the journal.
            if (journal::Journal::Record(scope) == elle::Status::Error)
              escape("unable to record the scope in the journal");

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

      return elle::Status::Ok;
    }

    elle::Status
    Group::Purge(gear::Identifier const&)
    {
      ELLE_TRACE_SCOPE("Purge()");

      // XXX to implement.

      return elle::Status::Ok;
    }

  }
}
