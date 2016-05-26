#ifndef ELLE_LDAP_HH
# define ELLE_LDAP_HH

# include <string>
# include <vector>
# include <unordered_map>

# include <elle/attribute.hh>

struct ldap_ldap;
typedef struct ldap_ldap LDAP;

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

    typedef std::vector<std::string> AttrValues;
    typedef std::unordered_map<std::string, AttrValues> Result;
    typedef std::vector<Result> Results;

    class LDAPClient
    {
    public:
      LDAPClient(std::string const& url,
                 Attr const& domain,
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

#endif
