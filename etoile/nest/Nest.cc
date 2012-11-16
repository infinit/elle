#include <etoile/nest/Nest.hh>
#include <etoile/gear/Transcript.hh>
#include <etoile/gear/Action.hh>
#include <etoile/depot/Depot.hh>

#include <elle/concurrency/Scheduler.hh>

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
            case Pod::NaturePersistent:
              {
                switch (pod->block.get()->state())
                  {
                  case nucleus::proton::StateClean:
                    {
                      // nothing to do: the block has not been modified.
                      break;
                    }
                  case nucleus::proton::StateDirty:
                    {
                      throw reactor::Exception(elle::concurrency::scheduler(),
                                               "The block is dirty but has not"
                                               "been sealed.");
                    }
                  case nucleus::proton::StateConsistent:
                    {
                      pod->address = pod->block.get()->bind();

                      /* XXX[porcupine]
                      if (transcript.Record(
                            new gear::Action(
                              pod->address,
                              pod->block)) == elle::Status::Error)
                        throw reactor::Exception(elle::concurrency::scheduler(),
                                                 "Unable to record the action.");
                      */
                    }
                  }

                break;
              }
            case Pod::NatureOrphan:
              {
                if (pod->address != nucleus::proton::Address::null())
                  {
                    /* XXX[porcupine]
                    if (transcript.Record(
                          new gear::Action(
                            pod->address)) == elle::Status::Error)
                      throw reactor::Exception(elle::concurrency::scheduler(),
                                               "Unable to record the action.");
                    */
                  }

                break;
              }
            case Pod::NatureUnknown:
              {
                throw reactor::Exception(elle::concurrency::scheduler(),
                                         "Unknown pod nature.");
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
      Nest::P::Scoutor scoutor;

      ELLE_TRACE_SCOPE("_retrieve(placement)");

      if ((scoutor = this->_placements.find(placement)) == this->_placements.end())
        throw reactor::Exception(elle::concurrency::scheduler(),
                                 "Unable to locate a pod for the given placement");
      assert(scoutor->second != nullptr);

      return (scoutor->second);
    }

    Pod*
    Nest::_retrieve(nucleus::proton::Address const& address) const
    {
      Nest::A::Scoutor scoutor;

      ELLE_TRACE_SCOPE("_retrieve(address)");

      if ((scoutor = this->_addresses.find(address)) == this->_addresses.end())
        throw reactor::Exception(elle::concurrency::scheduler(),
                                 "Unable to locate a pod for the given address");

      assert(scoutor->second != nullptr);

      return (scoutor->second);
    }

    void
    Nest::_delete(nucleus::proton::Placement const& placement)
    {
      Pod* pod;

      ELLE_TRACE_SCOPE("_delete(placement)");

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
      Pod* pod;

      ELLE_TRACE_SCOPE("_delete(address)");

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
    Nest::attach(std::unique_ptr<nucleus::proton::Contents>&& block)
    {
      ELLE_TRACE_SCOPE("attach(%s)", block.get());

      nucleus::proton::Placement placement =
        nucleus::proton::Placement::generate();

      assert(placement != nucleus::proton::Placement::Null);

      // create a new selectionoid.
      auto pod =
        std::unique_ptr<Pod>(
          new Pod(placement,
                  std::shared_ptr<nucleus::proton::Contents>(block.release())));

      // insert the pod.
      this->_insert(pod.get()->placement, pod.get());

      nucleus::proton::Handle handle;
      handle.appoint(pod.get()->placement);

      // release track.
      pod.release();

      return (handle);
    }

    /// XXX detach then unload
    void
    Nest::detach(nucleus::proton::Handle& handle)
    {
      ELLE_TRACE_SCOPE("detach(%s)", &handle);

      Pod* pod;

      // make sure placement is non-null.
      assert(handle.placement() != nucleus::proton::Placement::Null);

      // retrieve the pod.
      pod = this->_retrieve(handle.placement());

      // update the pod's nature.
      pod->nature = Pod::NatureOrphan;

      if ((pod->nature == Pod::NatureOrphan) &&
          (pod->counter == 0))
        pod->release();
    }

    std::shared_ptr<nucleus::proton::Contents>
    Nest::load(nucleus::proton::Handle& handle)
    {
      std::shared_ptr<nucleus::proton::Contents> block;

      ELLE_TRACE_SCOPE("load(%s)", &handle);

      // make sure the given handle is valid.
      assert(handle != nucleus::proton::Handle::Null);

      if (handle.placement() != nucleus::proton::Placement::Null)
        {
          Pod* pod;

          ELLE_TRACE("Placement present in handle");

          pod = this->_retrieve(handle.placement());

          block = pod->load(handle);
        }
      else if (handle.address() != nucleus::proton::Address::null())
        {
          ELLE_TRACE("Address present in handle");

          if (this->exists(handle.address()) == false)
            {
              ELLE_TRACE("No pod for this address: the block needs "
                         "to be loaded from the depot");

              /* XXX
                input: H1
                1) allocate contents
                2) load from depot
                3) decrypt with key => will use factory to allocate the correct node
                4) attach block to this nest -> handle H2
                5) H1.placement(H2.placement());
              */

              std::unique_ptr<nucleus::proton::Contents> contents =
                depot::Depot::pull<nucleus::proton::Contents>(
                  handle.address(),
                  nucleus::proton::Revision::Last);

              assert(handle.secret() != cryptography::SecretKey::Null);

              if (contents->decrypt(handle.secret()) == elle::Status::Error)
                throw reactor::Exception(elle::concurrency::scheduler(),
                                         "Unable to decrypt the contents");

              nucleus::proton::Handle h = this->attach(std::move(contents));

              handle.appoint(h.placement());

              Pod* pod = this->_retrieve(handle.placement());

              block = pod->load(handle);
            }
          else
            {
              assert(false);
              /* XXX
              Pod* pod;

              ELLE_TRACEE("A pod exists for this address");

              pod = this->_retrieve(handle.address());

              block = pod->load(handle);
              */
              // XXX here the handle should reference the pod's placement: handle.placement(pod.placement());
            }
        }
      else
        throw reactor::Exception(elle::concurrency::scheduler(),
                                 "Unable to load a null handle");

      block->node()->nest(*this);

      assert(handle.placement() != nucleus::proton::Placement::Null);
      assert(block != nullptr);

      return (block);
    }

    void
    Nest::unload(nucleus::proton::Handle& handle)
    {
      ELLE_TRACE_SCOPE("unload(%s)", &handle);

      Pod* pod;

      // make sure the given handle is valid.
      assert(handle.placement() != nucleus::proton::Placement::Null);

      pod = this->_retrieve(handle.placement());

      pod->unload(handle);

      if ((pod->nature == Pod::NatureOrphan) &&
          (pod->counter == 0))
        pod->release();
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
