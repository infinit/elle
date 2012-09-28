#include <cassert>

#include <boost/format.hpp>

#include <hole/storage/Storage.hh>

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
    Storage::path(Address const& address,
                  nucleus::proton::Revision const& revision) const
    {
      std::string id(this->_identifier(address, revision));
      return this->_path(id);
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
  }
}
