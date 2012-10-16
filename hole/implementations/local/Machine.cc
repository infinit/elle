#include <hole/implementations/local/Machine.hh>
#include <hole/Hole.hh>

#include <nucleus/proton/Address.hh>
#include <nucleus/proton/ImmutableBlock.hh>
#include <nucleus/proton/MutableBlock.hh>
#include <nucleus/proton/Revision.hh>
#include <nucleus/neutron/Object.hh>
#include <nucleus/neutron/Access.hh>

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
      Machine::put(nucleus::proton::Address const& address,
                   nucleus::proton::ImmutableBlock const& block)
      {
        this->_hole.storage().store(address, block);
      }

      void
      Machine::put(const nucleus::proton::Address& address,
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
                      (this->_hole.storage().load(object->access(), nucleus::proton::Revision::Last));
                      std::unique_ptr<nucleus::neutron::Access> access
                      (dynamic_cast<nucleus::neutron::Access*>(block.release()));
                    if (access == nullptr)
                      throw elle::Exception("Expected an access block.");

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
              throw elle::Exception(
                elle::sprintf("Unknown component '%u'.", address.component())
                );
            }
          }

        this->_hole.storage().store(address, block);
      }

      std::unique_ptr<nucleus::proton::Block>
      Machine::get(const nucleus::proton::Address& address)
      {
        std::unique_ptr<nucleus::proton::Block> block =
          this->_hole.storage().load(address);

        block->validate(address);

        return block;
      }

      std::unique_ptr<nucleus::proton::Block>
      Machine::get(const nucleus::proton::Address& address,
                   const nucleus::proton::Revision& revision)
      {
        // load the block.
         std::unique_ptr<nucleus::proton::Block> block =
          (this->_hole.storage().load(address, revision));

        // validate the block, depending on its component.
        //
        // indeed, the Object component requires as additional block for
        // being validated.
        switch (address.component())
          {
          case nucleus::neutron::ComponentObject:
            {
              const nucleus::neutron::Object* object =
                static_cast<const nucleus::neutron::Object*>(block.get());
              assert(dynamic_cast<const nucleus::neutron::Object*>(block.get()) != nullptr);

              // validate the object according to the presence of
              // a referenced access block.
              if (object->access() != nucleus::proton::Address::null)
                {
                  // Load the access block.
                  std::unique_ptr<nucleus::proton::Block> accesBlock
                    = this->_hole.storage().load(object->access(),
                                                 nucleus::proton::Revision::Last);
                  std::unique_ptr<nucleus::neutron::Access> access
                    (dynamic_cast<nucleus::neutron::Access*>(accesBlock.release()));
                  if (access == nullptr)
                    throw elle::Exception("Expected an access block.");

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
              throw elle::Exception(elle::sprintf("Unknown component '%u'.",
                                                     address.component()));
            }
          default:
            {
              // validate the block through the common interface.
              block->validate(address);

              break;
            }
          }
        return block;
      }

      void
      Machine::wipe(const nucleus::proton::Address& address)
      {
        // treat the request depending on the nature of the block which
        // the addres indicates.
        // FIXME: why a switch if we call the same method in both case.
        switch (address.family())
          {
          case nucleus::proton::Family::content_hash_block:
            {
              // erase the immutable block.
              this->_hole.storage().erase(address);

              break;
            }
          case nucleus::proton::Family::public_key_block:
          case nucleus::proton::Family::owner_key_block:
          case nucleus::proton::Family::imprint_block:
            {
              // erase the mutable block.
              this->_hole.storage().erase(address);

              break;
            }
          default:
            {
              throw elle::Exception("Unknown block family.");
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
