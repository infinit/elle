#include <elle/system/PIDFile.hh>

#include <boost/filesystem/fstream.hpp>

#include <elle/err.hh>
#include <elle/printf.hh>

namespace elle
{
  PIDFile::PIDFile(boost::filesystem::path path)
    : _path(std::move(path))
  {
    boost::filesystem::ofstream ofs(this->_path);
    ofs <<
#ifdef INFINIT_WINDOWS
      ::GetCurrentProcessId()
#else
      ::getpid()
#endif
      ;
  }

  PIDFile::~PIDFile()
  {
    boost::filesystem::remove(this->_path);
  }

  int
  PIDFile::read(boost::filesystem::path const& path)
  {
    boost::filesystem::ifstream ifs(path);
    if (!ifs.good())
      elle::err("unable to open %s for reading", path);
    int pid = -1;
    if (!(ifs >> pid))
      elle::err("unable to read PID from %s", path);
    return pid;
  }
}
