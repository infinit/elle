#include <sys/stat.h>

#include <elle/concept/Fileable.hh>
#include <elle/log.hh>

#include <hole/storage/Directory.hh>

ELLE_LOG_COMPONENT("infinit.hole.storage.Directory");

namespace hole
{
  namespace storage
  {
    /*-------------.
    | Construction |
    `-------------*/

    Directory::Directory(std::string const& root):
      Storage(),
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
      std::string path(_path(identifier));
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

    std::string
    Directory::_path(std::string const& identifier) const
    {
      return this->_root + "/" + identifier + ".blk";
    }
  }
}
