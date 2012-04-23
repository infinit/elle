//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [wed mar  3 20:50:57 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/Object.hh>

#include <etoile/gear/Identifier.hh>
#include <etoile/gear/Nature.hh>
#include <etoile/gear/Scope.hh>
#include <etoile/gear/Guard.hh>
#include <etoile/gear/Object.hh>
#include <etoile/gear/Gear.hh>

#include <etoile/automaton/Object.hh>
#include <etoile/automaton/Rights.hh>

#include <etoile/journal/Journal.hh>

#include <etoile/miscellaneous/Abstract.hh>

#include <Infinit.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method loads the object identified by _chemin_ into a new
    /// scope and returns the scope's identifier so that subsequent
    /// operations can be carried out.
    ///
    elle::Status        Object::Load(
                          const path::Chemin&                   chemin,
                          gear::Identifier&                     identifier)
    {
      gear::Scope*      scope;
      gear::Object*     context;

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] wall::Object::Load()\n");

      // acquire the scope.
      if (gear::Scope::Acquire(chemin, scope) == elle::Status::Error)
        escape("unable to acquire the scope");

      gear::Guard               guard(scope);

      // declare a critical section.
      reactor::Lock lock(&elle::concurrency::scheduler(), scope->mutex.write());
      {
        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          escape("unable to retrieve the context");

        // allocate an actor.
        guard.actor(new gear::Actor(scope));

        // return the identifier.
        identifier = guard.actor()->identifier;

        // locate the object based on the chemin.
        if (chemin.Locate(context->location) == elle::Status::Error)
          escape("unable to locate the object");

        // apply the load automaton on the context.
        if (automaton::Object::Load(*context) == elle::Status::Error)
          escape("unable to load the object");

        // waive the actor and the scope
        if (guard.Release() == elle::Status::Error)
          escape("unable to release the guard");
      }

      return elle::Status::Ok;
    }

    ///
    /// this method locks the object.
    ///
    /// the method returns true if the lock has been acquired, false
    /// otherwise.
    ///
    elle::Status        Object::Lock(
                          const gear::Identifier&)
    {
      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] wall::Object::Lock()\n");

      // XXX to implement.

      return elle::Status::Ok;
    }

    ///
    /// this method releases a previously locked object.
    ///
    elle::Status        Object::Release(
                          const gear::Identifier&)
    {
      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] wall::Object::Release()\n");

      // XXX to implement.

      return elle::Status::Ok;
    }

    ///
    /// this method returns general abstract regarding the identified
    /// object.
    ///
    elle::Status        Object::Information(
                          const gear::Identifier&               identifier,
                          miscellaneous::Abstract&              abstract)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Object*     context;

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] wall::Object::Information()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      reactor::Lock lock(&elle::concurrency::scheduler(), scope->mutex);
      {
        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          escape("unable to retrieve the context");

        // apply the information automaton on the context.
        if (automaton::Object::Information(*context,
                                           abstract) == elle::Status::Error)
          escape("unable to retrieve general information on the object");
      }

      return elle::Status::Ok;
    }

    ///
    /// this method discards the scope hence potentially ignoring some
    /// modifications.
    ///
    elle::Status        Object::Discard(
                          const gear::Identifier&               identifier)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Object*     context;

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] wall::Object::Discard()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        escape("unable to select the actor");

      gear::Guard               guard(actor);

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      reactor::Lock lock(&elle::concurrency::scheduler(), scope->mutex.write());
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
                 "discarding this object");

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
            // if the object has been sealed, i.e there is no more actor
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
    /// this method commits the pending modifications by placing the
    /// scope in the journal.
    ///
    elle::Status        Object::Store(
                          const gear::Identifier&               identifier)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Object*     context;

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] wall::Object::Store()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        escape("unable to select the actor");

      gear::Guard               guard(actor);

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      reactor::Lock lock(&elle::concurrency::scheduler(), scope->mutex.write());
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
                 "storing this object");

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
            // if the object has been sealed, i.e there is no more actor
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
    /// this method destroys an object.
    ///
    /// this method should be used with great care since, not knowing the
    /// object's genre, the data blocks will not be removed. therefore,
    /// the genre-specific Destroy() method should always be preferred.
    ///
    elle::Status        Object::Destroy(
                          const gear::Identifier&               identifier)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Object*     context;

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] wall::Object::Destroy()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        escape("unable to select the actor");

      gear::Guard               guard(actor);

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      reactor::Lock lock(&elle::concurrency::scheduler(), scope->mutex.write());
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
                 "destroying this object");

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
            // if the object has been sealed, i.e there is no more actor
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
    /// this method purges an object i.e removes all the blocks of all
    /// the versions associated with this object.
    ///
    /// this method should be used with great care since, not knowing the
    /// object's genre, the data blocks will not be removed. therefore,
    /// the genre-specific Destroy() method should always be preferred.
    ///
    elle::Status        Object::Purge(
                          const gear::Identifier&)
    {
      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] wall::Object::Purge()\n");

      // XXX to implement.

      return elle::Status::Ok;
    }

  }
}
