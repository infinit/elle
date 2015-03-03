#include <elle/UUID.hh>

#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/string_generator.hpp>

namespace elle
{
  UUID::UUID()
    : boost::uuids::uuid(boost::uuids::nil_generator()())
  {}

  UUID::UUID(boost::uuids::uuid uuid)
    : boost::uuids::uuid(std::move(uuid))
  {}

  UUID::UUID(std::string const& repr)
    : boost::uuids::uuid(boost::uuids::string_generator()(repr))
  {}

  UUID
  UUID::random()
  {
    static boost::uuids::random_generator generator;
    return generator();
  }

  namespace serialization
  {
    std::string
    Serialize<UUID>::convert(UUID& uuid)
    {
      return boost::lexical_cast<std::string>(uuid);
    }

    UUID
    Serialize<UUID>::convert(std::string& repr)
    {
      if (repr.empty())
        return UUID();
      return boost::lexical_cast<boost::uuids::uuid>(repr);
    }
  }
}
