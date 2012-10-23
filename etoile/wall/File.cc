#include <etoile/wall/File.hh>
#include <etoile/gear/Identifier.hh>
#include <etoile/gear/Scope.hh>
#include <etoile/gear/File.hh>
#include <etoile/gear/Operation.hh>
#include <etoile/gear/Guard.hh>
#include <etoile/automaton/File.hh>
#include <etoile/automaton/Rights.hh>
#include <etoile/journal/Journal.hh>

#include <nucleus/neutron/Offset.hh>
#include <nucleus/neutron/Size.hh>

#include <elle/log.hh>
#include <elle/concurrency/Scheduler.hh>

#include <Infinit.hh>

ELLE_LOG_COMPONENT("infinit.etoile.wall.File");

namespace etoile
{
  namespace wall
  {

//
// ---------- methods ---------------------------------------------------------
//

    gear::Identifier
    File::create()
    {
      gear::Scope* scope;
      gear::File* context;

      ELLE_TRACE_FUNCTION("");

      // acquire the scope.
      if (gear::Scope::Supply(scope) == elle::Status::Error)
        escape("unable to supply the scope");

      gear::Guard guard(scope);
      gear::Identifier identifier;

      // Declare a critical section.
      {
        reactor::Lock lock(elle::concurrency::scheduler(),
                           scope->mutex.write());

        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          escape("unable to retrieve the context");

        // allocate an actor.
        guard.actor(new gear::Actor(scope));

        // return the identifier.
        identifier = guard.actor()->identifier;

        // apply the create automaton on the context.
        if (automaton::File::Create(*context) == elle::Status::Error)
          escape("unable to create the file");

        // set the actor's state.
        guard.actor()->state = gear::Actor::StateUpdated;

        ELLE_DEBUG("returning identifier %s on %s", identifier, *scope);

        // waive the scope.
        if (guard.Release() == elle::Status::Error)
          escape("unable to release the guard");
      }

      return (identifier);
    }

    ///
    /// this method loads a file object given its chemin and initializes
    /// an associated context.
    ///
    elle::Status        File::Load(
                          const path::Chemin&                   chemin,
                          gear::Identifier&                     identifier)
    {
      gear::Scope*      scope;
      gear::File*       context;

      ELLE_TRACE_FUNCTION(chemin);

      // acquire the scope.
      if (gear::Scope::Acquire(chemin, scope) == elle::Status::Error)
        escape("unable to acquire the scope");

      gear::Guard               guard(scope);

      // declare a critical section.
      {
        reactor::Lock lock(elle::concurrency::scheduler(),
                           scope->mutex.write());

        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          escape("unable to retrieve the context");

        // allocate an actor.
        guard.actor(new gear::Actor(scope));

        // return the identifier.
        identifier = guard.actor()->identifier;

        // locate the object based on the chemin.
        if (chemin.Locate(context->location) == elle::Status::Error)
          escape("unable to locate the file");

        try
          {
            // apply the load automaton on the context.
            if (automaton::File::Load(*context) == elle::Status::Error)
              escape("unable to load the file");
          }
        catch (std::exception const&)
          {
            assert(scope != nullptr);
            Object::reload<gear::File>(*scope);
          }

        ELLE_DEBUG("returning identifier %s on %s", identifier, *scope);

        // waive the actor and the scope.
        if (guard.Release() == elle::Status::Error)
          escape("unable to release the guard");
      }

      return elle::Status::Ok;
    }

    void
    File::write(gear::Identifier const& identifier,
                nucleus::neutron::Offset const& offset,
                elle::standalone::Region const& data)
    {
      gear::Actor* actor;
      gear::Scope* scope;
      gear::File* context;

      ELLE_TRACE_FUNCTION(identifier, offset, data);

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      {
        reactor::Lock lock(elle::concurrency::scheduler(),
                           scope->mutex.write());

        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          escape("unable to retrieve the context");

        // apply the write automaton on the context.
        if (automaton::File::Write(*context,
                                   offset,
                                   data) == elle::Status::Error)
          escape("unable to write the file");

        // set the actor's state.
        actor->state = gear::Actor::StateUpdated;
      }
    }

    elle::standalone::Region
    File::read(gear::Identifier const& identifier,
               nucleus::neutron::Offset const& offset,
               nucleus::neutron::Size const& size)
    {
      gear::Actor* actor;
      gear::Scope* scope;
      gear::File* context;

      ELLE_TRACE_FUNCTION(identifier, offset, size);

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      elle::standalone::Region data;

      // declare a critical section.
      {
        reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex);

        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          escape("unable to retrieve the context");

        // apply the read automaton on the context.
        if (automaton::File::Read(*context,
                                  offset,
                                  size,
                                  data) == elle::Status::Error)
          escape("unable to read the file");
      }

      return (data);
    }

    ///
    /// this method adjusts the size of a file.
    ///
    elle::Status        File::Adjust(
                          const gear::Identifier&               identifier,
                          const nucleus::neutron::Size& size)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::File*       context;

      ELLE_TRACE_FUNCTION(identifier, size);

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // Declare a critical section.
      {
        reactor::Lock lock(elle::concurrency::scheduler(),
                           scope->mutex.write());

        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          escape("unable to retrieve the context");

        // apply the adjust automaton on the context.
        if (automaton::File::Adjust(*context,
                                    size) == elle::Status::Error)
          escape("unable to adjust the file's size");

        // set the actor's state.
        actor->state = gear::Actor::StateUpdated;
      }

      return elle::Status::Ok;
    }

    ///
    /// this method discards the scope, potentially ignoring the
    /// performed modifications.
    ///
    elle::Status        File::Discard(
                          const gear::Identifier&               identifier)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::File*       context;

      ELLE_TRACE_FUNCTION(identifier);

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        escape("unable to select the actor");

      gear::Guard               guard(actor);

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
              gear::OperationDiscard) == elle::Status::Error)
          escape("the user does not seem to have the necessary permission for "
                 "discarding this file");

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
            // if the file has been sealed, i.e there is no more actor
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
    File::store(gear::Identifier const& identifier)
    {
      gear::Actor* actor;
      gear::Scope* scope;
      gear::File* context;

      ELLE_TRACE_FUNCTION(identifier);

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
                 "storing this file");

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
            // if the file has been sealed, i.e there is no more actor
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
    /// this method destroys a file.
    ///
    elle::Status        File::Destroy(
                          const gear::Identifier&               identifier)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::File*       context;

      ELLE_TRACE_FUNCTION(identifier);

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        escape("unable to select the actor");

      gear::Guard               guard(actor);

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
              gear::OperationDestroy) == elle::Status::Error)
          escape("the user does not seem to have the necessary permission for "
                 "destroying this file");

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
            // if the file has been sealed, i.e there is no more actor
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
    /// this method purges a file i.e removes all the blocks of all
    /// the revisions associated with this file.
    ///
    elle::Status        File::Purge(
                          const gear::Identifier&)
    {
      ELLE_TRACE_FUNCTION("");

      // XXX to implement.

      return elle::Status::Ok;
    }

  }
}
