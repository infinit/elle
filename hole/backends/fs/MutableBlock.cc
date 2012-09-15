#include <hole/backends/fs/MutableBlock.hh>

#include <nucleus/proton/MutableBlock.hh>
#include <nucleus/Nucleus.hh>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("infinit.hole.backends.fs.MutableBlock");

namespace hole
{
  namespace backends
  {
    namespace fs
    {

//
// ---------- construction ----------------------------------------------------
//

      MutableBlock::MutableBlock(nucleus::proton::Network const& network,
                                 nucleus::proton::Address const& address,
                                 nucleus::proton::MutableBlock const& block):
        _network(network),
        _address(address),
        _block(block)
      {
      }

//
// ---------- methods ---------------------------------------------------------
//

      elle::Boolean
      MutableBlock::derives() const
      {
        // does the block already exist.
        if (nucleus::proton::MutableBlock::exists(
              this->_network,
              this->_address,
              nucleus::proton::Version::Last) == false)
          {
            ELLE_TRACE_SCOPE("this seems to be the first version of the "
                             "mutable block");
            return true;
          }

        nucleus::proton::MutableBlock* current;

        // build a block according to the component.
        if (nucleus::Nucleus::Factory.Build(
              this->_address.component,
              current) == elle::Status::Error)
          throw std::runtime_error("unable to build the block");

        std::unique_ptr<nucleus::proton::MutableBlock> guard(current);

        ELLE_TRACE_SCOPE("the mutable block seems to exist "
                         "locally: make sure it derives the "
                         "current version");

        // load the latest version.
        current->load(this->_network,
                      this->_address,
                      nucleus::proton::Version::Last);

        ELLE_LOG("Current block version %s and given block version is %s",
                 current->version, this->_block.version);

        if (this->_block.version != current->version)
          return this->_block.derives(*current);

        ELLE_LOG("Block have same version, we need to distinguish them as the"
                 " latest");

        // We check if contents are the same.
        {
          elle::utility::Buffer b1;
          b1.Writer() << this->_block;

          elle::utility::Buffer b2;
          b2.Writer() << *current;

          /// We need to choose arbitrarly one block as the latest. For that we
          /// decide to compare their content, so we got the same result on
          /// every hosts.
          if (b1 == b2)
            {
              ELLE_TRACE("Both block contents match. Let's say the remote one "
                         "doesn't derive the local one");
              return false;
            }

          ELLE_WARN("conflict detected: blocks have the same version but"
                    " different content; assuming the latest version is %s",
                    (b1 < b2 ? this->_block : *current));

          return (b1 < b2);
        }
      }

    }
  }
}
