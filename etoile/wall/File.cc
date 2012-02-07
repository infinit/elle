//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [fri aug 14 16:34:43 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/File.hh>

#include <etoile/gear/Identifier.hh>
#include <etoile/gear/Nature.hh>
#include <etoile/gear/Scope.hh>
#include <etoile/gear/File.hh>
#include <etoile/gear/Gear.hh>
#include <etoile/gear/ScopeGuard.hh>

#include <etoile/automaton/File.hh>
#include <etoile/automaton/Rights.hh>

#include <etoile/journal/Journal.hh>

#include <Infinit.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a new file object.
    ///
    /// note however that the object is not attached to the hierarchy
    /// and is therefore considered as orphan.
    ///
    elle::Status        File::Create(
                          gear::Identifier&                     identifier)
    {
      gear::Scope*      scope;
      gear::File*       context;

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] wall::File::Create()\n");

      // acquire the scope.
      if (gear::Scope::Supply(scope) == elle::StatusError)
        escape("unable to supply the scope");

      gear::ScopeGuard guard(scope);

      // declare a critical section.
      elle::Hurdle::Zone        zone(scope->hurdle, elle::ModeWrite);

      // protect the access.
      zone.Lock();
      {
        // retrieve the context.
        if (scope->Use(context) == elle::StatusError)
          escape("unable to retrieve the context");

        // allocate an actor.
        auto actor = new gear::Actor(scope);
        guard.actor(actor);

        // return the identifier.
        identifier = actor->identifier;

        // apply the create automaton on the context.
        if (automaton::File::Create(*context) == elle::StatusError)
          escape("unable to create the file");

        // set the actor's state.
        actor->state = gear::Actor::StateUpdated;

        // waive the scope.
        guard.release();
      }
      zone.Unlock();

      return elle::StatusOk;
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

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] wall::File::Load()\n");

      // acquire the scope.
      if (gear::Scope::Acquire(chemin, scope) == elle::StatusError)
        escape("unable to acquire the scope");

      gear::ScopeGuard guard(scope);

      // declare a critical section.
      elle::Hurdle::Zone        zone(scope->hurdle, elle::ModeWrite);

      // protect the access.
      zone.Lock();
      {
        // retrieve the context.
        if (scope->Use(context) == elle::StatusError)
          escape("unable to retrieve the context");

        // allocate an actor.
        guard.actor(new gear::Actor(scope));

        // return the identifier.
        identifier = guard.actor()->identifier;

        // locate the object based on the chemin.
        if (chemin.Locate(context->location) == elle::StatusError)
          escape("unable to locate the file");

        // apply the load automaton on the context.
        if (automaton::File::Load(*context) == elle::StatusError)
          escape("unable to load the file");

        // waive the actor and the scope.
        guard.release();
      }
      zone.Unlock();

      return elle::StatusOk;
    }

    ///
    /// this application locks the given file.
    ///
    /// the method returns true if the lock has been acquired or false
    /// otherwise.
    ///
    elle::Status        File::Lock(
                          const gear::Identifier&)
    {
      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] wall::File::Lock()\n");

      // XXX to implement.

      return elle::StatusOk;
    }

    ///
    /// this method releases a previously locked file.
    ///
    elle::Status        File::Release(
                          const gear::Identifier&)
    {
      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] wall::File::Release()\n");

      // XXX to implement.

      return elle::StatusOk;
    }

    ///
    /// this method writes the file with the given region of data.
    ///
    elle::Status        File::Write(
                          const gear::Identifier&               identifier,
                          const nucleus::Offset&                offset,
                          const elle::Region&                   region)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::File*       context;

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] wall::File::Write()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
        escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      elle::Hurdle::Zone        zone(scope->hurdle, elle::ModeWrite);

      // protect the access.
      zone.Lock();
      {
        // retrieve the context.
        if (scope->Use(context) == elle::StatusError)
          escape("unable to retrieve the context");

        // apply the write automaton on the context.
        if (automaton::File::Write(*context,
                                   offset,
                                   region) == elle::StatusError)
          escape("unable to write the file");

        // set the actor's state.
        actor->state = gear::Actor::StateUpdated;
      }
      zone.Unlock();

      return elle::StatusOk;
    }

    ///
    /// this method reads a region of the file.
    ///
    elle::Status        File::Read(
                          const gear::Identifier&               identifier,
                          const nucleus::Offset&                offset,
                          const nucleus::Size&                  size,
                          elle::Region&                         region)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::File*       context;

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] wall::File::Read()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
        escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      elle::Hurdle::Zone        zone(scope->hurdle, elle::ModeRead);

      // protect the access.
      zone.Lock();
      {
        // retrieve the context.
        if (scope->Use(context) == elle::StatusError)
          escape("unable to retrieve the context");

        // apply the read automaton on the context.
        if (automaton::File::Read(*context,
                                  offset,
                                  size,
                                  region) == elle::StatusError)
          escape("unable to read the file");
      }
      zone.Unlock();

      return elle::StatusOk;
    }

    ///
    /// this method adjusts the size of a file.
    ///
    elle::Status        File::Adjust(
                          const gear::Identifier&               identifier,
                          const nucleus::Size&                  size)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::File*       context;

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] wall::File::Adjust()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
        escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      elle::Hurdle::Zone        zone(scope->hurdle, elle::ModeWrite);

      // protect the access.
      zone.Lock();
      {
        // retrieve the context.
        if (scope->Use(context) == elle::StatusError)
          escape("unable to retrieve the context");

        // apply the adjust automaton on the context.
        if (automaton::File::Adjust(*context,
                                    size) == elle::StatusError)
          escape("unable to adjust the file's size");

        // set the actor's state.
        actor->state = gear::Actor::StateUpdated;
      }
      zone.Unlock();

      return elle::StatusOk;
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

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] wall::File::Discard()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
        escape("unable to select the actor");

      gear::ScopeGuard guard(actor);

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      elle::Hurdle::Zone        zone(scope->hurdle, elle::ModeWrite);

      // protect the access.
      zone.Lock();
      {
        // retrieve the context.
        if (scope->Use(context) == elle::StatusError)
          escape("unable to retrieve the context");

        // check the permissions before performing the operation in
        // order not to alter the scope should the operation not be
        // allowed.
        if (automaton::Rights::Operate(
              *context,
              gear::OperationDiscard) == elle::StatusError)
          escape("the user does not seem to have the necessary permission for "
                 "discarding this file");

        // specify the closing operation performed by the actor.
        if (actor->Operate(gear::OperationDiscard) == elle::StatusError)
          escape("this operation cannot be performed by this actor");

        // delete the actor.
        guard.actor(nullptr);

        // specify the closing operation performed on the scope.
        if (scope->Operate(gear::OperationDiscard) == elle::StatusError)
          escape("unable to specify the operation being performed "
                 "on the scope");

        // trigger the shutdown.
        if (scope->Shutdown() == elle::StatusError)
          escape("unable to trigger the shutdown");
      }
      zone.Unlock();

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
            if (gear::Scope::Relinquish(scope) == elle::StatusError)
              escape("unable to relinquish the scope");

            // record the scope in the journal.
            if (journal::Journal::Record(scope) == elle::StatusError)
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

      return elle::StatusOk;
    }

    ///
    /// this method closes the scope and places it in the journal for
    /// the modifications to be published in the storage layer.
    ///
    elle::Status        File::Store(
                          const gear::Identifier&               identifier)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::File*       context;

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] wall::File::Store()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
        escape("unable to select the actor");

      gear::ScopeGuard guard(actor);

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      elle::Hurdle::Zone        zone(scope->hurdle, elle::ModeWrite);

      // protect the access.
      zone.Lock();
      {
        // retrieve the context.
        if (scope->Use(context) == elle::StatusError)
          escape("unable to retrieve the context");

        // check the permissions before performing the operation in
        // order not to alter the scope should the operation not be
        // allowed.
        if (automaton::Rights::Operate(
              *context,
              gear::OperationStore) == elle::StatusError)
          escape("the user does not seem to have the necessary permission for "
                 "storing this file");

        // specify the closing operation performed by the actor.
        if (actor->Operate(gear::OperationStore) == elle::StatusError)
          escape("this operation cannot be performed by this actor");

        // delete the actor.
        guard.actor(nullptr);

        // specify the closing operation performed on the scope.
        if (scope->Operate(gear::OperationStore) == elle::StatusError)
          escape("unable to specify the operation being performed "
                 "on the scope");

        // trigger the shutdown.
        if (scope->Shutdown() == elle::StatusError)
          escape("unable to trigger the shutdown");
      }
      zone.Unlock();

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
            if (gear::Scope::Relinquish(scope) == elle::StatusError)
              escape("unable to relinquish the scope");

            // record the scope in the journal.
            if (journal::Journal::Record(scope) == elle::StatusError)
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

      return elle::StatusOk;
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

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] wall::File::Destroy()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
        escape("unable to select the actor");

      gear::ScopeGuard guard(actor);

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      elle::Hurdle::Zone        zone(scope->hurdle, elle::ModeWrite);

      // protect the access.
      zone.Lock();
      {
        // retrieve the context.
        if (scope->Use(context) == elle::StatusError)
          escape("unable to retrieve the context");

        // check the permissions before performing the operation in
        // order not to alter the scope should the operation not be
        // allowed.
        if (automaton::Rights::Operate(
              *context,
              gear::OperationDestroy) == elle::StatusError)
          escape("the user does not seem to have the necessary permission for "
                 "destroying this file");

        // specify the closing operation performed by the actor.
        if (actor->Operate(gear::OperationDestroy) == elle::StatusError)
          escape("this operation cannot be performed by this actor");

        // delete the actor.
        guard.actor(nullptr);

        // specify the closing operation performed on the scope.
        if (scope->Operate(gear::OperationDestroy) == elle::StatusError)
          escape("unable to specify the operation being performed "
                 "on the scope");

        // trigger the shutdown.
        if (scope->Shutdown() == elle::StatusError)
          escape("unable to trigger the shutdown");
      }
      zone.Unlock();

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
            if (gear::Scope::Relinquish(scope) == elle::StatusError)
              escape("unable to relinquish the scope");

            // record the scope in the journal.
            if (journal::Journal::Record(scope) == elle::StatusError)
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

      return elle::StatusOk;
    }

    ///
    /// this method purges a file i.e removes all the blocks of all
    /// the versions associated with this file.
    ///
    elle::Status        File::Purge(
                          const gear::Identifier&)
    {
      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] wall::File::Purge()\n");

      // XXX to implement.

      return elle::StatusOk;
    }

  }
}
