#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include <elle/attribute.hh>

struct ldap_ldap;
using LDAP = ldap_ldap;

namespace elle
{
  namespace ldap
  {
    /// An attribute of the LDAP.
    class Attr
    {
    public:
      /// Construct an empty Attribute.
      Attr();
      /// Construct @a k=@a v Attribute.
      ///
      /// @param k The key.
      /// @param v The value.
      Attr(std::string const& k, std::string const& v);
      /// Construct a plain value Attribute.
      ///
      /// @param v The value.
      Attr(std::string const& v);
      std::string value;
    };
    /// Chain Attributes, comma separated.
    Attr
    operator / (Attr const& a, Attr const& b);

    using AttrValues = std::vector<std::string>;
    using Result = std::unordered_map<std::string, AttrValues>;
    using Results = std::vector<Result>;

    /// An client for a LDAP system.
    class LDAPClient
    {
    public:
      /// Construct an LDAPClient.
      ///
      /// @param url The ldap url. ([ldap:[389]//]...)
      /// @parap domain The domain
      /// @param user The user name.
      /// @parma password The user password.
      LDAPClient(std::string const& url,
                 std::string const& domain,
                 std::string const& user,
                 std::string const& password);
      LDAPClient(LDAPClient const& other) = delete;
      LDAPClient(LDAPClient&& other);
      ~LDAPClient();
      /// Search in the LDAP.
      ///
      /// @param base The base (will be suffixed by the domain)
      /// @param query The filter.
      /// @param attrs The list of attributes to return.
      /// @returns Results.
      Results
      search(Attr base, Attr query, std::vector<std::string> const& attrs = {});
    private:
      ELLE_ATTRIBUTE(LDAP*, ld);
      ELLE_ATTRIBUTE(Attr, domain);
    };

    /// A common name Attribute.
    class CN
      : public Attr
    {
    public:
      using Attr::Attr;
      /// Create @a Attribute cn=v.
      CN(std::string const& v);
    };

    /// An organization unit Attribute.
    class OU
      : public Attr
    {
    public:
      using Attr::Attr;
      /// Create @a Attribute ou=v.
      OU(std::string const& v);
    };

    /// A domain component Attribute.
    class DC
      : public Attr
    {
    public:
      using Attr::Attr;
      /// Create @a Attribute dc=v.
      DC(std::string const& v);
    };
  }
}
