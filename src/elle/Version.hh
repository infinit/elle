#pragma once

#include <cstdint>
#include <functional>

#include <elle/attribute.hh>
#include <elle/operator.hh>
#include <elle/serialization/fwd.hh>
#include <elle/Printable.hh>

namespace elle
{
  /// This class represents a version as a tuple of three unsigned 8-bit
  /// integers.
  ///
  /// \code{.cc}
  ///
  /// auto v_0_3_1 = elle::Version(0, 3, 1);
  /// auto v_1_1_0 = elle::Version::from_string("1.1");
  ///
  /// assert(v_1_1_0.major() == 1);
  /// assert(v_1_1_0.minor() == 1);
  /// assert(v_1_1_0.subminor() == 0);
  /// assert(v_1_1_0 > v_0_3_1);
  ///
  /// \endcode
  class ELLE_API Version
    : public elle::Printable
  {
    /*-------------.
    | Construction |
    `-------------*/
  public:
    Version(); // XXX[to remove later]
    /// Construct a version from the trio (major, minor, subminor).
    ///
    /// @param major The major.
    /// @param minor The minor.
    /// @param subminor The subminor.
    inline
    constexpr
    Version(uint8_t major,
            uint8_t minor,
            uint8_t subminor);

    /*-----------.
    | Interfaces |
    `-----------*/
  public:
    // Print the Version.
    void
    print(std::ostream& stream) const override;

    /*----------.
    | Operators |
    `----------*/
    /// Check if two Versions are equal.
    bool
    operator ==(Version const&) const;
    /// Check order of version.
    bool
    operator <(Version const&) const;
    /// Check order of version.
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

    /// Construct a Version from its standard representation.
    ///
    /// Expected format: "<major>[.<minor>[.<subminor>]]".
    ///
    /// If omitted, subminor and minor are set to 0.
    ///
    /// @param repr A string representing a Version.
    static
    Version
    from_string(std::string const& repr);

    /// Turn a Version to its string representation, using the standard format
    /// "<major>.<minor>.<subminor>".
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

#include <elle/Version.hxx>
