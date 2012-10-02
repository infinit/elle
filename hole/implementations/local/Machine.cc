#include <hole/implementations/local/Machine.hh>
#include <hole/Holeable.hh>
#include <hole/Hole.hh>

#include <nucleus/proton/Address.hh>
#include <nucleus/proton/ImmutableBlock.hh>
#include <nucleus/proton/MutableBlock.hh>
#include <nucleus/proton/Revision.hh>
#include <nucleus/neutron/Object.hh>
#include <nucleus/neutron/Access.hh>
#include <nucleus/Nucleus.hh>

#include <elle/log.hh>

namespace hole
{
  namespace implementations
  {
    namespace local
    {

      ELLE_LOG_COMPONENT("hole.implementation.local.Machine");

      Machine::Machine(hole::Hole& hole)
        : _hole(hole)
      {}

//
// ---------- holeable --------------------------------------------------------
//
      void
      Machine::Put(const nucleus::proton::Address& address,
                   const nucleus::proton::ImmutableBlock& block)
      {
        // Check if the block already exist.
        if (this->_hole.storage().exist(address))
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "this immutable block seems to already "
                                   "exist");

        // Store the block.
        block.store(this->_hole.storage().path(address));
      }

      void
      Machine::Put(const nucleus::proton::Address&    address,
                   const nucleus::proton::MutableBlock& block)
      {
        // validate the block, depending on its component.
        //
        // indeed, the Object component requires as additional block for
        // being validated.
        switch (address.component())
          {
          case nucleus::neutron::ComponentObject:
            {
              const nucleus::neutron::Object* object =
                static_cast<const nucleus::neutron::Object*>(&block);
              assert(dynamic_cast<const nucleus::neutron::Object*>(&block) != nullptr);

              // validate the object according to the presence of
              // a referenced access block.
              if (object->access() != nucleus::proton::Address::null)
                {
                  ELLE_TRACE(
                    "Put nucleus::Object MutableBlock %p"
                    " with a referenced access block",
                    this
                    ) {
                    // Load the access block.
                    std::unique_ptr<nucleus::proton::Block> block
                      (this->_hole.pull(object->access(), nucleus::proton::Revision::Last));
                    std::unique_ptr<nucleus::neutron::Access> access
                      (dynamic_cast<nucleus::neutron::Access*>(block.release()));
                    if (access == nullptr)
                      throw reactor::Exception(elle::concurrency::scheduler(),
                                               "expected an access block");

                    object->validate(address, *access);
                  }
                }
              else
                {
                  ELLE_TRACE(
                      "Put nucleus::Object MutableBlock %p"
                      " with a Null access block",
                      this)
                    {
                      // validate the object.
                      object->validate(address, nucleus::neutron::Access::Null);
                    }
                }

              break;
            }
          default:
            {
              ELLE_TRACE("Put common MutableBlock %p", &block)
                {
                  // validate the block through the common interface.
                  block.validate(address);
                }

              break;
            }
          case nucleus::neutron::ComponentUnknown:
            {
              throw reactor::Exception(elle::concurrency::scheduler(),
                                       elle::sprintf("unknown component '%u'", address.component()));
            }
          }

        ELLE_TRACE("Check if the block derives the current block")
          if (this->_hole.conflict(address, block))
            throw reactor::Exception(
              elle::concurrency::scheduler(),
              "the block does not derive the local one");

        block.store(this->_hole.storage().path(address));
      }

      std::unique_ptr<nucleus::proton::Block>
      Machine::Get(const nucleus::proton::Address& address)
      {
        nucleus::proton::ImmutableBlock* block;
        nucleus::factory().Build(address.component(), block);

        // Does the block exist.
        if (!this->_hole.storage().exist(address))
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "the block does not seem to exist");

        // Load the block.
        block->Block::load(this->_hole.storage().path(address));

        // Validate the block.
        block->validate(address);

        return std::unique_ptr<nucleus::proton::Block>(block);
      }

      std::unique_ptr<nucleus::proton::Block>
      Machine::Get(const nucleus::proton::Address&    address,
                   const nucleus::proton::Revision&    revision)
      {
        nucleus::proton::MutableBlock* block;
        nucleus::factory().Build(address.component(), block);

        // does the block exist.
        if (!this->_hole.storage().exist(address, revision))
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "the block does not seem to exist");

        // load the block.
        block->load(this->_hole.storage().path(address, revision));

        // validate the block, depending on its component.
        //
        // indeed, the Object component requires as additional block for
        // being validated.
        switch (address.component())
          {
          case nucleus::neutron::ComponentObject:
            {
              const nucleus::neutron::Object* object =
                static_cast<const nucleus::neutron::Object*>(block);
              assert(dynamic_cast<const nucleus::neutron::Object*>(block) != nullptr);

              // validate the object according to the presence of
              // a referenced access block.
              if (object->access() != nucleus::proton::Address::null)
                {
                  // Load the access block.
                  std::unique_ptr<nucleus::proton::Block> block
                    (this->_hole.pull(object->access(), nucleus::proton::Revision::Last));
                  std::unique_ptr<nucleus::neutron::Access> access
                    (dynamic_cast<nucleus::neutron::Access*>(block.release()));
                  if (access == nullptr)
                    throw reactor::Exception(elle::concurrency::scheduler(),
                                             "expected an access block");

                  // Validate the object.
                  object->validate(address, *access);
                }
              else
                {
                  // validate the object.
                  object->validate(address, nucleus::neutron::Access::Null);
                }

              break;
            }
          case nucleus::neutron::ComponentUnknown:
            {
              throw reactor::Exception(elle::concurrency::scheduler(),
                                       elle::sprintf("unknown component '%u'",
                                                     address.component()));
            }
          default:
            {
              // validate the block through the common interface.
              block->validate(address);

              break;
            }
          }
        return std::unique_ptr<nucleus::proton::Block>(block);
      }

      void
      Machine::Kill(const nucleus::proton::Address& address)
      {
        // treat the request depending on the nature of the block which
        // the addres indicates.
        switch (address.family())
          {
          case nucleus::proton::Family::content_hash_block:
            {
              // erase the immutable block.
              nucleus::proton::ImmutableBlock::erase(
                this->_hole.storage().path(address));

              break;
            }
          case nucleus::proton::Family::public_key_block:
          case nucleus::proton::Family::owner_key_block:
          case nucleus::proton::Family::imprint_block:
            {
              // retrieve the mutable block.
              nucleus::proton::MutableBlock::erase(
                this->_hole.storage().path(address));

              break;
            }
          default:
            {
              throw reactor::Exception(elle::concurrency::scheduler(), "unknown block family");
            }
          }
      }

//
// ---------- dumpable --------------------------------------------------------
//

      ///
      /// this method dumps the implementation.
      ///
      elle::Status      Machine::Dump(const elle::Natural32     margin) const
      {
        elle::String    alignment(margin, ' ');

        std::cout << alignment << "[Machine]" << std::endl;

        return elle::Status::Ok;
      }

    }
  }
}
