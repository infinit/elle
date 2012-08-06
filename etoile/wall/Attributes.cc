#include <etoile/wall/Attributes.hh>
#include <etoile/gear/Identifier.hh>
#include <etoile/gear/Scope.hh>
#include <etoile/gear/Object.hh>
#include <etoile/gear/Gear.hh>
#include <etoile/automaton/Attributes.hh>

#include <nucleus/neutron/Trait.hh>
#include <nucleus/neutron/Range.hh>

#include <Infinit.hh>

#include <elle/concurrency/Scheduler.hh>
#include <elle/log.hh>

ELLE_LOG_COMPONENT("etoile.wall.Attributes");

namespace etoile
{
  namespace wall
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method sets an attribute for the given object.
    ///
    elle::Status        Attributes::Set(
                          const gear::Identifier&               identifier,
                          const elle::String&                   name,
                          const elle::String&                   value)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Object*     context;

      ELLE_LOG_TRACE_SCOPE("Set()");

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

        // apply the set automaton on the context.
        if (automaton::Attributes::Set(*context,
                                       name,
                                       value) == elle::Status::Error)
          escape("unable to set the attribute");

        // set the actor's state.
        actor->state = gear::Actor::StateUpdated;
      }

      return elle::Status::Ok;
    }

    ///
    /// this method returns the caller the trait associated with
    /// the given name.
    ///
    /// note that this method should be used careful as a pointer to the
    /// target trait is returned. should this trait be destroyed by another
    /// actor's operation, accessing it could make the system crash.
    ///
    elle::Status        Attributes::Get(
                          const gear::Identifier&               identifier,
                          const elle::String&                   name,
                          nucleus::neutron::Trait*& trait)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Object*     context;

      ELLE_LOG_TRACE_SCOPE("Get()");

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

        // apply the get automaton on the context.
        if (automaton::Attributes::Get(*context,
                                       name,
                                       trait) == elle::Status::Error)
          escape("unable to get the attribute");
      }

      return elle::Status::Ok;
    }

    ///
    /// this method returns all the attributes.
    ///
    /// note that this method should be used careful as a set of pointers to
    /// the target traits is returned. should one of the traits be destroyed
    /// by another actor's operation, accessing it could make the system crash.
    ///
    elle::Status        Attributes::Fetch(
                          const gear::Identifier&               identifier,
                          nucleus::neutron::Range<
                            nucleus::neutron::Trait>& range)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Object*     context;

      ELLE_LOG_TRACE_SCOPE("Fetch()");

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

        // apply the fetch automaton on the context.
        if (automaton::Attributes::Fetch(*context,
                                         range) == elle::Status::Error)
          escape("unable to fetch the attribute");
      }

      return elle::Status::Ok;
    }

    ///
    /// this method removes the given attribute from the list.
    ///
    elle::Status        Attributes::Omit(
                          const gear::Identifier&               identifier,
                          const elle::String&                   name)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Object*     context;

      ELLE_LOG_TRACE_SCOPE("Omit()");

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

        // apply the omit automaton on the context.
        if (automaton::Attributes::Omit(*context,
                                        name) == elle::Status::Error)
          escape("unable to omit the attribute");

        // set the actor's state.
        actor->state = gear::Actor::StateUpdated;
      }

      return elle::Status::Ok;
    }

  }
}
