#include <fstream>

#include <elle/filesystem/TemporaryFile.hh>

namespace elle
{
  namespace filesystem
  {
    TemporaryFile::TemporaryFile(std::string const& name)
      : _directory(name)
    {
      this->_path = this->_directory.path() / name;
      std::ofstream(this->_path);
    }
  }
}
