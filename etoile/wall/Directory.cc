#include <etoile/wall/Directory.hh>
#include <etoile/gear/Identifier.hh>
#include <etoile/gear/Scope.hh>
#include <etoile/gear/Directory.hh>
#include <etoile/gear/Operation.hh>
#include <etoile/gear/Guard.hh>
#include <etoile/automaton/Directory.hh>
#include <etoile/automaton/Rights.hh>
#include <etoile/journal/Journal.hh>
#include <etoile/path/Path.hh>
#include <etoile/shrub/Shrub.hh>

#include <nucleus/proton/Address.hh>
#include <nucleus/neutron/Index.hh>
#include <nucleus/neutron/Size.hh>
#include <nucleus/neutron/Range.hh>
#include <nucleus/neutron/Entry.hh>

#include <elle/log.hh>
#include <elle/concurrency/Scheduler.hh>

#include <Infinit.hh>

ELLE_LOG_TRACE_COMPONENT("infinit.etoile.wall.Directory");

namespace etoile
{
  namespace wall
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a new directory object.
    ///
    /// note however that the object is not attached to the hierarchy
    /// and is therefore considered as orphan.
    ///
    elle::Status        Directory::Create(
                          gear::Identifier&                     identifier)
    {
      gear::Scope*      scope;
      gear::Directory*  context;

      ELLE_LOG_TRACE("Create()");

      // acquire the scope.
      if (gear::Scope::Supply(scope) == elle::Status::Error)
        escape("unable to supply the scope");

      gear::Guard               guard(scope);

      // declare a critical section.
      reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex.write());
      {
        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          escape("unable to retrieve the context");

        // allocate an actor.
        guard.actor(new gear::Actor(scope));

        // return the identifier.
        identifier = guard.actor()->identifier;

        // apply the create automaton on the context.
        if (automaton::Directory::Create(*context) == elle::Status::Error)
          escape("unable to create the directory");

        // set the actor's state.
        guard.actor()->state = gear::Actor::StateUpdated;

        // waive the scope.
        if (guard.Release() == elle::Status::Error)
          escape("unable to release the guard");
      }

      return elle::Status::Ok;
    }

    ///
    /// this method loads the directory referenced through the given
    /// chemin.
    ///
    elle::Status        Directory::Load(
                          const path::Chemin&                   chemin,
                          gear::Identifier&                     identifier)
    {
      gear::Scope*      scope;
      gear::Directory*  context;

      ELLE_LOG_TRACE("Load()");

      // acquire the scope.
      if (gear::Scope::Acquire(chemin, scope) == elle::Status::Error)
        escape("unable to acquire the scope");

      gear::Guard               guard(scope);

      // declare a critical section.
      reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex.write());
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
          escape("unable to locate the directory");

        // apply the load automaton on the context.
        if (automaton::Directory::Load(*context) == elle::Status::Error)
          escape("unable to load the directory");

        // waive the scope.
        if (guard.Release() == elle::Status::Error)
          escape("unable to release the guard");
      }

      return elle::Status::Ok;
    }

    ///
    /// this method locks the given directory.
    ///
    /// the method returns true if the lock has been acquired, false
    /// otherwise.
    ///
    elle::Status        Directory::Lock(
                          const gear::Identifier&)
    {
      ELLE_LOG_TRACE("Lock()");

      // XXX to implement.

      return elle::Status::Ok;
    }

    ///
    /// this method releases a previously locked directory.
    ///
    elle::Status        Directory::Release(
                          const gear::Identifier&)
    {
      ELLE_LOG_TRACE("Release()");

      // XXX to implement.

      return elle::Status::Ok;
    }

    ///
    /// this method adds an entry to the given directory.
    ///
    elle::Status        Directory::Add(
                          const gear::Identifier&               parent,
                          const path::Slab&                     name,
                          const gear::Identifier&               child)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Directory*  directory;
      gear::Object*     object;
      nucleus::proton::Address address;

      ELLE_LOG_TRACE("Add()");

      // select the actor.
      if (gear::Actor::Select(child, actor) == elle::Status::Error)
        escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // retrieve the context.
      if (scope->Use(object) == elle::Status::Error)
        escape("unable to retrieve the context");

      // keep the address.
      address = object->location.address;

      // select the actor.
      if (gear::Actor::Select(parent, actor) == elle::Status::Error)
        escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex.write());
      {
        // retrieve the context.
        if (scope->Use(directory) == elle::Status::Error)
          escape("unable to retrieve the context");

        // apply the add automaton on the context.
        if (automaton::Directory::Add(*directory,
                                      name,
                                      address) == elle::Status::Error)
          escape("unable to add the directory entry");

        // set the actor's state.
        actor->state = gear::Actor::StateUpdated;
      }

      return elle::Status::Ok;
    }

    ///
    /// this method returns the directory entry associated with the
    /// given name.
    ///
    /// note that this method should be used careful as a pointer to the
    /// target entry is returned. should this entry be destroyed by another
    /// actor's operation, accessing it could make the system crash.
    ///
    elle::Status        Directory::Lookup(
                          const gear::Identifier&               identifier,
                          const path::Slab&                     name,
                          nucleus::neutron::Entry const*& entry)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Directory*  context;

      ELLE_LOG_TRACE("Lookup()");

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
        if (automaton::Directory::Lookup(*context,
                                         name,
                                         entry) == elle::Status::Error)
          escape("unable to lookup the directory entry");
      }

      return elle::Status::Ok;
    }

    ///
    /// this method returns a set [offset, offset + size[ of entries
    /// (name, address) from the directory identified by _identifier_.
    ///
    /// note that this method should be used careful as a set of pointers to
    /// the target entries is returned. should one of the entries be destroyed
    /// by another actor's operation, accessing it could make the system crash.
    ///
    elle::Status        Directory::Consult(
                          const gear::Identifier&               identifier,
                          const nucleus::neutron::Offset& offset,
                          const nucleus::neutron::Offset& size,
                          nucleus::neutron::Range<
                            nucleus::neutron::Entry>& range)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Directory*  context;

      ELLE_LOG_TRACE("Consult()");

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
        if (automaton::Directory::Consult(*context,
                                          offset,
                                          size,
                                          range) == elle::Status::Error)
          escape("unable to consult the directory entries");
      }

      return elle::Status::Ok;
    }

    ///
    /// this method renames a directory entry.
    ///
    elle::Status        Directory::Rename(
                          const gear::Identifier&               identifier,
                          const path::Slab&                     from,
                          const path::Slab&                     to)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Directory*  context;

      ELLE_LOG_TRACE("Rename()");

      // select the actor.
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

        // apply the rename automaton on the context.
        if (automaton::Directory::Rename(*context,
                                         from,
                                         to) == elle::Status::Error)
          escape("unable to rename the directory entry");

        // set the actor's state.
        actor->state = gear::Actor::StateUpdated;

        struct
        {
          path::Route           from;
          path::Route           to;
        }                       routes;

        //
        // create routes for both the _from_ and _to_ since these
        // routes are going to be used below several times.
        //
        {
          // build the route associated with the previous version of
          // the renamed entry.
          if (routes.from.Create(scope->chemin.route, from) == elle::Status::Error)
            escape("unable to create the route");

          // build the route associated with the new version of
          // the renamed entry.
          if (routes.to.Create(scope->chemin.route, to) == elle::Status::Error)
            escape("unable to create the route");
        }

        //
        // update the scopes should some reference the renamed entry.
        //
        // indeed, let us imagine the following scenario. a file
        // /tmp/F1 is created. this file is opened by two actors
        // A and B. then, actor A renames the file into /tmp/F2.
        //
        // later one, a actor, say C, re-creates and releases /tmp/F1.
        // then C loads /tmp/F1. since the original scope for /tmp/F1
        // has not been updated and since actors remain, i.e A and B,
        // the original scope is retrieved instead of the new one.
        //
        // for this reason, the scopes must be updated.
        //
        {
          path::Venue           venue;
          struct
          {
            path::Chemin        from;
            path::Chemin        to;
          }                     chemins;

          // resolve the old route _routes.from_ to a venue.
          if (path::Path::Resolve(routes.from, venue) == elle::Status::Error)
            escape("unable to resolve the route");

          // create a chemin based on both the old route and venue.
          if (chemins.from.Create(routes.from, venue) == elle::Status::Error)
            escape("unable to create the chemin");

          // create the new chemin which includes the new route and
          // the venue, which has not changed since.
          if (chemins.to.Create(routes.to, venue) == elle::Status::Error)
            escape("unable to create the chemin");

          // update the scope so as to update all the scopes
          // whose chemins are now inconsistent---i.e referencing
          // the old chemin _chemin.from_.
          if (gear::Scope::Update(chemins.from,
                                  chemins.to) == elle::Status::Error)
            escape("unable to update the scopes");
        }

        //
        // invalidate the _from_ and _to_ routes from the shrub.
        //
        {
          // evict the route from the shrub.
          if (shrub::Shrub::Evict(routes.from) == elle::Status::Error)
            escape("unable to evict the route from the shrub");

          // evict the route from the shrub.
          if (shrub::Shrub::Evict(routes.to) == elle::Status::Error)
            escape("unable to evict the route from the shrub");
        }
      }

      return elle::Status::Ok;
    }

    ///
    /// this method removes a directory entry.
    ///
    elle::Status        Directory::Remove(
                          const gear::Identifier&               identifier,
                          const path::Slab&                     name)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Directory*  context;
      path::Route       route;

      ELLE_LOG_TRACE("Remove()");

      // select the actor.
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
        if (automaton::Directory::Remove(*context,
                                         name) == elle::Status::Error)
          escape("unable to remove the directory entry");

        // set the actor's state.
        actor->state = gear::Actor::StateUpdated;

        //
        // invalidate the route in the shrub.
        //
        {
          // build the route associated with the removed entry.
          if (route.Create(scope->chemin.route, name) == elle::Status::Error)
            escape("unable to create the route");

          // evict the route from the shrub.
          if (shrub::Shrub::Evict(route) == elle::Status::Error)
            escape("unable to evict the route from the shrub");
        }
      }

      return elle::Status::Ok;
    }

    ///
    /// this method discards the scope along with the possible
    /// modifications having been performed.
    ///
    elle::Status        Directory::Discard(
                          const gear::Identifier&               identifier)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Directory*  context;

      ELLE_LOG_TRACE("Discard()");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        escape("unable to select the actor");

      gear::Guard               guard(actor);

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
                 "discarding this directory");

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
            // if the directory has been sealed, i.e there is no more actor
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
    /// this method closes the scope and places it in the journal for
    /// the modifications to be published in the storage layer.
    ///
    elle::Status        Directory::Store(
                          const gear::Identifier&               identifier)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Directory*  context;

      ELLE_LOG_TRACE("Store()");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        escape("unable to select the actor");

      gear::Guard               guard(actor);

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
                 "storing this directory");

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
            // if the directory has been sealed, i.e there is no more actor
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
    /// this method destroys a directory.
    ///
    elle::Status        Directory::Destroy(
                          const gear::Identifier&               identifier)
    {
      gear::Actor*              actor;
      gear::Scope*              scope;
      gear::Directory*          context;

      ELLE_LOG_TRACE("Destroy()");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        escape("unable to select the actor");

      gear::Guard               guard(actor);

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
                 "destroying this directory");

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
            // if the directory has been sealed, i.e there is no more actor
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
    /// this method purges a directory i.e removes all the blocks of all
    /// the versions associated with this directory.
    ///
    elle::Status        Directory::Purge(
                          const gear::Identifier&)
    {
      ELLE_LOG_TRACE("Purge()");

      // XXX to implement.

      return elle::Status::Ok;
    }

  }
}
