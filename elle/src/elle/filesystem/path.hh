#ifndef ELLE_FILESYSTEM_PATH_SERIALIZATION_HH
# define ELLE_FILESYSTEM_PATH_SERIALIZATION_HH

# include <boost/filesystem/path.hpp>

# include <elle/serialization/Serializer.hh>

namespace elle
{
  namespace serialization
  {
    template<>
    struct ELLE_API Serialize<boost::filesystem::path>
    {
      typedef std::string Type;
      static
      Type
      convert(boost::filesystem::path& path);

      static
      boost::filesystem::path
      convert(Type const& repr);
    };
  }
}


#endif
