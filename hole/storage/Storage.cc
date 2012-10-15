#include <elle/cast.hh>

#include <boost/format.hpp>

#include <hole/storage/Storage.hh>

ELLE_LOG_COMPONENT("infinit.hole.storage.Storage");

namespace hole
{
  namespace storage
  {
    /*-------------.
    | Construction |
    `-------------*/

    Storage::Storage()
    {}

    Storage::~Storage()
    {}

    /*--------.
    | Storage |
    `--------*/

    bool
    Storage::exist(Address const& address,
                   nucleus::proton::Revision const& revision) const
    {
      return this->_exist(this->_identifier(address, revision));
    }

    std::string
    Storage::_identifier(Address const& address,
                         nucleus::proton::Revision const& revision) const
    {
      if (revision == nucleus::proton::Revision::Last)
        return address.unique();
      else
        {
          static boost::format format("%s#%s");
          return str(boost::format(format)
                     % address.unique()
                     % revision.number);
        }
    }

    void
    Storage::store(const nucleus::proton::Address& address,
                   const nucleus::proton::ImmutableBlock& block) const
    {
      ELLE_TRACE_SCOPE("Storing immutable '%s' at '%s'.", block, address);

      if (this->_exist(this->_identifier(address)))
        throw elle::Exception("This block seems to already exists.");

      this->_store(address, block);
    }

    void
    Storage::store(const nucleus::proton::Address& address,
                   const nucleus::proton::MutableBlock& block) const
    {
      ELLE_TRACE_SCOPE("Storing mutable '%s' at '%s'.", block, address);

      if (conflict(address, block))
        throw elle::Exception(
          "The block does not derive the local one.");

      this->_store(address, block);
    }

    std::unique_ptr<nucleus::proton::Block>
    Storage::load(nucleus::proton::Address const& address) const
    {
      ELLE_TRACE_SCOPE("%s: retrieving block at @'%s'.", *this, address);

      if (!this->_exist(this->_identifier(address)))
        throw elle::Exception("The block does not seem to exist.");

      return this->_load(address);
    }

    std::unique_ptr<nucleus::proton::Block>
    Storage::load(nucleus::proton::Address const& address,
                  nucleus::proton::Revision const& revision) const
    {
      ELLE_TRACE_SCOPE("%s: retrieving block at @'%s' and revision '%s'.",
                       *this, address, revision);

      if (!this->_exist(this->_identifier(address, revision)))
        throw elle::Exception("The block does not seem to exist.");

      return this->_load(address, revision);
    }

    void
    Storage::erase(nucleus::proton::Address const& address) const
    {
      ELLE_TRACE_SCOPE("%s: deleting block at @'%s'.", *this, address);

      // Check block existence before trying to delete it.
      if(!this->_exist(this->_identifier(address)))
        throw elle::Exception("The block you tried to erase doesn't exist.");

      this->_erase(address);
    }

    /*----------.
    | Utilities |
    `----------*/

    bool
    Storage::conflict(nucleus::proton::Address const& address,
                   nucleus::proton::MutableBlock const& block) const
    {
      // If the block doesn't exist locally, this is the first version
      // we know of and there is no conflict.
      if (!this->_exist(this->_identifier(address)))
        {
          ELLE_DEBUG("This is the first revision of the block.");
          return false;
        }

      ELLE_DEBUG_SCOPE("The mutable block seems to exist "
                       "locally: make sure it derives the "
                       "current revision");

      std::unique_ptr<nucleus::proton::MutableBlock> current =
        elle::cast<nucleus::proton::MutableBlock>::runtime(
          load(address));

      ELLE_DEBUG("Current block revision '%s' and given block revision is '%s'.",
                 current->revision(), block.revision());

      // Check if the block derives from the local block.
      if (block.revision() != current->revision())
        return !block.derives(*current);

      ELLE_DEBUG("Block have same revision, we need to distinguish them "
                 "as the latest.");

      // We check if contents are the same.
      {
        elle::Buffer b1;
        b1.writer() << block;

        elle::Buffer b2;
        b2.writer() << *current;

        /// We need to choose arbitrarly one block as the latest. For that we
        /// decide to compare their content, so we got the same result on
        /// every hosts.
        if (b1 == b2)
          {
            ELLE_TRACE("Both block contents match. Let's say the remote one "
                       "doesn't derive the local one");
            return false;
          }

        ELLE_WARN("Conflict detected: blocks have the same revision but"
                  " different content; assuming the latest revision is '%s'.",
                  (b1 < b2 ? block : *current));

        return (b1 < b2);
      }
    }
  }
}
