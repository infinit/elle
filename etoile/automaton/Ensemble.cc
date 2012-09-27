#include <etoile/automaton/Ensemble.hh>
#include <etoile/gear/Group.hh>
#include <etoile/depot/Depot.hh>

#include <nucleus/proton/Revision.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/proton/State.hh>
#include <nucleus/neutron/Ensemble.hh>

#include <elle/cryptography/KeyPair.hh>
#include <elle/log.hh>

#include <hole/Hole.hh>
#include <agent/Agent.hh>

ELLE_LOG_COMPONENT("infinit.etoile.automaton.Ensemble");

namespace etoile
{
  namespace automaton
  {

    elle::Status
    Ensemble::Open(gear::Group& context)
    {
      ELLE_TRACE_SCOPE("Open()");

      // if the ensemble is already opened, return.
      if (context.ensemble != nullptr)
        return elle::Status::Ok;

      // check if there exists a ensemble. if so, load the block.
      if (context.group->ensemble() != nucleus::proton::Address::Null)
        {
          ELLE_TRACE_SCOPE("pull the ensemble block from the storage layer");

          // XXX[remove try/catch later]
          try
            {
              // XXX[the context should make use of unique_ptr instead
              //     of releasing here.]
              context.ensemble =
                depot::Depot::pull_ensemble(
                  context.group->ensemble()).release();
            }
          catch (std::exception const& e)
            {
              escape("%s", e.what());
            }
        }
      else
        {
          ELLE_TRACE_SCOPE("the group does not reference an ensemble");

          context.ensemble =
            new nucleus::neutron::Ensemble(depot::hole().network(),
                                           agent::Agent::Identity.pair.K);
        }

      assert(context.ensemble);

      return elle::Status::Ok;
    }

    elle::Status
    Ensemble::Destroy(gear::Group& context)
    {
      ELLE_TRACE_SCOPE("Destroy()");

      assert(context.group != nullptr);

      // if a block is referenced by the object, mark it as needing removal.
      if (context.group->ensemble() != nucleus::proton::Address::Null)
        {
          ELLE_TRACE("record the ensemble block in the transcript")
            context.transcript.wipe(context.group->ensemble());
        }

      return elle::Status::Ok;
    }

    elle::Status
    Ensemble::Close(gear::Group& context)
    {
      ELLE_TRACE_SCOPE("Close()");

      //
      // first, check if the block has been modified i.e exists and is dirty.
      //
      {
        // if there is no loaded ensemble, then there is nothing to do.
        if (context.ensemble == nullptr)
          return elle::Status::Ok;

        // if the ensemble has not changed, do nothing.
        if (context.ensemble->state() == nucleus::proton::StateClean)
          return elle::Status::Ok;
      }

      //
      // at this point, the ensemble is known to have been modified.
      //

      // modify the group according to the ensemble.
      if (context.ensemble->empty() == true)
        {
          //
          // if the ensemble became empty after removals, the
          // group should no longer point to any ensemble block while
          // the old block should be deleted.
          //
          // however, since the group benefits from history i.e several
          // revisions, removing the ensemble block would render the previous
          // revision inconsistent.
          //
          // therefore, the group is updated with a null content address.
          //
          // however, should the history functionality not be supported
          // by the network, the ensemble block is marked for deletion.
          //

          ELLE_TRACE_SCOPE("the ensemble is empty");

          // XXX: restore history handling
          // does the network support the history?
          // if (depot::hole().descriptor().meta().history() == false)
            {
              // destroy the ensemble block.
              if (Ensemble::Destroy(context) == elle::Status::Error)
                escape("unable to destroy the ensemble block");
            }

          // downgrade the group since it no longer reference an ensemble
          // of fellows.
          // XXX[remove try/catch]
          try
            {
              // The group size equals one i.e the group manager in this
              // case since no ensemble is referenced.
              context.group->size(1);

              context.group->downgrade();
            }
          catch (...)
            {
              escape("unable to downdate the group");
            }
        }
      else
        {
          //
          // otherwise, the new ensemble address needs to be computed and
          // the group updated accordingly.
          //
          // note that the previous ensemble block is not removed since
          // groups benefit from the history i.e multiple revisions; unless
          // the history support is not activated for this network.
          //

          ELLE_TRACE_SCOPE("the ensemble contains fellows: update and bind the ensemble");

          elle::cryptography::KeyPair pass;

          // XXX: restore history handling
          // does the network support the history?
          // if (depot::hole().descriptor().meta().history() == false)
            {
              // destroy the ensemble block.
              if (Ensemble::Destroy(context) == elle::Status::Error)
                escape("unable to destroy the ensemble block");
            }

          /* XXX[in theory, a new pass should be generated but for now,
                 we decided to use the same pass throughout the group's
                 history, which is less secure, but enough for now]
          // generate the new pass.
          if (pass.Generate() == elle::Status::Error) // XXX[use specific length]
            escape("unable to generate the pass");
          */
          /* XXX[instead the Group has been modified such that a keypair has
                 been generation at the construction. thus the group already
                 has a public pass while the manager already has a valid token.
                 these can be used to retrieve the previous pass and emulate
                 the process of updating the group with the same public/private
                 pass, as show below.] */
          // XXX
          {
            nucleus::neutron::Token token(context.group->manager_token());
            elle::cryptography::PrivateKey k;

            if (token.Extract(agent::Agent::Identity.pair.k,
                              k) == elle::Status::Error)
              escape("unable to extract the token");

            pass.K = context.group->pass_K();
            pass.k = k;
          }
          // XXX

          // upgrade the ensemble's tokens with the new pass.
          // besides, update the group's size with the number
          // of elements in the ensemble.
          // XXX[remove try/catch]
          try
            {
              context.ensemble->update(pass.k);
            }
          catch (...)
            {
              escape("unable to upgrade the ensemble with a new pass");
            }

          nucleus::proton::Address address(context.ensemble->bind());

          // set the content as consistent.
          context.ensemble->state(nucleus::proton::StateConsistent);

          // mark the block as needing to be stored.
          context.transcript.push(address, context.ensemble);

          // ugrade the group.
          // XXX[remove try/catch]
          try
            {
              // The group size equals the number of fellows in the
              // ensemble plus the group manager. This is why one is
              // added to the size.
              context.group->size(context.ensemble->size() + 1);

              // Regenerate the group manager's token.
              nucleus::neutron::Token manager_token(
                context.group->manager_subject().user(),
                pass.k);

              context.group->upgrade(address, pass.K, manager_token);
            }
          catch (...)
            {
              escape("unable to upgrade the group");
            }
        }

      return elle::Status::Ok;
    }

  }
}
