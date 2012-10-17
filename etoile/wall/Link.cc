#include <etoile/wall/Link.hh>
#include <etoile/gear/Identifier.hh>
#include <etoile/gear/Scope.hh>
#include <etoile/gear/Link.hh>
#include <etoile/gear/Operation.hh>
#include <etoile/gear/Guard.hh>
#include <etoile/automaton/Link.hh>
#include <etoile/automaton/Rights.hh>
#include <etoile/journal/Journal.hh>
#include <etoile/path/Way.hh>

#include <elle/log.hh>
#include <elle/concurrency/Scheduler.hh>

#include <Infinit.hh>

ELLE_LOG_COMPONENT("infinit.etoile.wall.Link");

namespace etoile
{
  namespace wall
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a new link object.
    ///
    /// note however that the object is not attached to the hierarchy
    /// and is therefore considered as orphan.
    ///
    elle::Status        Link::Create(
                          gear::Identifier&                     identifier)
    {
      gear::Scope*      scope;
      gear::Link*       context;

      ELLE_TRACE_FUNCTION("");

      // acquire the scope.
      if (gear::Scope::Supply(scope) == elle::Status::Error)
        escape("unable to supply the scope");

      gear::Guard               guard(scope);

      // Declare a critical section.
      {
        reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex.write());

        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          escape("unable to retrieve the context");

        // allocate an actor.
        guard.actor(new gear::Actor(scope));

        // return the identifier.
        identifier = guard.actor()->identifier;

        // apply the create automaton on the context.
        if (automaton::Link::Create(*context) == elle::Status::Error)
          escape("unable to create the link");

        // set the actor's state.
        guard.actor()->state = gear::Actor::StateUpdated;

        ELLE_DEBUG("returning identifier %s on %s", identifier, *scope);

        // waive the actor and the scope.
        if (guard.Release() == elle::Status::Error)
          escape("unable to release the guard");
      }

      ELLE_DEBUG("returning identifier %s", identifier);
      return elle::Status::Ok;
    }

    ///
    /// this method loads a link object given its chemin and initializes
    /// an associated context.
    ///
    elle::Status        Link::Load(
                          const path::Chemin&                   chemin,
                          gear::Identifier&                     identifier)
    {
      gear::Scope*      scope;
      gear::Link*       context;

      ELLE_TRACE_FUNCTION(chemin);

      // acquire the scope.
      if (gear::Scope::Acquire(chemin, scope) == elle::Status::Error)
        escape("unable to acquire the scope");

      gear::Guard               guard(scope);

      // Declare a critical section.
      {
        reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex.write());

        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          escape("unable to retrieve the context");

        // allocate an actor.
        guard.actor(new gear::Actor(scope));

        // return the identifier.
        identifier = guard.actor()->identifier;

        // locate the object based on the chemin.
        if (chemin.Locate(context->location) == elle::Status::Error)
          escape("unable to locate the link");

        try
          {
            // apply the load automaton on the context.
            if (automaton::Link::Load(*context) == elle::Status::Error)
              escape("unable to load the link");
          }
        catch (std::exception const&)
          {
            assert(scope != nullptr);
            Object::reload<gear::Link>(*scope);
          }

        ELLE_DEBUG("returning identifier %s on %s", identifier, *scope);

        // waive the actor and the scope
        if (guard.Release() == elle::Status::Error)
          escape("unable to release the guard");
      }

      return elle::Status::Ok;
    }

    ///
    /// this application locks the given link.
    ///
    /// the method returns true if the lock has been acquired or false
    /// otherwise.
    ///
    elle::Status        Link::Lock(
                          const gear::Identifier&)
    {
      ELLE_TRACE_SCOPE("Lock()");

      // XXX to implement.

      return elle::Status::Ok;
    }

    ///
    /// this method releases a previously locked link.
    ///
    elle::Status        Link::Release(
                          const gear::Identifier&)
    {
      ELLE_TRACE_SCOPE("Release()");

      // XXX to implement.

      return elle::Status::Ok;
    }

    ///
    /// this method binds a new target way to the given object.
    ///
    elle::Status        Link::Bind(
                          const gear::Identifier&               identifier,
                          const path::Way&                      way)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Link*       context;

      ELLE_TRACE_SCOPE("Bind(%s, %s)", identifier, way);

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // Declare a critical section.
      {
        reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex.write());

        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          escape("unable to retrieve the context");

        // apply the bind automaton on the context.
        if (automaton::Link::Bind(*context,
                                  way) == elle::Status::Error)
          escape("unable to bind the link");

        // set the actor's state.
        actor->state = gear::Actor::StateUpdated;
      }

      return elle::Status::Ok;
    }

    ///
    /// this method returns the way pointed by this link.
    ///
    elle::Status        Link::Resolve(
                          const gear::Identifier&               identifier,
                          path::Way&                            way)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Link*       context;

      ELLE_TRACE_SCOPE("Resolve()");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // Declare a critical section.
      {
        reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex);

        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          escape("unable to retrieve the context");

        // apply the resolve automaton on the context.
        if (automaton::Link::Resolve(*context,
                                     way) == elle::Status::Error)
          escape("unable to resolve the link");
      }

      return elle::Status::Ok;
    }

    ///
    /// this method discards the scope, potentially ignoring the
    /// performed modifications.
    ///
    elle::Status        Link::Discard(
                          const gear::Identifier&               identifier)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Link*       context;

      ELLE_TRACE_SCOPE("Discard()");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        escape("unable to select the actor");

      gear::Guard               guard(actor);

      // retrieve the scope.
      scope = actor->scope;

      // Declare a critical section.
      {
        reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex.write());

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
                 "discarding this link");

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
            // if the link has been sealed, i.e there is no more actor
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

    void
    Link::store(gear::Identifier const& identifier)
    {
      gear::Actor* actor;
      gear::Scope* scope;
      gear::Link* context;

      ELLE_TRACE_SCOPE("Store()");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        escape("unable to select the actor");

      gear::Guard guard(actor);

      // retrieve the scope.
      scope = actor->scope;

      // Declare a critical section.
      {
        reactor::Lock lock(elle::concurrency::scheduler(),
                           scope->mutex.write());

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
                 "storing this link");

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
            // if the link has been sealed, i.e there is no more actor
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
    }

    ///
    /// this method destroys a link.
    ///
    elle::Status        Link::Destroy(
                          const gear::Identifier&               identifier)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Link*       context;

      ELLE_TRACE_SCOPE("Destroy()");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        escape("unable to select the actor");

      gear::Guard               guard(actor);

      // retrieve the scope.
      scope = actor->scope;

      // Declare a critical section.
      {
        reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex.write());

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
                 "destroying this link");

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
            // if the link has been sealed, i.e there is no more actor
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

    ///
    /// this method purges a link i.e removes all the blocks of all
    /// the revisions associated with this link.
    ///
    elle::Status        Link::Purge(
                          const gear::Identifier&)
    {
      ELLE_TRACE_SCOPE("Purge()");

      // XXX to implement.

      return elle::Status::Ok;
    }

  }
}
