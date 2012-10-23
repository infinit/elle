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

ELLE_LOG_COMPONENT("infinit.etoile.wall.Attributes");

namespace etoile
{
  namespace wall
  {

//
// ---------- static methods --------------------------------------------------
//

    void
    Attributes::set(gear::Identifier const& identifier,
                    elle::String const& name,
                    elle::String const& value)
    {
      gear::Actor* actor;
      gear::Scope* scope;
      gear::Object* context;

      ELLE_TRACE_SCOPE("Set()");

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

        // apply the set automaton on the context.
        if (automaton::Attributes::Set(*context,
                                       name,
                                       value) == elle::Status::Error)
          escape("unable to set the attribute");

        // set the actor's state.
        actor->state = gear::Actor::StateUpdated;
      }
    }

    nucleus::neutron::Trait
    Attributes::get(gear::Identifier const& identifier,
                    elle::String const& name)
    {
      gear::Actor* actor;
      gear::Scope* scope;
      gear::Object* context;

      ELLE_TRACE_SCOPE("Get()");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      nucleus::neutron::Trait const* trait = nullptr;

      // Declare a critical section.
      {
        reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex);

        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          escape("unable to retrieve the context");

        // apply the get automaton on the context.
        if (automaton::Attributes::Get(*context,
                                       name,
                                       trait) == elle::Status::Error)
          escape("unable to get the attribute");
      }

      // Return the trait according to the automaton's result.
      if (trait != nullptr)
        return (*trait);
      else
        return (nucleus::neutron::Trait::Null);
    }

    nucleus::neutron::Range<nucleus::neutron::Trait>
    Attributes::fetch(gear::Identifier const& identifier)
    {
      gear::Actor* actor;
      gear::Scope* scope;
      gear::Object* context;

      ELLE_TRACE_SCOPE("Fetch()");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      nucleus::neutron::Range<nucleus::neutron::Trait> range;

      // Declare a critical section.
      {
        reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex);

        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          escape("unable to retrieve the context");

        // apply the fetch automaton on the context.
        if (automaton::Attributes::Fetch(*context,
                                         range) == elle::Status::Error)
          escape("unable to fetch the attribute");
      }

      return (range);
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

      ELLE_TRACE_SCOPE("Omit()");

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
