#include <elle/UUID.hh>

#include <chrono>

#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/string_generator.hpp>

namespace elle
{
  /*-------------------.
  | UUID: Construction |
  `-------------------*/

  UUID::UUID()
    : boost::uuids::uuid(boost::uuids::nil_generator()())
  {}

  UUID::UUID(boost::uuids::uuid uuid)
    : boost::uuids::uuid(std::move(uuid))
  {}

  UUID::UUID(std::string const& repr)
  try
    : boost::uuids::uuid(boost::uuids::string_generator()(repr))
  {}
  catch (std::runtime_error const&)
  {
    throw InvalidUUID(repr);
  }

  UUID
  UUID::random()
  {
    // Beware, (probably) not thread safe.
    static bool first = true;
    static boost::mt19937 rng;
    if (first)
    {
      first = false;
      rng.seed(std::chrono::steady_clock::now().time_since_epoch().count() );
    }
    static boost::uuids::basic_random_generator<boost::mt19937> generator(&rng);
    return generator();
  }

  /*----------------.
  | UUID: Observers |
  `----------------*/

  std::string
  UUID::repr() const
  {
    return boost::lexical_cast<std::string>(*this);
  }

  /*------------.
  | InvalidUUID |
  `------------*/

  InvalidUUID::InvalidUUID(std::string repr)
    : Super(elle::sprintf("invalid UUID: \"%s\"", repr))
    , _repr(std::move(repr))
  {}

  /*--------------.
  | Serialization |
  `--------------*/

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
      return elle::UUID(repr);
    }
  }
}
