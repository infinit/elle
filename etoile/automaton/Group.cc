#include <etoile/automaton/Group.hh>
#include <etoile/automaton/Ensemble.hh>
#include <etoile/automaton/Rights.hh>
#include <etoile/gear/Group.hh>
#include <etoile/depot/Depot.hh>

#include <nucleus/neutron/Group.hh>

#include <agent/Agent.hh>

#include <elle/log.hh>

ELLE_LOG_TRACE_COMPONENT("infinit.etoile.automaton.Group");

namespace etoile
{
  namespace automaton
  {

//
// ---------- methods ---------------------------------------------------------
//

    elle::Status
    Group::Create(gear::Group& context,
                  elle::String const& description,
                  typename nucleus::neutron::Group::Identity& identity)
    {
      nucleus::proton::Address address;

      ELLE_LOG_TRACE_SCOPE("Create()");

      context.group = new nucleus::neutron::Group(agent::Agent::Subject.user(),
                                                  description);

      // bind the object to its address i.e this will never changed.
      if (context.group->Bind(address) == elle::Status::Error)
        escape("unable to bind the object");

      // create the context's location with an initial version number.
      if (context.location.Create(address) == elle::Status::Error)
        escape("unable to create the location");

      context.state = gear::Context::StateCreated;

      // return the identity.
      identity = address;

      return elle::Status::Ok;
    }

    elle::Status
    Group::Load(gear::Group& context)
    {
      ELLE_LOG_TRACE_SCOPE("Load()");

      // return if the context has already been loaded.
      if (context.state != gear::Context::StateUnknown)
        return elle::Status::Ok;

      context.group = new nucleus::neutron::Group;

      if (depot::Depot::Pull(context.location.address,
                             context.location.version,
                             *context.group) == elle::Status::Error)
        escape("unable to retrieve the object block");

      // compute the base in order to seal the block's original state.
      if (context.group->base.Create(*context.group) == elle::Status::Error)
        escape("unable to compute the base");

      // set the context's state.
      context.state = gear::Context::StateLoaded;

      return elle::Status::Ok;
    }

    elle::Status
    Group::Add(gear::Group& context,
               nucleus::neutron::Subject const& subject)
    {
      ELLE_LOG_TRACE_SCOPE("Add()");

      // determine the rights.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // check if the current user is the owner of the group.
      if (context.rights.role != nucleus::neutron::RoleOwner)
        escape("the user does not seem to be the group owner");

      // open the ensemble.
      if (Ensemble::Open(context) == elle::Status::Error)
        escape("unable to open the ensemble");

      // XXX[remove try/catch]
      try
        {
          context.ensemble->add(
            std::move(std::unique_ptr<nucleus::neutron::Fellow>(
              new nucleus::neutron::Fellow(subject))));
        }
      catch (...)
        {
          escape("unable to add the subject to the ensemble");
        }

      // set the context's state.
      context.state = gear::Context::StateModified;

      return elle::Status::Ok;
    }

    elle::Status
    Group::Lookup(gear::Group& context,
                  nucleus::neutron::Subject const& subject,
                  nucleus::neutron::Fellow const*& fellow)
    {
      ELLE_LOG_TRACE_SCOPE("Lookup()");

      // open the ensemble.
      if (Ensemble::Open(context) == elle::Status::Error)
        escape("unable to open the ensemble");

      // XXX[remove try/catch]
      try
        {
          fellow = &context.ensemble->locate(subject);
        }
      catch (...)
        {
          escape("unable to lookup the subject in the ensemble");
        }

      return elle::Status::Ok;
    }

    elle::Status
    Group::Consult(gear::Group& context,
                   nucleus::neutron::Index const& index,
                   nucleus::neutron::Size const& size,
                   nucleus::neutron::Range<nucleus::neutron::Fellow>& range)
    {
      ELLE_LOG_TRACE_SCOPE("Consult(%s, %s, %s, %s)",
                           context, index, size, range);

      // open the ensemble.
      if (Ensemble::Open(context) == elle::Status::Error)
        escape("unable to open the ensemble");

      // XXX[remove try/catch]
      try
        {
          range = context.ensemble->consult(index, size);
        }
      catch (...)
        {
          escape("unable to consult the ensemble");
        }

      return elle::Status::Ok;
    }

    elle::Status
    Group::Remove(gear::Group& context,
                  nucleus::neutron::Subject const& subject)
    {
      ELLE_LOG_TRACE_SCOPE("Remove()");

      // determine the rights.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // check if the current user is the owner of the group.
      if (context.rights.role != nucleus::neutron::RoleOwner)
        escape("the user does not seem to be the group owner");

      // open the ensemble.
      if (Ensemble::Open(context) == elle::Status::Error)
        escape("unable to open the ensemble");

      // XXX[remove try/catch]
      try
        {
          context.ensemble->remove(subject);
        }
      catch (...)
        {
          escape("unable to remove the subject from the ensemble");
        }

      // set the context's state.
      context.state = gear::Context::StateModified;

      return elle::Status::Ok;
    }

    elle::Status
    Group::Discard(gear::Group& context)
    {
      ELLE_LOG_TRACE_SCOPE("Discard()");

      // set the context's state.
      context.state = gear::Context::StateDiscarded;

      return elle::Status::Ok;
    }

    elle::Status
    Group::Destroy(gear::Group& context)
    {
      ELLE_LOG_TRACE_SCOPE("Destroy()");

      // determine the rights.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // check if the current user is the group owner.
      if (context.rights.role != nucleus::neutron::RoleOwner)
        escape("the user does not seem to be the group owner");

      // open the ensemble.
      if (Ensemble::Open(context) == elle::Status::Error)
        escape("unable to open the ensemble");

      // destroy the ensemble.
      if (Ensemble::Destroy(context) == elle::Status::Error)
        escape("unable to destroy the ensemble");

      // mark the group as needing to be removed.
      if (context.transcript.Wipe(context.location.address) ==
          elle::Status::Error)
        escape("unable to record the group for removal");

      // set the context's state.
      context.state = gear::Context::StateDestroyed;

      return elle::Status::Ok;
    }

    elle::Status
    Group::Store(gear::Group& context)
    {
      ELLE_LOG_TRACE_SCOPE("Store()");

      // close the ensemble.
      if (Ensemble::Close(context) == elle::Status::Error)
        escape("unable to close the ensemble");

      // if the group has been modified i.e is dirty.
      if (context.group->state == nucleus::proton::StateDirty)
        {
          // seal the group, depending on the presence of a referenced
          // access block.

          ELLE_LOG_TRACE_SCOPE("the group is dirty");

          // XXX[remove try/catch]
          try
            {
              context.group->seal(agent::Agent::Identity.pair.k);
            }
          catch (...)
            {
              escape("unable to seal the group");
            }

          // mark the block as needing to be stored.
          if (context.transcript.Push(context.location.address,
                                      context.group) == elle::Status::Error)
            escape("unable to record the group for storing");
        }

      // set the context's state.
      context.state = gear::Context::StateStored;

      return elle::Status::Ok;
    }

  }
}
