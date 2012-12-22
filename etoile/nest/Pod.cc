#include <etoile/nest/Pod.hh>

#include <etoile/depot/Depot.hh>

#include <nucleus/proton/Contents.hh>
#include <nucleus/proton/Placement.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Contents.hh>
#include <nucleus/proton/Handle.hh>
#include <nucleus/proton/Revision.hh>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("infinit.etoile.nest.Pod");

namespace etoile
{
  namespace nest
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// XXX
    ///
    Pod::Pod()
    {
    }

    ///
    /// XXX
    ///
    Pod::Pod(const nucleus::proton::Placement& placement,
             nucleus::proton::Contents* block):
      nature(NatureVolatile),
      link(LinkAttached),
      state(StateUnloaded),
      placement(placement),
      address(nucleus::proton::Address::null()),
      block(block),
      counter(0)
    {
    }

    ///
    /// XXX
    ///
    Pod::Pod(const nucleus::proton::Placement& placement,
             const nucleus::proton::Address& address,
             nucleus::proton::Contents* block):
      nature(NaturePersistent),
      link(LinkAttached),
      state(StateUnloaded),
      placement(placement),
      address(address),
      block(block),
      counter(0)
    {
    }

    ///
    /// XXX
    ///
    Pod::Pod(const Pod&                                             element):
      elle::radix::Object(element),

      nature(element.nature),
      link(element.link),
      state(element.state),
      placement(element.placement),
      address(element.address),
      block(element.block),
      counter(element.counter)
    {
      ELLE_ASSERT(false && "XXX is this necessary?");
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    std::shared_ptr<nucleus::proton::Contents>
    Pod::load(nucleus::proton::Handle& handle)
    {
      ELLE_TRACE_METHOD(handle);

      ELLE_TRACE("counter: %s -> %s",
                 this->counter, this->counter + 1);

      ELLE_ASSERT(this->placement == handle.placement());
      ELLE_ASSERT(this->link != Pod::LinkDetached);

      if (this->block == nullptr)
        {
          ELLE_ASSERT(false);
          /* XXX
          // XXX depend on the nature: local or network => probablement en local via placement

          ELLE_ASSERT(this->address != nucleus::proton::Address::Null);

          nucleus::proton::Contents* block;

          block =
            nucleus::Nucleus::factory.allocate<nucleus::proton::Contents>(
              this->address.component);

          // XXX[configure & link]

          // XXX
          block->Dump();

          std::unique_ptr<nucleus::proton::Contents> guard(block);

          if (depot::Depot::Pull(this->address,
                                 nucleus::proton::Revision::Last,
                                 *block) == elle::Status::Error)
            escape("unable to load the block");

          this->block.reset(block);

          guard.release();

          // XXX switch (nodule/value) -> nest(nest) OR Node virtual method link() + configure()
          */
        }

      this->counter++;

      this->state = Pod::StateLoaded;

      ELLE_ASSERT(this->block != nullptr);

      return (this->block);
    }

    ///
    /// XXX
    ///
    void
    Pod::unload(nucleus::proton::Handle& handle)
    {
      ELLE_TRACE_METHOD(handle);

      ELLE_TRACE("counter: %s -> %s",
                 this->counter, this->counter - 1);

      ELLE_ASSERT(this->placement == handle.placement());
      ELLE_ASSERT(this->counter > 0);

      this->counter--;

      if (this->counter == 0)
        this->state = Pod::StateUnloaded;
    }

    ///
    /// XXX
    ///
    void
    Pod::release()
    {
      ELLE_TRACE_METHOD("");

      ELLE_ASSERT(this->counter == 0);

      if (this->block.unique())
        this->block.reset();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(Pod, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an object.
    ///
    elle::Status            Pod::Dump(elle::Natural32               margin) const
    {
      elle::String          alignment(margin, ' ');

      std::cout << alignment << "[Pod] " << std::hex << this << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift << "[Nature] "
                << std::dec << this->nature << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift << "[State] "
                << std::dec << this->state << std::endl;

      if (this->placement.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the placement");

      if (this->address.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the address");

      std::cout << alignment << elle::io::Dumpable::Shift << "[Block] "
                << this->block.get() << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift << "[Counter] "
                << std::dec << this->counter << std::endl;

      return elle::Status::Ok;
    }

  }
}
