#include <etoile/nest/Pod.hh>

#include <etoile/depot/Depot.hh>

#include <nucleus/Nucleus.hh>
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
    Pod::Pod():
      nature(NatureUnknown),
      state(StateUnloaded),
      block(nullptr),
      counter(0)
    {
    }

    ///
    /// XXX
    ///
    Pod::Pod(const nucleus::proton::Placement&                              placement,
             std::shared_ptr<nucleus::proton::Contents>                        block):
      nature(NatureVolatile),
      state(StateUnloaded),
      placement(placement),
      block(block),
      counter(0)
    {
    }

    ///
    /// XXX
    ///
    Pod::Pod(const nucleus::proton::Placement&                              placement,
             const nucleus::proton::Address&                                address):
      nature(NaturePersistent),
      state(StateUnloaded),
      placement(placement),
      address(address),
      block(nullptr),
      counter(0)
    {
    }

    ///
    /// XXX
    ///
    Pod::Pod(const Pod&                                             element):
      elle::radix::Object(element),

      nature(element.nature),
      state(element.state),
      placement(element.placement),
      address(element.address),
      block(element.block),
      counter(element.counter)
    {
      assert(false && "XXX is this necessary?");
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
      ELLE_TRACE_SCOPE("load() this(%s) handle(%s) [%s -> %s]",
                       this, &handle, this->counter, this->counter + 1);

      assert(this->placement == handle.placement());
      assert(this->nature != Pod::NatureOrphan);

      // XXX
      if (this->nature == Pod::NatureOrphan)
        throw reactor::Exception(elle::concurrency::scheduler(),
                                 "XXX");

      if (this->block == nullptr)
        {
          /* XXX
          assert(false);

          // XXX depend on the nature: local or network => probablement en local via placement

          assert(this->address != nucleus::proton::Address::Null);

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

      assert(this->block != nullptr);

      return (this->block);
    }

    ///
    /// XXX
    ///
    void
    Pod::unload(nucleus::proton::Handle& handle)
    {
      ELLE_TRACE_SCOPE("unload() this(%s) handle(%s) [%s -> %s]",
                       this, &handle, this->counter, this->counter - 1);

      assert(this->placement == handle.placement());
      assert(this->counter > 0);

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
      ELLE_TRACE_SCOPE("release() this(%s)",
                       this);

      assert(this->counter == 0);

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

      if (this->block != nullptr)
        {
          // XXX
          //if (this->block->Dump(margin + 2) == elle::Status::Error)
          //escape("unable to dump the block");
        }
      else
        {
          std::cout << alignment << elle::io::Dumpable::Shift << "[Block] "
                    << elle::none << std::endl;
        }

      std::cout << alignment << elle::io::Dumpable::Shift << "[Counter] "
                << std::dec << this->counter << std::endl;

      return elle::Status::Ok;
    }

  }
}
