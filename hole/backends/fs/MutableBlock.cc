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
              nucleus::proton::Version::Last) == true)
          {
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

            return (this->_block.derives(*current));
          }
        else
          {
            ELLE_TRACE_SCOPE("this seems to be the first version of the "
                             "mutable block");
            return (true);
          }
      }

    }
  }
}
