#include <etoile/nest/Nest.hh>
#include <etoile/gear/Transcript.hh>
#include <etoile/gear/Action.hh>
#include <etoile/depot/Depot.hh>

#include <elle/concurrency/Scheduler.hh>
#include <elle/finally.hh>

#include <cryptography/SecretKey.hh>

#include <nucleus/proton/Nest.hh>
#include <nucleus/proton/Handle.hh>
#include <nucleus/proton/Placement.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Revision.hh>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("infinit.etoile.nest.Nest");

namespace etoile
{
  namespace nest
  {

//
// ---------- constructors & destructors --------------------------------------
//

    Nest::Nest(nucleus::proton::Limits const& limits):
      nucleus::proton::Nest(limits)
    {
    }

    Nest::~Nest()
    {
      this->clear();
    }

//
// ---------- methods ---------------------------------------------------------
//

    elle::Boolean
    Nest::exists(nucleus::proton::Placement const& placement) const
    {
      if (this->_placements.find(placement) == this->_placements.end())
        return false;

      return true;
    }

    elle::Boolean
    Nest::exists(nucleus::proton::Address const& address) const
    {
      if (this->_addresses.find(address) == this->_addresses.end())
        return false;

      return true;
    }

    void
    Nest::clear()
    {
      while (this->_placements.size() != 0)
        {
          auto iterator = this->_placements.begin();
          Pod* pod = iterator->second;

          this->_delete(pod->placement);
        }
    }

    void
    Nest::record(gear::Transcript& transcript)
    {
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
    }

    void
    Nest::_insert(nucleus::proton::Placement const& placement,
                  Pod* pod)
    {
      std::pair<Nest::P::Iterator, elle::Boolean>   result;

      // insert the pod in the container.
      result =
        this->_placements.insert(
          std::pair<const nucleus::proton::Placement, Pod*>(placement, pod));

      // check if the insertion was successful.
      if (result.second == false)
        throw reactor::Exception(elle::concurrency::scheduler(),
                                 "Unable to insert the pod in the container");
    }

    void
    Nest::_insert(nucleus::proton::Placement const& placement,
                  nucleus::proton::Address const& address,
                  Pod* pod)
    {
      struct
      {
        std::pair<Nest::A::Iterator, elle::Boolean>         address;
        std::pair<Nest::P::Iterator, elle::Boolean>         placement;
      }                                                     result;

      // insert the pod in the container.
      result.placement =
        this->_placements.insert(
          std::pair<const nucleus::proton::Placement, Pod*>(placement, pod));

      // check if the insertion was successful.
      if (result.placement.second == false)
        throw reactor::Exception(elle::concurrency::scheduler(),
                                 "Unable to insert the pod in the container");

      // insert the pod in the container.
      result.address =
        this->_addresses.insert(
          std::pair<const nucleus::proton::Address, Pod*>(address, pod));

      // check if the insertion was successful.
      if (result.address.second == false)
        throw reactor::Exception(elle::concurrency::scheduler(),
                                 "Unable to insert the pod in the container");
    }

    Pod*
    Nest::_retrieve(nucleus::proton::Placement const& placement) const
    {
      ELLE_TRACE_METHOD(placement);

      Nest::P::Scoutor scoutor;

      if ((scoutor = this->_placements.find(placement)) ==
          this->_placements.end())
        throw reactor::Exception(elle::concurrency::scheduler(),
                                 "unable to locate a pod for the given "
                                 "placement");
      assert(scoutor->second != nullptr);

      return (scoutor->second);
    }

    Pod*
    Nest::_retrieve(nucleus::proton::Address const& address) const
    {
      ELLE_TRACE_METHOD(address);

      Nest::A::Scoutor scoutor;

      if ((scoutor = this->_addresses.find(address)) == this->_addresses.end())
        throw reactor::Exception(elle::concurrency::scheduler(),
                                 "Unable to locate a pod for the given address");

      assert(scoutor->second != nullptr);

      return (scoutor->second);
    }

    void
    Nest::_delete(nucleus::proton::Placement const& placement)
    {
      ELLE_TRACE_METHOD(placement);

      Pod* pod;

      //
      // retrieve the pod.
      //

      pod = this->_retrieve(placement);

      //
      // remove the pod from the containers.
      //

      if (pod->address != nucleus::proton::Address::null())
        this->_addresses.erase(pod->address);

      this->_placements.erase(pod->placement);

      //
      // delete the pod.
      //

      delete pod;
    }

    void
    Nest::_delete(nucleus::proton::Address const& address)
    {
      ELLE_TRACE_METHOD(address);

      Pod* pod;

      //
      // retrieve the pod.
      //

      pod = this->_retrieve(address);

      assert(pod->placement != nucleus::proton::Placement::Null);

      //
      // remove the pod from the containers.
      //

      this->_addresses.erase(pod->address);

      this->_placements.erase(pod->placement);

      //
      // delete the pod.
      //

      delete pod;
    }

//
// ---------- interafaces -----------------------------------------------------
//

    nucleus::proton::Handle const
    Nest::attach(nucleus::proton::Contents* block)
    {
      ELLE_TRACE_METHOD(block);

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

      // XXX[future] nucleus::proton::Handle handle(pod->placement, footprint);
      nucleus::proton::Handle handle(pod->placement, some, secret);

      return (handle);
    }

    /// XXX detach then unload
    void
    Nest::detach(nucleus::proton::Handle& handle)
    {
      ELLE_TRACE_METHOD(handle);

      // make sure placement is non-null.
      assert(handle.placement() != nucleus::proton::Placement::Null);

      // retrieve the pod.
      Pod* pod = this->_retrieve(handle.placement());

      // update the pod's link.
      pod->link = Pod::LinkDetached;

      // Try to optimize the nest.
      this->_optimize();
    }

    std::shared_ptr<nucleus::proton::Contents>
    Nest::load(nucleus::proton::Handle& handle)
    {
      ELLE_TRACE_METHOD(handle);

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
    }

    void
    Nest::unload(nucleus::proton::Handle& handle)
    {
      ELLE_TRACE_METHOD(handle);

      Pod* pod;

      // make sure the given handle is valid.
      assert(handle.placement() != nucleus::proton::Placement::Null);

      pod = this->_retrieve(handle.placement());

      pod->unload(handle);

      // If the block has been detached and is no longer used,
      // Try to optimize the nest.
      this->_optimize();
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

//
// ---------- dumpable --------------------------------------------------------
//

    elle::Status            Nest::Dump(const elle::Natural32    margin) const
    {
      elle::String alignment(margin, ' ');
      auto i = this->_placements.begin();
      auto e = this->_placements.end();

      std::cout << alignment << "[Nest]" << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift << "[_Placements] #"
                << this->_placements.size() << std::endl;

      for (; i != e; ++i)
        {
          if (i->second->Dump(margin + 4) == elle::Status::Error)
            escape("unable to dump the pod");
        }

      auto                  j = this->_addresses.begin();
      auto                  f = this->_addresses.end();

      std::cout << alignment << elle::io::Dumpable::Shift << "[_Addresses] #"
                << this->_addresses.size() << std::endl;

      for (; j != f; ++j)
        {
          std::cout << alignment << elle::io::Dumpable::Shift
                    << elle::io::Dumpable::Shift
                    << "[Pod] " << std::hex << j->second << std::endl;
        }

      return elle::Status::Ok;
    }

  }
}
