#include <sys/stat.h>

#include <elle/log.hh>
#include <elle/finally.hh>
#include <elle/concept/Fileable.hh>
#include <elle/serialize/Serializable.hh>
#include <elle/serialize/insert.hh>
#include <elle/serialize/extract.hh>

#include <hole/storage/Directory.hh>

#include <nucleus/factory.hh>
#include <nucleus/fwd.hh>

ELLE_LOG_COMPONENT("infinit.hole.storage.Directory");

namespace hole
{
  namespace storage
  {
    /*-------------.
    | Construction |
    `-------------*/

    Directory::Directory(nucleus::proton::Network const& network,
                         std::string const& root):
      Storage(network),
      _root(root)
    {}

    Directory::~Directory()
    {}

    /*----------.
    | Printable |
    `----------*/

    void
    Directory::print(std::ostream& stream) const
    {
      stream << "Directory " << this->_root;
    }

    /*--------.
    | Storage |
    `--------*/

    bool
    Directory::_exist(std::string const& identifier) const
    {
      std::string path(this->path(identifier));

      ELLE_TRACE_SCOPE("%s: check if %s exists", *this, identifier);

      // XXX: duplicated from elle::io::File::Exist
      struct ::stat stat;
      if (::stat(path.c_str(), &stat) != 0)
        {
          return false;
        }
      if (!S_ISREG(stat.st_mode))
        return false;
      return true;
    }

    void
    Directory::_store(const nucleus::proton::Address& address,
                      const nucleus::proton::ImmutableBlock& block)
    {
      elle::io::Path path(this->path(address));

      if (elle::io::File::Dig(path) == elle::Status::Error)
        throw std::runtime_error(
          elle::sprintf("Unable to dig the path '%s'.", path));

      // Serialize the block.
      elle::serialize::to_file(path.string()) << block;
    }

    void
    Directory::_store(const nucleus::proton::Address& address,
                      const nucleus::proton::MutableBlock& block)

    {
      // Generate path.
      elle::io::Path path(this->path(address));

      // Check path status.
      if (elle::io::File::Dig(path) == elle::Status::Error)
        throw std::runtime_error(
          elle::sprintf("Unable to dig the path '%s'.", path));

      // Serialize the block.
      elle::serialize::to_file(path.string()) << block;
    }

    std::unique_ptr<nucleus::proton::Block>
    Directory::_load(nucleus::proton::Address const& address) const
    {
      // Get block path.
      elle::io::Path path(this->path(address));

      // Create an empty block.
      nucleus::proton::ImmutableBlock* block{
        nucleus::factory::block().allocate<nucleus::proton::ImmutableBlock>(
          address.component())};

      ELLE_FINALLY_ACTION_DELETE(block);

      // Deserialize the block.
      elle::serialize::from_file(path.string()) >> *block;

      ELLE_FINALLY_ABORT(block);

      return std::unique_ptr<nucleus::proton::Block>(block);
    }

    std::unique_ptr<nucleus::proton::Block>
    Directory::_load(nucleus::proton::Address const& address,
                     nucleus::proton::Revision const& revision) const
    {
      // Get block path.
      elle::io::Path path(this->path(address, revision));

      // Create an empty block.
      nucleus::proton::MutableBlock* block{
        nucleus::factory::block().allocate<nucleus::proton::MutableBlock>(
          address.component())};

      ELLE_FINALLY_ACTION_DELETE(block);

      // Deserialize the block.
      elle::serialize::from_file(path.string()) >> *block;

      ELLE_FINALLY_ABORT(block);

      return std::unique_ptr<nucleus::proton::Block>(block);
    }

    void
    Directory::_erase(nucleus::proton::Address const& address)
    {
      // Get block path.
      elle::io::Path path(this->path(address));

      // Delete block.
      if (elle::io::File::Erase(path) == elle::Status::Error)
        throw std::runtime_error(
          elle::sprintf("Unable to erase the file '%s'.", path));
    }

    /*----------.
    | Utilities |
    `----------*/
    std::string
    Directory::path(nucleus::proton::Address const& address,
                    nucleus::proton::Revision const& revision) const
    {
      std::string id(this->_identifier(address, revision));
      return path(id);
    }

    std::string
    Directory::path(std::string const& identifier) const
    {
      return this->_root + "/" + identifier + ".blk";
    }
  }
}
