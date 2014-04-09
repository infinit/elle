#ifndef ELLE_FILESYSTEM_TEMPORARYDIRECTORY_HH
# define ELLE_FILESYSTEM_TEMPORARYDIRECTORY_HH

# include <string>

# include <elle/attribute.hh>
# include <elle/filesystem.hh>

namespace elle
{
  namespace filesystem
  {
    class TemporaryDirectory
    {
    public:
      TemporaryDirectory();
      TemporaryDirectory(std::string const& name);
      ~TemporaryDirectory();
    private:
      ELLE_ATTRIBUTE_R(boost::filesystem::path, path);
      ELLE_ATTRIBUTE(boost::filesystem::path, root);
    };
  }
}

#endif
