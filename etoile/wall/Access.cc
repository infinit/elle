#include <etoile/wall/Access.hh>
#include <etoile/gear/Identifier.hh>
#include <etoile/gear/Scope.hh>
#include <etoile/gear/Object.hh>
#include <etoile/gear/Gear.hh>
#include <etoile/automaton/Access.hh>

#include <nucleus/neutron/Subject.hh>
#include <nucleus/neutron/Record.hh>
#include <nucleus/neutron/Index.hh>
#include <nucleus/neutron/Size.hh>
#include <nucleus/neutron/Range.hh>
#include <nucleus/neutron/Permissions.hh>

#include <elle/concurrency/Scheduler.hh>
#include <elle/log.hh>

#include <Infinit.hh>

ELLE_LOG_TRACE_COMPONENT("etoile.wall.Access");

namespace etoile
{
  namespace wall
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method returns the caller the access record associated with
    /// the given subject.
    ///
    /// note that this method should be used careful as a pointer to the
    /// target record is returned. should this record be destroyed by another
    /// actor's operation, accessing it could make the system crash.
    ///
    elle::Status        Access::Lookup(
                          const gear::Identifier&               identifier,
                          const nucleus::neutron::Subject& subject,
                          nucleus::neutron::Record*& record)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Object*     context;

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
        if (automaton::Access::Lookup(*context,
                                      subject,
                                      record) == elle::Status::Error)
          escape("unable to lookup the access record");
      }

      return elle::Status::Ok;
    }

    ///
    /// this method returns a subset of the object's access access list.
    ///
    /// note that this method should be used careful as a set of pointers to
    /// the target records is returned. should one of the records be destroyed
    /// by another actor's operation, accessing it could make the system crash.
    ///
    elle::Status        Access::Consult(
                          const gear::Identifier&               identifier,
                          const nucleus::neutron::Index& index,
                          const nucleus::neutron::Size& size,
                          nucleus::neutron::Range<
                            nucleus::neutron::Record>& range)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Object*     context;

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
        if (automaton::Access::Consult(*context,
                                       index,
                                       size,
                                       range) == elle::Status::Error)
          escape("unable to consult the access records");
      }

      return elle::Status::Ok;
    }

    ///
    /// this method grants the given access permissions to the subject.
    ///
    elle::Status        Access::Grant(
                          const gear::Identifier&               identifier,
                          const nucleus::neutron::Subject& subject,
                          const nucleus::neutron::Permissions& permissions)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Object*     context;

      ELLE_LOG_TRACE("Grant()");

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

        // apply the grant automaton on the context.
        if (automaton::Access::Grant(*context,
                                     subject,
                                     permissions) == elle::Status::Error)
          escape("unable to grant access to the subject");

        // set the actor's state.
        actor->state = gear::Actor::StateUpdated;
      }

      return elle::Status::Ok;
    }

    ///
    /// this method removes the user's permissions from the access control
    /// list.
    ///
    elle::Status        Access::Revoke(
                          const gear::Identifier&               identifier,
                          const nucleus::neutron::Subject& subject)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Object*     context;

      ELLE_LOG_TRACE("Revoke()");

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

        // apply the revoke automaton on the context.
        if (automaton::Access::Revoke(*context,
                                      subject) == elle::Status::Error)
          escape("unable to revoke the subject's access permissions");

        // set the actor's state.
        actor->state = gear::Actor::StateUpdated;
      }
      return elle::Status::Ok;
    }

  }
}
