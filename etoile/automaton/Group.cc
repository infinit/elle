#include <etoile/automaton/Group.hh>
#include <etoile/automaton/Ensemble.hh>
#include <etoile/automaton/Rights.hh>
#include <etoile/gear/Group.hh>
#include <etoile/depot/Depot.hh>
#include <etoile/abstract/Group.hh>

#include <nucleus/neutron/Group.hh>

#include <agent/Agent.hh>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("infinit.etoile.automaton.Group");

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

      ELLE_TRACE_SCOPE("Create()");

      // XXX[remove try/catch later]
      try
        {
          context.group = new nucleus::neutron::Group(agent::Agent::Subject.user(),
                                                      description);
        }
      catch (std::exception const& e)
        {
          escape("%s", e.what());
        }

      // bind the object to its address i.e this will never changed.
      if (context.group->Bind(address) == elle::Status::Error)
        escape("unable to bind the object");

      // create the context's location with an initial version number.
      if (context.location.Create(address,
                                  context.group->version) == elle::Status::Error)
        escape("unable to create the location");

      context.state = gear::Context::StateCreated;

      // return the identity.
      identity = address;

      return elle::Status::Ok;
    }

    elle::Status
    Group::Load(gear::Group& context)
    {
      ELLE_TRACE_SCOPE("Load()");

      // return if the context has already been loaded.
      if (context.state != gear::Context::StateUnknown)
        return elle::Status::Ok;

      // XXX[remove try/catch later]
      try
        {
          context.group =
            depot::Depot::pull_group(
              context.location.address,
              context.location.version).release();
        }
      catch (std::exception const& e)
        {
          escape("%s", e.what());
        }

      // compute the base in order to seal the block's original state.
      if (context.group->base.Create(*context.group) == elle::Status::Error)
        escape("unable to compute the base");

      // set the context's state.
      context.state = gear::Context::StateLoaded;

      return elle::Status::Ok;
    }

    elle::Status
    Group::Information(gear::Group& context,
                       abstract::Group& abstract)
    {
      // generate the abstract based on the group.
      if (abstract.Create(*context.group) == elle::Status::Error)
        escape("unable to generate the abstract");

      return elle::Status::Ok;
    }

    elle::Status
    Group::Add(gear::Group& context,
               nucleus::neutron::Subject const& subject)
    {
      ELLE_TRACE_SCOPE("Add()");

      // determine the rights.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // check if the current user is the manager of the group.
      if (context.rights.role != nucleus::neutron::Group::RoleManager)
        escape("the user does not seem to be the group manager");

      // update the group depending on the subject.
      if (subject == context.group->manager_subject())
        {
          //
          // in this case, the subject represents the group's manager.
          //

          escape("unable to add the group's manager to the group");
        }
      else
        {
          //
          // otherwise, the subject represents another user or group.
          //

          // open the ensemble.
          if (Ensemble::Open(context) == elle::Status::Error)
            escape("unable to open the ensemble block");

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
        }

      // is the target subject the user i.e the group manager in this case.
      if (agent::Agent::Subject == subject)
        {
          // recompute the context rights.
          if (Rights::Recompute(context) == elle::Status::Error)
            escape("unable to recompute the rights");
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
      ELLE_TRACE_SCOPE("Lookup()");

      // Ty to make the best of this call.
      if (agent::Agent::Subject == subject)
        {
          // Indeed, if the target subject is the current user, determine
          // the user's rights so that this is not to be done later.

          ELLE_TRACE("The target subject is the current user");

          if (Rights::Determine(context) == elle::Status::Error)
            escape("unable to determine the user's rights");

          fellow = &context.group->manager_fellow();
        }
      else
        {
          // Otherwise, proceed normally.

          ELLE_TRACE("The target subject is _not_ the current user");

          // Perform the lookup according to the subject.
          if (subject == context.group->manager_subject())
            {
              ELLE_TRACE("The target subject is the group manager");

              // If the target subject is the object owner, retrieve the
              // access record from the owner's meta section. Note that
              // this record is not part of the object but has been generated
              // automatically when the object was extracted.

              fellow = &context.group->manager_fellow();
            }
          else
            {
              // If we are dealing with a fellow, open the ensemble block
              // and look in it.

              ELLE_TRACE("The target subject is _not_ a group manager: try "
                             "to look in the ensemble");

              if (Ensemble::Open(context) == elle::Status::Error)
                escape("unable to open the ensemble block");

              // XXX[remove try/catch]
              try
                {
                  fellow = &context.ensemble->locate(subject);
                }
              catch (...)
                {
                  escape("unable to lookup the subject in the ensemble");
                }
            }
        }

      return elle::Status::Ok;
    }

    elle::Status
    Group::Consult(gear::Group& context,
                   nucleus::neutron::Index const& index,
                   nucleus::neutron::Size const& size,
                   nucleus::neutron::Range<nucleus::neutron::Fellow>& range)
    {
      ELLE_TRACE_SCOPE("Consult(%s, %s, %s, %s)",
                           context, index, size, range);

      if (Ensemble::Open(context) == elle::Status::Error)
        escape("unable to open the ensemble");

      // first detach the data from the range.
      if (range.Detach() == elle::Status::Error)
        escape("unable to detach the data from the range");

      // If the index starts with 0, include the manager by creating
      // a record for him.
      if (index == 0)
        {
          // Add the manager's fellow to the range.
          if (range.Add(&context.group->manager_fellow()) == elle::Status::Error)
            escape("unable to add the owner record");

          // Consult the ensemble by taking care of consulting one fellow
          // less i.e the manager's.

          // XXX[remove try/catch]
          try
            {
              nucleus::neutron::Range<nucleus::neutron::Fellow> r;

              r = context.ensemble->consult(index, size - 1);

              if (range.Add(r) == elle::Status::Error)
                escape("unable to add the consulted range to the final range");
            }
          catch (...)
            {
              escape("unable to consult the ensemble");
            }
        }
      else
        {
          // Consult the ensemble by taking care of starting the consultation
          // one index before since the manager's fellow, which is not located
          // in the ensemble block, counts as one fellow.

          // XXX[remove try/catch]
          try
            {
              range = context.ensemble->consult(index - 1, size);
            }
          catch (...)
            {
              escape("unable to consult the ensemble");
            }
        }

      return elle::Status::Ok;
    }

    elle::Status
    Group::Remove(gear::Group& context,
                  nucleus::neutron::Subject const& subject)
    {
      ELLE_TRACE_SCOPE("Remove()");

      // determine the rights.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // check if the current user is the manager of the group.
      if (context.rights.role != nucleus::neutron::Group::RoleManager)
        escape("the user does not seem to be the group manager");

      // update the group depending on the subject.
      if (subject == context.group->manager_subject())
        {
          //
          // in this case, the subject represents the group's manager.
          //

          escape("unable to remove the group's manager from the group");
        }
      else
        {
          //
          // otherwise, the subject represents another user or group.
          //

          // open the ensemble.
          if (Ensemble::Open(context) == elle::Status::Error)
            escape("unable to open the ensemble block");

          // XXX[remove try/catch]
          try
            {
              context.ensemble->remove(subject);
            }
          catch (...)
            {
              escape("unable to remove the subject from the ensemble");
            }
        }

      // is the target subject the user i.e the group manager in this case.
      if (agent::Agent::Subject == subject)
        {
          // recompute the context rights.
          if (Rights::Recompute(context) == elle::Status::Error)
            escape("unable to recompute the rights");
        }

      // set the context's state.
      context.state = gear::Context::StateModified;

      return elle::Status::Ok;
    }

    elle::Status
    Group::Discard(gear::Group& context)
    {
      ELLE_TRACE_SCOPE("Discard()");

      // set the context's state.
      context.state = gear::Context::StateDiscarded;

      return elle::Status::Ok;
    }

    elle::Status
    Group::Destroy(gear::Group& context)
    {
      ELLE_TRACE_SCOPE("Destroy()");

      // determine the rights.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // check if the current user is the group manager.
      if (context.rights.role != nucleus::neutron::Group::RoleManager)
        escape("the user does not seem to be the group manager");

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
      ELLE_TRACE_SCOPE("Store()");

      // determine the rights.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // check if the current user is the group manager.
      if (context.rights.role != nucleus::neutron::Group::RoleManager)
        escape("the user does not seem to be the group manager");

      // close the ensemble.
      if (Ensemble::Close(context) == elle::Status::Error)
        escape("unable to close the ensemble");

      // if the group has been modified i.e is dirty.
      if (context.group->state == nucleus::proton::StateDirty)
        {
          // seal the group, depending on the presence of a referenced
          // access block.

          ELLE_TRACE_SCOPE("the group is dirty");

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
