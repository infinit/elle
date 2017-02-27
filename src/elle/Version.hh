#ifndef ELLE_VERSION_HH
# define ELLE_VERSION_HH

# include <cstdint>
# include <functional>

# include <elle/attribute.hh>
# include <elle/operator.hh>
# include <elle/serialization/fwd.hh>
# include <elle/Printable.hh>

namespace elle
{
  /// This class represents the version of the Infinit software.
  class ELLE_API Version
    : public elle::Printable
  {
    /*-------------.
    | Construction |
    `-------------*/
  public:
    Version(); // XXX[to remove later]
    inline
    constexpr
    Version(uint8_t major,
            uint8_t minor,
            uint8_t subminor);

    /*-----------.
    | Interfaces |
    `-----------*/
  public:
    // printable
    void
    print(std::ostream& stream) const override;

    /*----------.
    | Operators |
    `----------*/
    bool
    operator ==(Version const&) const;
    bool
    operator <(Version const&) const;
    bool
    operator >(Version const&) const;
    ELLE_OPERATOR_NEQ(Version);
    ELLE_OPERATOR_LTE(Version);
    ELLE_OPERATOR_GTE(Version);

  /*-----------.
  | Attributes |
  `-----------*/
  public:
    ELLE_ATTRIBUTE_R(uint8_t, major);
    ELLE_ATTRIBUTE_R(uint8_t, minor);
    ELLE_ATTRIBUTE_R(uint8_t, subminor);

  /*--------------.
  | Serialization |
  `--------------*/
  public:
    void
    serialize(elle::serialization::Serializer& s);

    static
    Version
    from_string(std::string const& repr);

    static
    std::string
    to_string(elle::Version const& version);
  };
}

/*-----.
| Hash |
`-----*/

namespace std
{
  template <>
  struct ELLE_API hash<elle::Version>
  {
  public:
    std::size_t
    operator()(elle::Version const& version) const;
  };
}

# include <elle/Version.hxx>

#endif
