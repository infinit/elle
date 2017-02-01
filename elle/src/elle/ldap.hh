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
    class Attr
    {
    public:
      Attr();
      Attr(std::string const& k, std::string const& v);
      Attr(std::string const& v);
      std::string value;
    };
    Attr
    operator / (Attr const& a, Attr const& b);

    using AttrValues = std::vector<std::string>;
    using Result = std::unordered_map<std::string, AttrValues>;
    using Results = std::vector<Result>;

    class LDAPClient
    {
    public:
      LDAPClient(std::string const& url,
                 std::string const& domain,
                 std::string const& user,
                 std::string const& password);
      LDAPClient(LDAPClient const& other) = delete;
      LDAPClient(LDAPClient&& other);
      ~LDAPClient();
      Results
      search(Attr base, Attr query, std::vector<std::string> const& attrs = {});
    private:
      ELLE_ATTRIBUTE(LDAP*, ld);
      ELLE_ATTRIBUTE(Attr, domain);
    };

    class CN
      : public Attr
    {
    public:
      using Attr::Attr;
      CN(std::string const& v);
    };

    class OU
      : public Attr
    {
    public:
      using Attr::Attr;
      OU(std::string const& v);
    };

    class DC
      : public Attr
    {
    public:
      using Attr::Attr;
      DC(std::string const& v);
    };
  }
}
