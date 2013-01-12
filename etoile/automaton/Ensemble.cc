#include <etoile/automaton/Ensemble.hh>
#include <etoile/gear/Group.hh>
#include <etoile/depot/Depot.hh>

#include <nucleus/proton/Revision.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/proton/State.hh>
#include <nucleus/neutron/Ensemble.hh>

#include <elle/finally.hh>
#include <elle/log.hh>

#include <cryptography/KeyPair.hh>
// XXX[temporary: for cryptography]
using namespace infinit;

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
      ELLE_TRACE_FUNCTION(context);

      // if the ensemble is already opened, return.
      if (context.ensemble != nullptr)
        return elle::Status::Ok;

      // check if there exists a ensemble. if so, load the block.
      if (context.group->ensemble() != nucleus::proton::Address::null())
        {
          ELLE_TRACE_SCOPE("pull the ensemble block from the storage layer");

          // XXX[the context should make use of unique_ptr instead
          //     of releasing here.]
          context.ensemble.reset(
            depot::Depot::pull_ensemble(
              context.group->ensemble()).release());
        }
      else
        {
          ELLE_TRACE_SCOPE("the group does not reference an ensemble");

          context.ensemble.reset(
            new nucleus::neutron::Ensemble(
              nucleus::proton::Network(Infinit::Network),
              agent::Agent::Identity.pair().K()));
        }

      ELLE_ASSERT(context.ensemble != nullptr);

      return elle::Status::Ok;
    }

    elle::Status
    Ensemble::Destroy(gear::Group& context)
    {
      ELLE_TRACE_FUNCTION(context);

      assert(context.group != nullptr);

      // if a block is referenced by the object, mark it as needing removal.
      if (context.group->ensemble() != nucleus::proton::Address::null())
        {
          ELLE_TRACE("record the ensemble block in the transcript");

          context.transcript().record(
            new gear::action::Wipe(context.group->ensemble()));
        }

      return elle::Status::Ok;
    }

    elle::Status
    Ensemble::Close(gear::Group& context)
    {
      ELLE_TRACE_FUNCTION(context);

      //
      // first, check if the block has been modified i.e exists and is dirty.
      //
      {
        // if there is no loaded ensemble, then there is nothing to do.
        if (context.ensemble == nullptr)
          return elle::Status::Ok;

        // if the ensemble has not changed, do nothing.
        if (context.ensemble->state() == nucleus::proton::State::clean)
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

          // The group size equals one i.e the group manager in this
          // case since no ensemble is referenced.
          context.group->size(1);

          context.group->downgrade();
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

          // XXX[use finally, from cryptography]
          cryptography::KeyPair* pass(nullptr);

          ELLE_FINALLY_ACTION_DELETE(pass);

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

            cryptography::PrivateKey k(
              token.extract<cryptography::PrivateKey>(
                agent::Agent::Identity.pair().k()));

            pass = new cryptography::KeyPair(context.group->pass_K(), k);
          }
          // XXX

          // upgrade the ensemble's tokens with the new pass.
          // besides, update the group's size with the number
          // of elements in the ensemble.
          context.ensemble->update(pass->k());

          nucleus::proton::Address address(context.ensemble->bind());

          // set the content as consistent.
          context.ensemble->state(nucleus::proton::State::consistent);

          // mark the block as needing to be stored.
          context.transcript().record(
            new gear::action::Push(address,
                                   std::move(context.ensemble)));

          // ugrade the group.

          // The group size equals the number of fellows in the
          // ensemble plus the group manager. This is why one is
          // added to the size.
          context.group->size(context.ensemble->size() + 1);

          // Regenerate the group manager's token.
          nucleus::neutron::Token manager_token(
            pass->K(),
            context.group->manager_subject().user());

          context.group->upgrade(address, pass->K(), manager_token);

          ELLE_FINALLY_ABORT(pass);

          delete pass;
        }

      return elle::Status::Ok;
    }

  }
}
