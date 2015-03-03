#ifndef ELLE_UUID_HH
# define ELLE_UUID_HH

# include <boost/uuid/uuid.hpp>
# include <boost/uuid/uuid_io.hpp>

# include <elle/serialization/Serializer.hh>

namespace elle
{
  class UUID:
    public boost::uuids::uuid
  {
  public:
    UUID();
    UUID(boost::uuids::uuid uuid);
    static
    UUID
    random();
  };

  namespace serialization
  {
    template <>
    struct Serialize<UUID>
    {
      typedef std::string Type;
      static
      std::string
      convert(UUID& uuid);
      static
      UUID
      convert(std::string& repr);
    };
  }
}

#endif
