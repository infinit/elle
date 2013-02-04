#include <etoile/wall/File.hh>
#include <etoile/gear/Identifier.hh>
#include <etoile/gear/Scope.hh>
#include <etoile/gear/File.hh>
#include <etoile/gear/Operation.hh>
#include <etoile/gear/Guard.hh>
#include <etoile/automaton/File.hh>
#include <etoile/automaton/Rights.hh>
#include <etoile/journal/Journal.hh>
#include <etoile/abstract/Object.hh>

#include <nucleus/neutron/Offset.hh>
#include <nucleus/neutron/Size.hh>

#include <elle/log.hh>
#include <elle/Buffer.hh>
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
      ELLE_TRACE_FUNCTION("");

      gear::Scope* scope;
      gear::File* context;

      // acquire the scope.
      if (gear::Scope::Supply(scope) == elle::Status::Error)
        throw elle::Exception("unable to supply the scope");

      gear::Guard guard(scope);
      gear::Identifier identifier;

      // Declare a critical section.
      {
        reactor::Lock lock(elle::concurrency::scheduler(),
                           scope->mutex.write());

        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          throw elle::Exception("unable to retrieve the context");

        // allocate an actor.
        guard.actor(new gear::Actor(scope));

        // return the identifier.
        identifier = guard.actor()->identifier;

        // apply the create automaton on the context.
        if (automaton::File::Create(*context) == elle::Status::Error)
          throw elle::Exception("unable to create the file");

        // set the actor's state.
        guard.actor()->state = gear::Actor::StateUpdated;

        ELLE_DEBUG("returning identifier %s on %s", identifier, *scope);

        // waive the scope.
        if (guard.Release() == elle::Status::Error)
          throw elle::Exception("unable to release the guard");
      }

      return (identifier);
    }

    gear::Identifier
    File::load(path::Chemin const& chemin)
    {
      ELLE_TRACE_FUNCTION(chemin);

      gear::Scope* scope;
      gear::File* context;

      // acquire the scope.
      if (gear::Scope::Acquire(chemin, scope) == elle::Status::Error)
        throw elle::Exception("unable to acquire the scope");

      gear::Guard guard(scope);
      gear::Identifier identifier;

      // declare a critical section.
      {
        reactor::Lock lock(elle::concurrency::scheduler(),
                           scope->mutex.write());

        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          throw elle::Exception("unable to retrieve the context");

        // allocate an actor.
        guard.actor(new gear::Actor(scope));

        // return the identifier.
        identifier = guard.actor()->identifier;

        // locate the object based on the chemin.
        if (chemin.Locate(context->location) == elle::Status::Error)
          throw elle::Exception("unable to locate the file");

        try
          {
            // apply the load automaton on the context.
            if (automaton::File::Load(*context) == elle::Status::Error)
              throw elle::Exception("unable to load the file");
          }
        catch (std::exception const&)
          {
            assert(scope != nullptr);
            Object::reload<gear::File>(*scope);
          }

        ELLE_DEBUG("returning identifier %s on %s", identifier, *scope);

        // waive the actor and the scope.
        if (guard.Release() == elle::Status::Error)
          throw elle::Exception("unable to release the guard");
      }

      return (identifier);
    }

    void
    File::write(gear::Identifier const& identifier,
                nucleus::neutron::Offset const& offset,
                elle::WeakBuffer const& data)
    {
      ELLE_TRACE_FUNCTION(identifier, offset, data);

      gear::Actor* actor;
      gear::Scope* scope;
      gear::File* context;

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        throw elle::Exception("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      {
        reactor::Lock lock(elle::concurrency::scheduler(),
                           scope->mutex.write());

        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          throw elle::Exception("unable to retrieve the context");

        // apply the write automaton on the context.
        if (automaton::File::Write(*context,
                                   offset,
                                   data) == elle::Status::Error)
          throw elle::Exception("unable to write the file");

        // set the actor's state.
        actor->state = gear::Actor::StateUpdated;
      }
    }

    elle::Buffer
    File::read(gear::Identifier const& identifier,
               nucleus::neutron::Offset const& offset,
               nucleus::neutron::Size const& size)
    {
      ELLE_TRACE_FUNCTION(identifier, offset, size);

      gear::Actor* actor;
      gear::Scope* scope;
      gear::File* context;

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        throw elle::Exception("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      elle::Buffer* buffer = nullptr;

      ELLE_FINALLY_ACTION_DELETE(buffer);

      // declare a critical section.
      {
        reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex);

        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          throw elle::Exception("unable to retrieve the context");

        // apply the read automaton on the context.
        buffer = new elle::Buffer{std::move(automaton::File::read(*context,
                                                                  offset,
                                                                  size))};
      }

      // Thanks to the finally, the buffer will be deleted after being returned.
      //
      // Besides, the buffer is moved so as to make sure no copy is performed.
      // We are forced to do so because Buffers are no-copyable by default.
      return (std::move(*buffer));
    }

    ///
    /// this method adjusts the size of a file.
    ///
    elle::Status        File::Adjust(
                          const gear::Identifier&               identifier,
                          const nucleus::neutron::Size& size)
    {
      ELLE_TRACE_FUNCTION(identifier, size);

      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::File*       context;

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        throw elle::Exception("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // Declare a critical section.
      {
        reactor::Lock lock(elle::concurrency::scheduler(),
                           scope->mutex.write());

        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          throw elle::Exception("unable to retrieve the context");

        // apply the adjust automaton on the context.
        if (automaton::File::Adjust(*context,
                                    size) == elle::Status::Error)
          throw elle::Exception("unable to adjust the file's size");

        // set the actor's state.
        actor->state = gear::Actor::StateUpdated;
      }

      return elle::Status::Ok;
    }

    void
    File::discard(gear::Identifier const& identifier)
    {
      ELLE_TRACE_FUNCTION(identifier);

      gear::Actor* actor;
      gear::Scope* scope;
      gear::File* context;

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        throw elle::Exception("unable to select the actor");

      gear::Guard guard(actor);

      // retrieve the scope.
      scope = actor->scope;

      // Declare a critical section.
      {
        reactor::Lock lock(elle::concurrency::scheduler(),
                           scope->mutex.write());

        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          throw elle::Exception("unable to retrieve the context");

        // check the permissions before performing the operation in
        // order not to alter the scope should the operation not be
        // allowed.
        if (automaton::Rights::Operate(
              *context,
              gear::OperationDiscard) == elle::Status::Error)
          throw elle::Exception("the user does not seem to have the necessary permission for "
                 "discarding this file");

        // specify the closing operation performed by the actor.
        if (actor->Operate(gear::OperationDiscard) == elle::Status::Error)
          throw elle::Exception("this operation cannot be performed by this actor");

        // delete the actor.
        guard.actor(nullptr);

        // specify the closing operation performed on the scope.
        if (scope->Operate(gear::OperationDiscard) == elle::Status::Error)
          throw elle::Exception("unable to specify the operation being performed "
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
            // if the file has been sealed, i.e there is no more actor
            // operating on it, record it in the journal.
            //

            // relinquish the scope: at this point we know there is no
            // remaining actor.
            if (gear::Scope::Relinquish(scope) == elle::Status::Error)
              throw elle::Exception("unable to relinquish the scope");

            // record the scope in the journal.
            if (journal::Journal::Record(scope) == elle::Status::Error)
              throw elle::Exception("unable to record the scope in the journal");

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
    File::store(gear::Identifier const& identifier)
    {
      ELLE_TRACE_FUNCTION(identifier);

      gear::Actor* actor;
      gear::Scope* scope;
      gear::File* context;

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        throw elle::Exception("unable to select the actor");

      gear::Guard guard(actor);

      // retrieve the scope.
      scope = actor->scope;

      // Declare a critical section.
      {
        reactor::Lock lock(elle::concurrency::scheduler(),
                           scope->mutex.write());

        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          throw elle::Exception("unable to retrieve the context");

        // check the permissions before performing the operation in
        // order not to alter the scope should the operation not be
        // allowed.
        if (automaton::Rights::Operate(
              *context,
              gear::OperationStore) == elle::Status::Error)
          throw elle::Exception("the user does not seem to have the necessary permission for "
                 "storing this file");

        // specify the closing operation performed by the actor.
        if (actor->Operate(gear::OperationStore) == elle::Status::Error)
          throw elle::Exception("this operation cannot be performed by this actor");

        // delete the actor.
        guard.actor(nullptr);

        // specify the closing operation performed on the scope.
        if (scope->Operate(gear::OperationStore) == elle::Status::Error)
          throw elle::Exception("unable to specify the operation being performed "
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
            // if the file has been sealed, i.e there is no more actor
            // operating on it, record it in the journal.
            //

            // relinquish the scope: at this point we know there is no
            // remaining actor.
            if (gear::Scope::Relinquish(scope) == elle::Status::Error)
              throw elle::Exception("unable to relinquish the scope");

            // record the scope in the journal.
            if (journal::Journal::Record(scope) == elle::Status::Error)
              throw elle::Exception("unable to record the scope in the journal");

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
      ELLE_TRACE_FUNCTION(identifier);

      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::File*       context;

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        throw elle::Exception("unable to select the actor");

      gear::Guard               guard(actor);

      // retrieve the scope.
      scope = actor->scope;

      // Declare a critical section.
      {
        reactor::Lock lock(elle::concurrency::scheduler(),
                           scope->mutex.write());

        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          throw elle::Exception("unable to retrieve the context");

        // check the permissions before performing the operation in
        // order not to alter the scope should the operation not be
        // allowed.
        if (automaton::Rights::Operate(
              *context,
              gear::OperationDestroy) == elle::Status::Error)
          throw elle::Exception("the user does not seem to have the necessary permission for "
                 "destroying this file");

        // specify the closing operation performed by the actor.
        if (actor->Operate(gear::OperationDestroy) == elle::Status::Error)
          throw elle::Exception("this operation cannot be performed by this actor");

        // delete the actor.
        guard.actor(nullptr);

        // specify the closing operation performed on the scope.
        if (scope->Operate(gear::OperationDestroy) == elle::Status::Error)
          throw elle::Exception("unable to specify the operation being performed "
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
            return elle::Status::Ok;
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
            // if the file has been sealed, i.e there is no more actor
            // operating on it, record it in the journal.
            //

            // relinquish the scope: at this point we know there is no
            // remaining actor.
            if (gear::Scope::Relinquish(scope) == elle::Status::Error)
              throw elle::Exception("unable to relinquish the scope");

            // record the scope in the journal.
            if (journal::Journal::Record(scope) == elle::Status::Error)
              throw elle::Exception("unable to record the scope in the journal");

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

//
// ---------- XXX -------------------------------------------------------------
//

    // XXX[to remove as soon as possible i.e when etoile will be instanciable]

    nucleus::neutron::Size
    File::transferto(elle::String const& source,
                     gear::Identifier const& destination,
                     nucleus::neutron::Offset const& offset)
    {
      ELLE_TRACE_FUNCTION(source, destination, offset);

      std::streamsize N = 5242880;
      std::ifstream stream(source, std::ios::binary);
      nucleus::neutron::Offset _offset(offset);
      elle::Buffer buffer(N);

      while (stream.good())
        {
          buffer.size(N);

          stream.read((char*)buffer.mutable_contents(), buffer.size());

          buffer.size(stream.gcount());

          File::write(destination, _offset, elle::WeakBuffer{buffer});

          _offset += buffer.size();
        }

      stream.close();

      return (_offset - offset);
    }

    nucleus::neutron::Size
    File::transferfrom(gear::Identifier const& source,
                       elle::String const& destination,
                       nucleus::neutron::Offset const& offset,
                       nucleus::neutron::Size const& size)
    {
      ELLE_TRACE_FUNCTION(source, destination, offset, size);

      std::ofstream stream(destination,
                           std::ios::out | std::ios::binary | std::ios::app);

      stream.seekp(offset, std::ios_base::beg);

      if (!stream.good())
        throw elle::Exception("unable to seek at %s in the output file",
                              offset);

      ELLE_TRACE("reading %s bytes at offset %s",
                 size, offset);

      elle::Buffer buffer = File::read(source, offset, size);

      ELLE_ASSERT(buffer.size() != 0);

      stream.write((const char*)buffer.contents(),
                   static_cast<std::streamsize>(buffer.size()));

      stream.close();

      return (buffer.size());
    }
  }
}
