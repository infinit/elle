#include <hole/implementations/local/Machine.hh>
#include <hole/Holeable.hh>
#include <hole/Hole.hh>

#include <nucleus/proton/Address.hh>
#include <nucleus/proton/ImmutableBlock.hh>
#include <nucleus/proton/MutableBlock.hh>
#include <nucleus/proton/Version.hh>
#include <nucleus/neutron/Object.hh>
#include <nucleus/neutron/Access.hh>
#include <nucleus/Nucleus.hh>

#include <elle/log.hh>

#include <Infinit.hh>

namespace hole
{
  namespace implementations
  {
    namespace local
    {

      ELLE_LOG_TRACE_COMPONENT("hole.implementation.local.Machine");
//
// ---------- holeable --------------------------------------------------------
//
      void
      Machine::Put(const nucleus::proton::Address& address,
                   const nucleus::proton::ImmutableBlock& block)
      {
        // Debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::local::Put[Immutable]()\n");

        // Check if the block already exist.
        if (block.Exist(Hole::Implementation->network,
                        address) == elle::Status::True)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "this immutable block seems to already exist");

        // Store the block.
        if (block.Store(Hole::Implementation->network,
                        address) == elle::Status::Error)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "unable to store the block");
      }

      void
      Machine::Put(const nucleus::proton::Address&    address,
                   const nucleus::proton::MutableBlock& block)
      {
        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::local::Put[Mutable]()\n");

        // validate the block, depending on its component.
        //
        // indeed, the Object component requires as additional block for
        // being validated.
        switch (address.component)
          {
          case nucleus::neutron::ComponentObject:
            {
              const nucleus::neutron::Object* object =
                static_cast<const nucleus::neutron::Object*>(&block);
              assert(dynamic_cast<const nucleus::neutron::Object*>(&block) != nullptr);

              // validate the object according to the presence of
              // a referenced access block.
              if (object->meta.access != nucleus::proton::Address::Null)
                {
                  ELLE_LOG_TRACE(
                    "Put nucleus::Object MutableBlock %p"
                    " with a referenced access block",
                    this
                    ) {
                    // Load the access block.
                    std::unique_ptr<nucleus::proton::Block> block
                      (Hole::Pull(object->meta.access, nucleus::proton::Version::Last));
                    std::unique_ptr<nucleus::neutron::Access> access
                      (dynamic_cast<nucleus::neutron::Access*>(block.release()));
                    if (access == nullptr)
                      throw reactor::Exception(elle::concurrency::scheduler(),
                                               "expected an access block");

                    if (object->Validate(address, *access) == elle::Status::Error)
                      throw reactor::Exception(elle::concurrency::scheduler(),
                                               "unable to validate the object");
                  }
                }
              else
                {
                  ELLE_LOG_TRACE(
                      "Put nucleus::Object MutableBlock %p"
                      " with a Null access block",
                      this
                  ) {
                      // validate the object.
                      if (object->Validate(
                            address,
                            nucleus::neutron::Access::Null) == elle::Status::Error)
                        throw reactor::Exception(elle::concurrency::scheduler(),
                                                 "unable to validate the object");
                  }
                }

              break;
            }
          default:
            {
              ELLE_LOG_TRACE("Put common MutableBlock %p", &block)
                {
                  // validate the block through the common interface.
                  if (block.Validate(address) == elle::Status::Error)
                    throw reactor::Exception(elle::concurrency::scheduler(),
                                             "the block seems to be invalid");
                }

              break;
            }
          case nucleus::neutron::ComponentUnknown:
            {
              throw reactor::Exception(elle::concurrency::scheduler(),
                                       elle::sprintf("unknown component '%u'", address.component));
            }
          }

        ELLE_LOG_TRACE("Store the block %p", &block)
          {
            // store the block.
            if (block.Store(Hole::Implementation->network,
                            address) == elle::Status::Error)
              throw reactor::Exception(elle::concurrency::scheduler(), "unable to store the block");
          }

        ELLE_LOG_TRACE("Block %p successfully stored", &block);
      }

      std::unique_ptr<nucleus::proton::Block>
      Machine::Get(const nucleus::proton::Address& address)
      {
        // Debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::local::Get[Immutable]()\n");

        nucleus::proton::ImmutableBlock* block;
        nucleus::Nucleus::Factory.Build(address.component, block);

        // Does the block exist.
        if (block->Exist(Hole::Implementation->network, address) == elle::Status::False)
          throw reactor::Exception(elle::concurrency::scheduler(), "the block does not seem to exist");

        // Load the block.
        if (block->Load(Hole::Implementation->network,
                       address) == elle::Status::Error)
          throw reactor::Exception(elle::concurrency::scheduler(), "unable to load the block");

        // Validate the block.
        if (block->Validate(address) == elle::Status::Error)
          throw reactor::Exception(elle::concurrency::scheduler(), "the block seems to be invalid");

        return std::unique_ptr<nucleus::proton::Block>(block);
      }

      std::unique_ptr<nucleus::proton::Block>
      Machine::Get(const nucleus::proton::Address&    address,
                   const nucleus::proton::Version&    version)
      {
        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::local::Get[Mutable]()\n");

        nucleus::proton::MutableBlock* block;
        nucleus::Nucleus::Factory.Build(address.component, block);

        // does the block exist.
        if (block->Exist(Hole::Implementation->network,
                        address, version) == elle::Status::False)
          throw reactor::Exception(elle::concurrency::scheduler(), "the block does not seem to exist");

        // load the block.
        if (block->Load(Hole::Implementation->network,
                       address, version) == elle::Status::Error)
          throw reactor::Exception(elle::concurrency::scheduler(), "unable to load the block");

        // validate the block, depending on its component.
        //
        // indeed, the Object component requires as additional block for
        // being validated.
        switch (address.component)
          {
          case nucleus::neutron::ComponentObject:
            {
              const nucleus::neutron::Object* object =
                static_cast<const nucleus::neutron::Object*>(block);
              assert(dynamic_cast<const nucleus::neutron::Object*>(block) != nullptr);

              // validate the object according to the presence of
              // a referenced access block.
              if (object->meta.access != nucleus::proton::Address::Null)
                {
                  // Load the access block.
                  std::unique_ptr<nucleus::proton::Block> block
                    (Hole::Pull(object->meta.access, nucleus::proton::Version::Last));
                  std::unique_ptr<nucleus::neutron::Access> access
                    (dynamic_cast<nucleus::neutron::Access*>(block.release()));
                  if (access == nullptr)
                    throw reactor::Exception(elle::concurrency::scheduler(),
                                             "expected an access block");

                  // Validate the object.
                  if (object->Validate(address, *access) == elle::Status::Error)
                    throw reactor::Exception(elle::concurrency::scheduler(), "unable to validate the object");
                }
              else
                {
                  // validate the object.
                  if (object->Validate(
                        address,
                        nucleus::neutron::Access::Null) == elle::Status::Error)
                    throw reactor::Exception(elle::concurrency::scheduler(), "unable to validate the object");
                }

              break;
            }
          default:
            {
              // validate the block through the common interface.
              if (block->Validate(address) == elle::Status::Error)
                throw reactor::Exception(elle::concurrency::scheduler(), "the block seems to be invalid");

              break;
            }
          case nucleus::neutron::ComponentUnknown:
            {
              throw reactor::Exception(elle::concurrency::scheduler(),
                                       elle::sprintf("unknown component '%u'",
                                                     address.component));
            }
          }
        return std::unique_ptr<nucleus::proton::Block>(block);
      }

      void
      Machine::Kill(const nucleus::proton::Address& address)
      {
        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::local::Kill()\n");

        // treat the request depending on the nature of the block which
        // the addres indicates.
        switch (address.family)
          {
          case nucleus::proton::FamilyContentHashBlock:
            {
              // erase the immutable block.
              if (nucleus::proton::ImmutableBlock::Erase(Hole::Implementation->network,
                           address) == elle::Status::Error)
                throw reactor::Exception(elle::concurrency::scheduler(), "unable to erase the block");
              break;
            }
          case nucleus::proton::FamilyPublicKeyBlock:
          case nucleus::proton::FamilyOwnerKeyBlock:
          case nucleus::proton::FamilyImprintBlock:
            {
              // retrieve the mutable block.
              if (nucleus::proton::MutableBlock::Erase(Hole::Implementation->network,
                           address) == elle::Status::Error)
                throw reactor::Exception(elle::concurrency::scheduler(), "unable to erase the block");

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
