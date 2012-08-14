#include <etoile/automaton/Ensemble.hh>
#include <etoile/gear/Group.hh>
#include <etoile/depot/Depot.hh>

#include <nucleus/proton/Version.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/proton/State.hh>
#include <nucleus/neutron/Ensemble.hh>

#include <elle/cryptography/KeyPair.hh>
#include <elle/log.hh>

#include <agent/Agent.hh>

#include <hole/Hole.hh>

ELLE_LOG_TRACE_COMPONENT("infinit.etoile.automaton.Ensemble");

namespace etoile
{
  namespace automaton
  {

    elle::Status
    Ensemble::Open(gear::Group& context)
    {
      ELLE_LOG_TRACE_SCOPE("Open()");

      // if the ensemble is already opened, return.
      if (context.ensemble != nullptr)
        return elle::Status::Ok;

      // otherwise create a new ensemble according to the context's type.
      context.ensemble = new nucleus::neutron::Ensemble;

      // check if there exists a ensemble. if so, load the block.
      if (context.group->ensemble() != nucleus::proton::Address::Null)
        {
          ELLE_LOG_TRACE_SCOPE("pull the ensemble block from the storage layer");

          // load the block.
          if (depot::Depot::Pull(context.group->ensemble(),
                                 nucleus::proton::Version::Any,
                                 *context.ensemble) == elle::Status::Error)
            escape("unable to load the ensemble");
        }
      else
        {
          ELLE_LOG_TRACE_SCOPE("the group does not reference an ensemble");
        }

      return elle::Status::Ok;
    }

    elle::Status
    Ensemble::Destroy(gear::Group& context)
    {
      ELLE_LOG_TRACE_SCOPE("Destroy()");

      assert(context.group != nullptr);

      // if a block is referenced by the object, mark it as needing removal.
      if (context.group->ensemble() != nucleus::proton::Address::Null)
        {
          ELLE_LOG_TRACE_SCOPE("record the ensemble block in the transcript");

          // mark the content block for removal.
          if (context.transcript.Wipe(context.group->ensemble()) ==
              elle::Status::Error)
            escape("unable to mark the ensemble block for removal");
        }

      return elle::Status::Ok;
    }

    elle::Status
    Ensemble::Close(gear::Group& context)
    {
      ELLE_LOG_TRACE_SCOPE("Close()");

      //
      // first, check if the block has been modified i.e exists and is dirty.
      //
      {
        // if there is no loaded ensemble, then there is nothing to do.
        if (context.ensemble == nullptr)
          return elle::Status::Ok;

        // if the ensemble has not changed, do nothing.
        if (context.ensemble->state == nucleus::proton::StateClean)
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
          // versions, removing the ensemble block would render the previous
          // version inconsistent.
          //
          // therefore, the group is updated with a null content address.
          //
          // however, should the history functionality not be supported
          // by the network, the ensemble block is marked for deletion.
          //

          ELLE_LOG_TRACE_SCOPE("the ensemble is empty");

          // does the network support the history?
          if (hole::Hole::Descriptor.history == false)
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
          // groups benefit from the history i.e multiple versions; unless
          // the history support is not activated for this network.
          //

          ELLE_LOG_TRACE_SCOPE("the ensemble contains fellows: update and bind the ensemble");

          nucleus::proton::Address address;
          elle::cryptography::KeyPair pass;

          // does the network support the history?
          if (hole::Hole::Descriptor.history == false)
            {
              // destroy the ensemble block.
              if (Ensemble::Destroy(context) == elle::Status::Error)
                escape("unable to destroy the ensemble block");
            }

          /* XXX[in theory, a new pass should be generated but for now,
                 we decided to use the same pass throughout the group's
                 history, which is less secure, but enough for now]
          // generate the new pass.
          if (pass.Generate() == elle::Status::Error)
            escape("unable to generate the pass");
          */
          /* XXX[instead we must make sure to generate a key pair when
                 necessary i.e for the first version of the group. otherwise,
                 create a pass, based on the current public and private pass.] */
          // XXX
          {
            if (context.group->version == 1)
              {
                if (pass.Generate() == elle::Status::Error)
                  escape("unable to generate the pass");
              }
            else
              {
                nucleus::neutron::Token token(context.group->manager_token());
                elle::cryptography::PrivateKey k;

                if (token.Extract(agent::Agent::Identity.pair.k,
                                  k) == elle::Status::Error)
                  escape("unable to extract the token");

                pass.K = context.group->pass();
                pass.k = k;
              }
          }
          // XXX

          // upgrade the ensemble's tokens with the new pass.
          // besides, update the group's size with the number
          // of elements in the ensemble.
          // XXX[remove try/catch]
          try
            {
              context.ensemble->update(pass.k);

              // The group size equals the number of fellows in the
              // ensemble plus the group manager. This is why one is
              // added to the size.
              context.group->size(context.ensemble->size() + 1);
            }
          catch (...)
            {
              escape("unable to upgrade the ensemble with a new pass");
            }

          // bind the ensemble i.e seal it by computing its address.
          if (context.ensemble->Bind(address) == elle::Status::Error)
            escape("unable to bind the ensemble");

          // set the content as consistent.
          context.ensemble->state = nucleus::proton::StateConsistent;

          // mark the block as needing to be stored.
          if (context.transcript.Push(address,
                                      context.ensemble) == elle::Status::Error)
            escape("unable to record the ensemble for storing");

          // ugrade the group.
          // XXX[remove try/catch]
          try
            {
              context.group->upgrade(address, pass.K, pass.k);
            }
          catch (...)
            {
              escape("unable to update the object");
            }

          // XXX
          context.ensemble->Dump();
          context.group->Dump();
        }

      return elle::Status::Ok;
    }

  }
}
