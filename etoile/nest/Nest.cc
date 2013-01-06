#include <etoile/nest/Nest.hh>
#include <etoile/gear/Transcript.hh>
#include <etoile/gear/Action.hh>
#include <etoile/depot/Depot.hh>

#include <elle/concurrency/Scheduler.hh>
#include <elle/finally.hh>

#include <cryptography/SecretKey.hh>

#include <nucleus/proton/Nest.hh>
#include <nucleus/proton/Handle.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Revision.hh>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("infinit.etoile.nest.Nest");

/* XXX
      /// Compute a static temporary address which will be the same for every
      /// block in the same nest since every such block belongs to the same
      /// object hence within the same network.
      ///
      /// The idea behind this computation is to provide a temporary address
      /// whose footprint (i.e size once serialized) is identical to the final
      /// one. Therefore, it has to be somewhat ressemble the final one without
      /// being valid.
      static Address some(block->network(),
                          block->family(),
                          block->component());

      ELLE_ASSERT(block->network() == some.network());
      ELLE_ASSERT(block->family() == some.family());
      ELLE_ASSERT(block->component() == some.component());
*/

namespace etoile
{
  namespace nest
  {
    /*-------------.
    | Construction |
    `-------------*/

    Nest::Nest(nucleus::proton::Limits const& limits):
      nucleus::proton::Nest(limits)
    {
    }

    Nest::~Nest()
    {
      for (auto pod: this->_pods)
        delete pod;

      this->_pods.clear();
      this->_addresses.clear();
      this->_queue.clear();
    }

    /*--------.
    | Methods |
    `--------*/

    gear::Transcript
    Nest::transcribe()
    {
      /* XXX
      auto i = this->_placements.begin();
      auto e = this->_placements.end();

      for (; i != e; ++i)
        {
          Pod* pod = i->second;

          switch (pod->nature)
            {
            case Pod::NatureVolatile:
              {
                // Ignore volatile detached blocks.
                if (pod->link == Pod::LinkDetached)
                  continue;

                switch (pod->block->state())
                  {
                  case nucleus::proton::State::clean:
                    {
                      // nothing to do: the block has not been modified.
                      //
                      // weird though! a block has been created and nothing has
                      // been inserted! perhaps show a warning?
                      break;
                    }
                  case nucleus::proton::State::dirty:
                    {
                      throw reactor::Exception(elle::concurrency::scheduler(),
                                               "The block is dirty but has not"
                                               "been sealed.");
                    }
                  case nucleus::proton::State::consistent:
                    {
                      pod->address = pod->block->bind();

                      transcript.record(new gear::action::Push(pod->address,
                                                               pod->block));

                      break;
                    }
                  }

                break;
              }
            case Pod::NaturePersistent:
              {
                switch (pod->link)
                  {
                  case Pod::LinkAttached:
                    {
                      switch (pod->block->state())
                        {
                        case nucleus::proton::State::clean:
                          {
                            // nothing to do: the block has not been modified.
                            break;
                          }
                        case nucleus::proton::State::dirty:
                          {
                            throw elle::Exception("the block is dirty but has "
                                                  "not been sealed.");
                          }
                        case nucleus::proton::State::consistent:
                          {
                            ELLE_ASSERT(pod->address !=
                                        nucleus::proton::Address::null());

                            transcript.record(
                              new gear::action::Wipe(pod->address));

                            nucleus::proton::Address address =
                              pod->block->bind();

                            transcript.record(
                              new gear::action::Push(address,
                                                     pod->block));

                            break;
                          }
                        }

                      break;
                    }
                  case Pod::LinkDetached:
                    {
                      if (pod->address != nucleus::proton::Address::null())
                        transcript.record(new gear::action::Wipe(pod->address));

                      break;
                    }
                  }

                break;
              }
            default:
              {
                throw reactor::Exception(elle::concurrency::scheduler(),
                                         "unknown pod nature");
              }
            }
        }
      */
    }

    elle::Boolean
    Nest::_exist(nucleus::proton::Address const& address) const
    {
      return (this->_addresses.find(address) != this->_addresses.end());
    }

    void
    Nest::_insert(Pod* pod)
    {
      // Insert the pod in the container.
      if (this->_pods.insert(pod).second == false)
        throw elle::Exception("unable to insert the pod");
    }

    void
    Nest::_map(nucleus::proton::Address const& address,
               Pod* pod)
    {
      // Insert a tuple address/pod in the container.
      if (this->_addresses.insert(
            std::pair<nucleus::proton::Address const, Pod*>(
              address, pod)).second == false)
        throw elle::Exception("unable to insert the address/pod tuple");
    }

    Pod*
    Nest::_lookup(nucleus::proton::Address const& address) const
    {
      // XXX
    }

    void
    Nest::_unmap(nucleus::proton::Address const& address)
    {
      // XXX
    }

    void
    Nest::_optimize()
    {
//       // XXX[100 / 20]
//       elle::Natural32 const limit_high = 0;
//       elle::Natural32 const limit_low = 0;

//       ELLE_TRACE_FUNCTION("");

//       if (this->_placements.size() <= high_limit) // XXX
//         {
//           ELLE_TRACE("the nest does not need to be optimized");
//           return;
//         }

//       ELLE_TRACE("look for unused pod so as to lighten the nest");

//       elle::Boolean traversed = false;

//       while (traversed == false)
//         {
//           traversed = true;

//           for (auto& pair: this->_placements)
//             {
//               auto& placement = pair.first;
//               auto& pod = pair.second;

//               // Handle volatile blocks (i.e just created) which have
//               // been detached.
//               if ((pod->nature == Pod::NatureVolatile) &&
//                   (pod->link == Pod::LinkDetached) &&
//                   (pod->counter == 0))
//                 {
//                   this->_delete(placement);

//                   traversed = false;

//                   break;
//                 }

//               // Ignore pod which are still in use.
//               if (pod->state != Pod::StateUnloaded)
//                 continue;

//               ELLE_ASSERT(pod->counter == 0);

//               // Ignore pod whose block has been unloaded on the storage layer.
//               if (pod->block == nullptr)
//                 continue;

//               // Bind the block.
//               // XXX pod->address = pod->block->bind();

//               // XXX
//             }
//         }
    }

    /*---------.
    | Dumpable |
    `---------*/

    elle::Status
    Nest::Dump(const elle::Natural32 margin) const
    {
      elle::String alignment(margin, ' ');

      // XXX

      return elle::Status::Ok;
    }

    /*-----.
    | Nest |
    `-----*/

    nucleus::proton::Handle
    Nest::attach(nucleus::proton::Contents* block)
    {
      ELLE_TRACE_METHOD(block);
      /* XXX
      ELLE_FINALLY_ACTION_DELETE(block);

      nucleus::proton::Placement placement =
        nucleus::proton::Placement::generate();

      assert(placement != nucleus::proton::Placement::Null);

      // create a new selectionoid.
      auto pod = new Pod(placement, block);

      ELLE_FINALLY_ABORT(block);
      ELLE_FINALLY_ACTION_DELETE(pod);

      // insert the pod.
      this->_insert(pod->placement, pod);

      ELLE_FINALLY_ABORT(pod);

      // XXX[a temporary address for the footprint() to be valid]
      // XXX[use a network argument passed when Nest is instanciated +
      //     family.. et component contents]
      static nucleus::proton::Address some(pod->block->network(),
                                           pod->block->family(),
                                           pod->block->component());
      /// This constant represents the hard-coded length of the secrets
      /// used to encrypt the blocks composing porcupines.
      /// XXX in bytes & to move in Contents where the encryption actually
      ///     takes place?
      /// XXX the length should be parameterable
      static const elle::Natural32 secret_length = 256;
      // XXX[pass info so as to parameter the future size of the key]
      static cryptography::SecretKey secret{
        cryptography::SecretKey::generate(secret_length)};
      static nucleus::proton::Footprint footprint{
        elle::serialize::footprint(some) + elle::serialize::footprint(secret)};

      nucleus::proton::Handle handle(pod->placement, footprint);

      return (handle);
      */
    }

    /// XXX detach then unload
    void
    Nest::detach(nucleus::proton::Handle& handle)
    {
      ELLE_TRACE_METHOD(handle);
      /* XXX
      // make sure placement is non-null.
      assert(handle.placement() != nucleus::proton::Placement::Null);

      // retrieve the pod.
      Pod* pod = this->_retrieve(handle.placement());

      // update the pod's link.
      pod->link = Pod::LinkDetached;

      // Try to optimize the nest.
      this->_optimize();
      */
    }

    void
    Nest::load(nucleus::proton::Handle& handle)
    {
      ELLE_TRACE_METHOD(handle);

      // Act depending on the handle's state.
      switch (handle.state())
        {
        case nucleus::proton::Handle::State::unnested:
          {
            // In this case, this is the first time this handle
            // instance is actually loaded.

            // Note that this does not mean that the nest is not
            // already tracking the associated block. Indeed, a
            // copy of the given handle may have been previously
            // loaded. In this case, try to retrieve the pod associated
            // with the handle address so as to make it track the
            // appropriate egg.
            if (this->_exist(handle.address()) == true)
              {
                // Retrieve the existing pod.
                Pod* pod = this->_lookup(handle.address());

                ELLE_ASSERT(handle.address() == pod->egg()->address());
                ELLE_ASSERT(handle.secret() == pod->egg()->secret());

                // And make the handle track the block's existing egg.
                handle.place(pod->egg());
              }
            else
              {
                // In this case, no egg exists. One must therefore create
                // an egg, encapsulte it in a pod which must be tracked by
                // the nest.

                // Make the handle evolve so as to reference a newly
                // created egg.
                handle.evolve();

                // XXX create a pod and insert it and map it
              }

            break;
          }
        case nucleus::proton::Handle::State::nested:
          {
            // There is nothing to do since the handle is already nest.
            break;
          }
        }

      /* XXX
      std::shared_ptr<nucleus::proton::Contents> block;

      if (handle.placement() != nucleus::proton::Placement::Null)
        {
          ELLE_TRACE("placement present in handle");

          Pod* pod = this->_retrieve(handle.placement());

          block = pod->load(handle);
        }
      else if (handle.address() != nucleus::proton::Address::null())
        {
          ELLE_TRACE("address present in handle");

          if (this->exists(handle.address()) == false)
            {
              ELLE_TRACE("no pod for this address: the block needs "
                         "to be loaded from the depot");

              nucleus::proton::Contents* contents =
                depot::Depot::pull<nucleus::proton::Contents>(
                  handle.address(),
                  nucleus::proton::Revision::Last).release();

              ELLE_FINALLY_ACTION_DELETE(contents);

              contents->decrypt(handle.secret());

              nucleus::proton::Placement placement =
                nucleus::proton::Placement::generate();

              assert(placement != nucleus::proton::Placement::Null);

              // create a new selectionoid.
              auto pod = new Pod(placement, handle.address(), contents);

              ELLE_FINALLY_ABORT(contents);
              ELLE_FINALLY_ACTION_DELETE(pod);

              // insert the pod.
              this->_insert(pod->placement, pod->address, pod);

              ELLE_FINALLY_ABORT(pod);

              handle.placement(pod->placement); // XXX[to change]

              block = pod->load(handle);
            }
          else
            {
              ELLE_TRACE("a pod exists for this address");

              Pod* pod;

              pod = this->_retrieve(handle.address());

              handle.placement(pod->placement); // XXX[to change]

              block = pod->load(handle);
            }
        }
      else
        throw reactor::Exception(elle::concurrency::scheduler(),
                                 "Unable to load a null handle");

      block->node().nest(*this);

      assert(handle.placement() != nucleus::proton::Placement::Null);
      assert(block != nullptr);

      return (block);
      */
    }

    void
    Nest::unload(nucleus::proton::Handle& handle)
    {
      ELLE_TRACE_METHOD(handle);
      /* XXX
      Pod* pod;

      // make sure the given handle is valid.
      assert(handle.placement() != nucleus::proton::Placement::Null);

      pod = this->_retrieve(handle.placement());

      pod->unload(handle);

      // If the block has been detached and is no longer used,
      // Try to optimize the nest.
      this->_optimize();
      */
    }
  }
}
