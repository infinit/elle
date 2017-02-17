#ifndef ELLE_UUID_HH
# define ELLE_UUID_HH

# include <boost/functional/hash.hpp>
# include <boost/uuid/uuid.hpp>
# include <boost/uuid/uuid_io.hpp>

# include <elle/serialization/Serializer.hh>

namespace elle
{
  /// Universal unique identifier
  ///
  /// Universal unique identifier as per RFC 4122. This class is mostly a
  /// convenience wrapper above boost::uuids.
  class ELLE_API UUID:
    public boost::uuids::uuid
  {
  /*-------------.
  | Construction |
  `-------------*/
  public:
    /// Create nil UUID
    UUID();
    /// Create from Boost UUID.
    UUID(boost::uuids::uuid uuid);
    /// Create from string representation.
    UUID(std::string const& repr);
    /// Create random UUID.
    static
    UUID
    random();

  /*----------.
  | Observers |
  `----------*/
  public:
    /// The string representation.
    std::string
    repr() const;
  };

  /// String cannot be interpreted as a UUID error.
  ///
  ///
  class ELLE_API InvalidUUID
    : public Error
  {
  /*------.
  | Types |
  `------*/
  public:
    typedef InvalidUUID Self;
    typedef Error Super;

  /*-------------.
  | Construction |
  `-------------*/
  public:
    InvalidUUID(std::string repr);

  /*-----------.
  | Attributes |
  `-----------*/
  public:
    ELLE_ATTRIBUTE_R(std::string, repr);
  };


  /// Serialization
  ///
  /// Serialize UUID as strings. The empty string is considered a valid, nil
  /// UUID.
  namespace serialization
  {
    template <>
    struct ELLE_API Serialize<UUID>
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

namespace std
{
  template<>
  struct ELLE_API hash<elle::UUID>
  {
  public:
    size_t operator()(elle::UUID const& uuid) const
    {
      return boost::hash<boost::uuids::uuid>()(uuid);
    }
  };
}

#endif
