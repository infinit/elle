#include <hole/storage/Memory.hh>

#include <elle/log.hh>
#include <elle/finally.hh>
#include <elle/Exception.hh>
#include <elle/serialize/Serializable.hh>
#include <elle/io/Unique.hh>

#include <nucleus/factory.hh>
#include <nucleus/fwd.hh>

#include <boost/format.hpp>

#include <sstream>

ELLE_LOG_COMPONENT("infinit.hole.storage.Memory");

namespace hole
{
  namespace storage
  {
    /*-------------.
    | Construction |
    `-------------*/

    Memory::Memory(nucleus::proton::Network const& network):
      Storage(network)
    {}

    Memory::~Memory()
    {
      this->_container.clear();
    }

    /*--------.
    | Methods |
    `--------*/

    elle::Boolean
    Memory::empty() const
    {
      return (this->_container.empty());
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Memory::dump() const
    {
      std::cout << "[Memory] #" << this->_container.size() << std::endl;

      for (auto& pair: this->_container)
        {
          std::cout << "  " << pair.first << std::endl;
        }
    }

    void
    Memory::print(std::ostream& stream) const
    {
      stream << "#" << this->_container.size();
    }

    /*--------.
    | Storage |
    `--------*/

    bool
    Memory::_exist(std::string const& identifier) const
    {
      ELLE_TRACE_METHOD(identifier);

      return (this->_container.find(identifier) != this->_container.end());
    }

    void
    Memory::_store(const nucleus::proton::Address& address,
                   const nucleus::proton::ImmutableBlock& block)
    {
      ELLE_TRACE_METHOD(address, block);

      // Convert the address and block into strings.
      elle::io::Unique unique_address{address.unique()};
      std::ostringstream stream(std::ios_base::out | std::ios_base::binary);
      // XXX[to improve: contact Raphael]
      static_cast<elle::serialize::Serializable<elle::serialize::BinaryArchive> const&>(block).serialize(stream);

      // Insert in the container.
      auto result =
        this->_container.insert(
          std::pair<elle::String const, elle::String const>(unique_address,
                                                            stream.str()));

      // Check if the insertion was successful.
      if (result.second == false)
        throw elle::Exception("unable to insert the pair address/block "
                              "in the container");
    }

    void
    Memory::_store(const nucleus::proton::Address& address,
                   const nucleus::proton::MutableBlock& block)
    {
      ELLE_TRACE_METHOD(address, block);

      // Convert the address and block into strings.
      elle::io::Unique unique_address{address.unique()};
      std::ostringstream stream(std::ios_base::out | std::ios_base::binary);
      // XXX[to improve: contact Raphael]
      static_cast<elle::serialize::Serializable<elle::serialize::BinaryArchive> const&>(block).serialize(stream);

      // Insert in the container.
      auto result =
        this->_container.insert(
          std::pair<elle::String const, elle::String const>(unique_address,
                                                            stream.str()));

      // Check if the insertion was successful.
      if (result.second == false)
        throw elle::Exception("unable to insert the pair address/block "
                              "in the container");
    }

    std::unique_ptr<nucleus::proton::Block>
    Memory::_load(nucleus::proton::Address const& address) const
    {
      ELLE_TRACE_METHOD(address);

      // Convert the address to a string.
      elle::io::Unique unique_address{address.unique()};

      ELLE_ASSERT(this->_exist(unique_address) == true);

      // Create an empty block.
      nucleus::proton::ImmutableBlock* block{
        nucleus::factory::block().allocate<nucleus::proton::ImmutableBlock>(
          address.component())};

      ELLE_FINALLY_ACTION_DELETE(block);

      // Open an input stream.
      std::istringstream stream(this->_container.find(unique_address)->second,
                                std::ios_base::in | std::ios_base::binary);

      // Deserialize the block from the steam.
      // XXX[to improve: contact Raphael]
      static_cast<elle::serialize::Serializable<elle::serialize::BinaryArchive>*>(block)->deserialize(stream);

      ELLE_FINALLY_ABORT(block);

      return std::unique_ptr<nucleus::proton::Block>(block);
    }

    std::unique_ptr<nucleus::proton::Block>
    Memory::_load(nucleus::proton::Address const& address,
                  nucleus::proton::Revision const& revision) const
    {
      ELLE_TRACE_METHOD(address, revision);

      // Convert the address to a string.
      elle::io::Unique unique_address{address.unique()};

      static boost::format format("%s#%s");
      elle::String unique(str(boost::format(format)
                              % unique_address
                              % revision.number));

      ELLE_ASSERT(this->_exist(unique) == true);

      // Create an empty block.
      nucleus::proton::ImmutableBlock* block{
        nucleus::factory::block().allocate<nucleus::proton::ImmutableBlock>(
          address.component())};

      ELLE_FINALLY_ACTION_DELETE(block);

      // Open an input stream.
      std::stringstream stream(this->_container.find(unique)->second,
                               std::ios_base::in | std::ios_base::binary);

      // Deserialize the block from the steam.
      // XXX[to improve: contact Raphael]
      static_cast<elle::serialize::Serializable<elle::serialize::BinaryArchive>*>(block)->deserialize(stream);

      ELLE_FINALLY_ABORT(block);

      return std::unique_ptr<nucleus::proton::Block>(block);
    }

    void
    Memory::_erase(nucleus::proton::Address const& address)
    {
      ELLE_TRACE_METHOD(address);

      // Convert the address to a string.
      elle::io::Unique unique_address{address.unique()};

      ELLE_ASSERT(this->_exist(unique_address) == true);

      this->_container.erase(unique_address);
    }
  }
}
