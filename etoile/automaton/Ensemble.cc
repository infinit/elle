#include <etoile/automaton/Ensemble.hh>
#include <etoile/gear/Group.hh>
#include <etoile/depot/Depot.hh>
#include <etoile/nest/Nest.hh>

#include <nucleus/proton/Revision.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/proton/State.hh>
#include <nucleus/proton/Porcupine.hh>
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

      // XXX
      static cryptography::SecretKey secret_key(cryptography::cipher::Algorithm::aes256,
                                                ENSEMBLE_SECRET_KEY);

      if (context.ensemble_porcupine != nullptr)
        return elle::Status::Ok;

      // Check if there exists ensemble. if so, instanciate a porcupine.
      if (context.group->ensemble().empty() == false)
        {
          // Instanciate a nest.
          context.ensemble_nest =
            new etoile::nest::Nest(ENSEMBLE_SECRET_KEY_LENGTH,
                                   context.ensemble_limits,
                                   depot::hole().storage().network(),
                                   agent::Agent::Subject.user());

          // Instanciate a porcupine.
          context.ensemble_porcupine =
            new nucleus::proton::Porcupine<nucleus::neutron::Ensemble>{
              context.group->ensemble(),
              secret_key,
              *context.ensemble_nest};
        }
      else
        {
          // Instanciate a nest.
          context.ensemble_nest =
            new etoile::nest::Nest(ENSEMBLE_SECRET_KEY_LENGTH,
                                   context.ensemble_limits,
                                   depot::hole().storage().network(),
                                   agent::Agent::Subject.user());

          // otherwise create a new empty porcupine.
          context.ensemble_porcupine =
            new nucleus::proton::Porcupine<nucleus::neutron::Ensemble>{
              *context.ensemble_nest};
        }

      ELLE_ASSERT(context.ensemble_porcupine != nullptr);

      return elle::Status::Ok;
    }

    elle::Status
    Ensemble::Destroy(gear::Group& context)
    {
      ELLE_TRACE_FUNCTION(context);

      // If the.group holds some ensemble, mark the blocks as
      // needing removal.
      if (context.group->ensemble().empty() == false)
        {
          // Optimisation: only proceed if the content strategy is block-based:
          switch (context.group->ensemble().strategy())
            {
            case nucleus::proton::Strategy::none:
              throw elle::Exception("unable to destroy an empty content");
            case nucleus::proton::Strategy::value:
              {
                // Nothing to do in this case since there is no block for
                // holding the ensemble.
                //
                // The optimization makes us save some time since the content
                // is not deserialized.

                break;
              }
            case nucleus::proton::Strategy::block:
            case nucleus::proton::Strategy::tree:
              {
                Ensemble::Open(context);

                ELLE_TRACE("record the ensemble blocks for removal");
                ELLE_ASSERT(context.ensemble_porcupine != nullptr);
                context.ensemble_porcupine->destroy();

                ELLE_TRACE("mark the destroyed blocks as needed to be "
                           "removed from the storage layer");
                context.transcript().merge(
                  context.ensemble_nest->transcribe());

                break;
              }
            default:
              throw elle::Exception("unknown strategy '%s'",
                                    context.group->ensemble().strategy());
            }
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
        if (context.ensemble_porcupine == nullptr)
          return elle::Status::Ok;

        // if the ensemble has not changed, do nothing.
        if (context.ensemble_porcupine->state() ==
            nucleus::proton::State::clean)
          return elle::Status::Ok;
      }

      //
      // at this point, the ensemble is known to have been modified.
      //

      // retrieve the ensemble's size.
      nucleus::neutron::Size size = context.ensemble_porcupine->size();

      // modify the group according to the ensemble.
      if (size == 0)
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
                throw elle::Exception("unable to destroy the ensemble block");
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
                throw elle::Exception("unable to destroy the ensemble block");
            }

          /* XXX[in theory, a new pass should be generated but for now,
                 we decided to use the same pass throughout the group's
                 history, which is less secure, but enough for now]
          // generate the new pass.
          if (pass.Generate() == elle::Status::Error) // XXX[use specific length]
            throw elle::Exception("unable to generate the pass");
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

          // XXX
          static cryptography::SecretKey secret_key(cryptography::cipher::Algorithm::aes256,
                                                    ENSEMBLE_SECRET_KEY);

          // upgrade the ensemble's tokens with the new pass.
          // besides, update the group's size with the number
          // of elements in the ensemble.
          nucleus::neutron::ensemble::upgrade(*context.ensemble_porcupine,
                                              pass->k());


          // XXX[too slow without a nest optimization: to activate later]
          ELLE_STATEMENT(context.ensemble_porcupine->check(nucleus::proton::flags::all));

          // ugrade the group.

          // The group size equals the number of fellows in the
          // ensemble plus the group manager. This is why one is
          // added to the size.
          context.group->size(1 + size);

          // Regenerate the group manager's token.
          nucleus::neutron::Token manager_token(
            pass->K(),
            context.group->manager_subject().user());

          context.group->upgrade(context.ensemble_porcupine->seal(secret_key),
                                 pass->K(),
                                 manager_token);

          ELLE_FINALLY_ABORT(pass);

          delete pass;

          // mark the new/modified blocks as needing to be stored.
          context.transcript().merge(context.ensemble_nest->transcribe());
        }

      return elle::Status::Ok;
    }
  }
}
