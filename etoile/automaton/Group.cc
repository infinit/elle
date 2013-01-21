#include <etoile/automaton/Group.hh>
#include <etoile/automaton/Ensemble.hh>
#include <etoile/automaton/Rights.hh>
#include <etoile/gear/Group.hh>
#include <etoile/depot/Depot.hh>
#include <etoile/abstract/Group.hh>

#include <nucleus/proton/Door.hh>
#include <nucleus/proton/Porcupine.hh>
#include <nucleus/neutron/Group.hh>
#include <nucleus/neutron/Ensemble.hh>

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
      ELLE_TRACE_FUNCTION(context, description);

      ELLE_ASSERT(context.group == nullptr);

      context.group.reset(
        new nucleus::neutron::Group(nucleus::proton::Network(Infinit::Network),
                                    agent::Agent::Subject.user(),
                                    description));

      // Manually set the group as dirty for the automata to consider it
      // new and ready to be serialized. Otherwise, the group would be ignored.
      //
      // This is required because creating an empty group is not meaningless
      // since a group always has at least one member, its manager. One could
      // therefore want to create a group a later add users. The group, when
      // "empty" should therefore be considered as valid.
      context.group->state(nucleus::proton::State::dirty);

      nucleus::proton::Address address(context.group->bind());

      // create the context's location with an initial revision number.
      context.location =
        nucleus::proton::Location(address, context.group->revision());

      context.state = gear::Context::StateCreated;

      // return the identity.
      identity = address;

      return elle::Status::Ok;
    }

    elle::Status
    Group::Load(gear::Group& context)
    {
      ELLE_TRACE_FUNCTION(context);

      // return if the context has already been loaded.
      if (context.state != gear::Context::StateUnknown)
        return elle::Status::Ok;

      ELLE_ASSERT(context.group == nullptr);

      context.group.reset(
        depot::Depot::pull_group(
          context.location.address(),
          context.location.revision()).release());

      // compute the base in order to seal the block's original state.
      context.group->base(nucleus::proton::Base(*context.group));

      // set the context's state.
      context.state = gear::Context::StateLoaded;

      return elle::Status::Ok;
    }

    elle::Status
    Group::Information(gear::Group& context,
                       abstract::Group& abstract)
    {
      ELLE_TRACE_FUNCTION(context, abstract);

      // generate the abstract based on the group.
      if (abstract.Create(*context.group) == elle::Status::Error)
        escape("unable to generate the abstract");

      return elle::Status::Ok;
    }

    elle::Status
    Group::Add(gear::Group& context,
               nucleus::neutron::Subject const& subject)
    {
      ELLE_TRACE_FUNCTION(context, subject);

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

          // Retrieve a door on the ensemble.
          nucleus::proton::Door<nucleus::neutron::Ensemble> door{
            context.ensemble_porcupine->lookup(subject)};

          door.open();

          if (door().exist(subject) == true)
            throw elle::Exception("this subject already exist");

          /// Deliberately provide a null token because the right token
          /// will be generated when the group is closed. This improves
          /// the performance by delaying the cryptographic operations.
          door().insert(new nucleus::neutron::Fellow{
                          subject,
                          nucleus::neutron::Token::null()});

          door.close();

          // Update the porcupine.
          context.ensemble_porcupine->update(subject);
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
      ELLE_TRACE_FUNCTION(context, subject);

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
              // ensemble fellow from the owner's meta section. Note that
              // this fellow is not part of the object but has been generated
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

              // Retrieve a door on the ensemble.
              nucleus::proton::Door<nucleus::neutron::Ensemble> door{
                context.ensemble_porcupine->lookup(subject)};

              door.open();

              // XXX[does the fellow exist: if not, return a null pointer]
              if (door().exist(subject) == false)
                {
                  fellow = nullptr;
                }
              else
                {
                  // Look up the fellow.
                  // XXX[we take the address of the reference: wrong]
                  fellow = &(door().locate(subject));
                }

              door.close();
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
      ELLE_TRACE_FUNCTION(context, index, size);

      nucleus::neutron::Index _index = index;

      if (Ensemble::Open(context) == elle::Status::Error)
        escape("unable to open the ensemble");

      // if the index starts with 0, include the manager by creating
      // a fellow for him.
      if (_index == 0)
        {
          // add the fellow to the range.
          range.insert(
            std::shared_ptr<nucleus::neutron::Fellow>{
              new nucleus::neutron::Fellow{context.group->manager_fellow()}});

          _index++;
        }

      // Seek the group responsible for the given index.
      auto _size = context.ensemble_porcupine->size();

      // Decrement index because an index of 1 is actually 0 relative to the
      // porcupine because the fellow index-0 is the manager's.
      _index--;

      while (_size > 0)
        {
          auto pair = context.ensemble_porcupine->seek(_index);
          auto& door = pair.first;
          auto& base = pair.second;

          door.open();

          auto start = _index - base;
          auto length = _size > (door().size() - start) ?
            (door().size() - start) : _size;

          ELLE_ASSERT(length != 0);

          // Retrieve the fellows falling in the requested
          // range [index, index + size[.
          nucleus::neutron::Range<nucleus::neutron::Fellow> subrange{
            door().consult(start, length)};

          door.close();

          // Inject the retrieved fellows into the main range.
          range.add(subrange);

          // Update the variables _index and _size.
          _index += length;
          _size -= length;
        }

      return elle::Status::Ok;
    }

    elle::Status
    Group::Remove(gear::Group& context,
                  nucleus::neutron::Subject const& subject)
    {
      ELLE_TRACE_FUNCTION(context, subject);

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

          // Retrieve a door on the ensemble.
          nucleus::proton::Door<nucleus::neutron::Ensemble> door{
            context.ensemble_porcupine->lookup(subject)};

          door.open();

          // remove the fellowt.
          door().erase(subject);

          door.close();

          // Update the porcupine.
          context.ensemble_porcupine->update(subject);
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
      ELLE_TRACE_FUNCTION(context);

      // set the context's state.
      context.state = gear::Context::StateDiscarded;

      return elle::Status::Ok;
    }

    elle::Status
    Group::Destroy(gear::Group& context)
    {
      ELLE_TRACE_FUNCTION(context);

      // determine the rights.
      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      // check if the current user is the group manager.
      if (context.rights.role != nucleus::neutron::Group::RoleManager)
        escape("the user does not seem to be the group manager");

      // destroy the ensemble.
      if (Ensemble::Destroy(context) == elle::Status::Error)
        escape("unable to destroy the ensemble");

      // mark the group as needing to be removed.
      context.transcript().record(
        new gear::action::Wipe(context.location.address()));

      // set the context's state.
      context.state = gear::Context::StateDestroyed;

      return elle::Status::Ok;
    }

    elle::Status
    Group::Store(gear::Group& context)
    {
      ELLE_TRACE_FUNCTION(context);

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
      if (context.group->state() == nucleus::proton::State::dirty)
        {
          // seal the group, depending on the presence of a referenced
          // ensemble block.

          ELLE_TRACE_SCOPE("the group is dirty");

          context.group->seal(agent::Agent::Identity.pair().k());

          // mark the block as needing to be stored.
          context.transcript().record(
            new gear::action::Push(context.location.address(),
                                   std::move(context.group)));
        }

      // set the context's state.
      context.state = gear::Context::StateStored;

      return elle::Status::Ok;
    }

  }
}
