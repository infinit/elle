#include <hole/storage/MainMemory.hh>

#include <elle/log.hh>
#include <elle/finally.hh>
#include <elle/Exception.hh>
#include <elle/serialize/Serializable.hh>
#include <elle/io/Unique.hh>

#include <nucleus/nucleus.hh>
#include <nucleus/fwd.hh>

#include <boost/format.hpp>

#include <sstream>

ELLE_LOG_COMPONENT("infinit.hole.storage.MainMemory");

namespace hole
{
  namespace storage
  {
    /*-------------.
    | Construction |
    `-------------*/

    MainMemory::MainMemory():
      Storage()
    {}

    MainMemory::~MainMemory()
    {
      this->_container.clear();
    }

    /*----------.
    | Printable |
    `----------*/

    void
    MainMemory::dump() const
    {
      std::cout << "MainMemory #" << this->_container.size() << std::endl;

      for (auto& pair: this->_container)
        {
          std::cout << "  " << pair.first << std::endl;
        }
    }

    void
    MainMemory::print(std::ostream& stream) const
    {
      stream << "MainMemory #" << this->_container.size();
    }

    /*--------.
    | Storage |
    `--------*/

    bool
    MainMemory::_exist(std::string const& identifier) const
    {
      ELLE_TRACE_METHOD(identifier);

      return (this->_container.find(identifier) != this->_container.end());
    }

    void
    MainMemory::_store(const nucleus::proton::Address& address,
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
    MainMemory::_store(const nucleus::proton::Address& address,
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
    MainMemory::_load(nucleus::proton::Address const& address) const
    {
      ELLE_TRACE_METHOD(address);

      // Convert the address to a string.
      elle::io::Unique unique_address{address.unique()};

      // Create an empty block.
      nucleus::proton::ImmutableBlock* block{
        nucleus::factory().allocate<nucleus::proton::ImmutableBlock>(
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
    MainMemory::_load(nucleus::proton::Address const& address,
                      nucleus::proton::Revision const& revision) const
    {
      ELLE_TRACE_METHOD(address, revision);

      // Convert the address to a string.
      elle::io::Unique unique_address{address.unique()};

      static boost::format format("%s#%s");
      elle::String unique(str(boost::format(format)
                              % unique_address
                              % revision.number));

      // Create an empty block.
      nucleus::proton::ImmutableBlock* block{
        nucleus::factory().allocate<nucleus::proton::ImmutableBlock>(
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
    MainMemory::_erase(nucleus::proton::Address const& address)
    {
      ELLE_TRACE_METHOD(address);

      // Convert the address to a string.
      elle::io::Unique unique_address{address.unique()};

      this->_container.erase(unique_address);
    }
  }
}
