#include <elle/ldap.hh>
#define ldap ldap_ldap
#include <ldap.h>
#undef ldap

#include <boost/algorithm/string.hpp>

#include <elle/log.hh>
#include <elle/Error.hh>

ELLE_LOG_COMPONENT("elle.ldap");

namespace elle
{
  namespace ldap
  {
    /*-----.
    | Attr |
    `-----*/

    Attr::Attr()
    {}

    Attr::Attr(std::string const& k, std::string const& v)
      : value(k + "=" + v)
    {}

    Attr::Attr(std::string const& v)
      : value(v)
    {}

    Attr
    operator / (Attr const& a, Attr const& b)
    {
      auto res = Attr(a.value + "," + b.value);
      if (res.value[0] == ',')
        res.value = res.value.substr(1);
      if (res.value[res.value.size()-1] == ',')
        res.value = res.value.substr(0, res.value.size()-1);
      return res;
    }

    /*---.
    | CN |
    `---*/

    CN::CN(std::string const& v)
      : Attr("cn", v)
    {}

    /*---.
    | OU |
    `---*/

    OU::OU(std::string const& v)
      : Attr("ou", v)
    {}

    /*---.
    | DC |
    `---*/

    DC::DC(std::string const& v)
    {
      std::vector<std::string> comps;
      boost::algorithm::split(comps, v, boost::is_any_of("."));
      for (auto& s: comps)
        s = "dc=" + s;
      value = boost::algorithm::join(comps, ",");
    }

    /*-----------.
    | LDAPClient |
    `-----------*/

    LDAPClient::LDAPClient(std::string const& url_,
                           std::string const& domain,
                           std::string const& user,
                           std::string const& password)
      : _domain(Attr(domain))
    {
      std::string url(url_);
      auto p = url.find("://");
      if (p == url.npos)
      {
        url = "ldap://" + url;
        p = 4;
      }
      auto p2 = url.find_last_of(':');
      if (p2 == p || p2 == url.npos)
        url = url + ":389";
      ELLE_TRACE("Using ldap url %s", url);
      int rc;
      rc = ldap_initialize(&this->_ld, url.c_str());
      if (rc != LDAP_SUCCESS)
        elle::err("ldap_initialize: %s", ldap_err2string(rc));
      int desired_version = 3;
      rc = ldap_set_option(this->_ld, LDAP_OPT_PROTOCOL_VERSION, &desired_version);
      if (rc != LDAP_OPT_SUCCESS)
        elle::err("ldap_set_option: %s", ldap_err2string(rc));
      Attr authuser = Attr(user);
      if (!domain.empty())
      {
        if (user.find("cn=") == user.npos)
          authuser = CN(user);
        if (user.find(domain) == user.npos)
          authuser = authuser / this->_domain;
      }
      ELLE_TRACE("binding with %s", authuser.value);
      /*
      std::string qbind = "cn=" + user + ",dc=" + domain;
      rc = ldap_simple_bind_s(ld, qbind.c_str(), password.c_str());
      */
      struct berval cred;
      cred.bv_val = (char*)password.c_str();
      cred.bv_len = password.size();
      rc = ldap_sasl_bind_s(_ld, authuser.value.c_str(),
        0, &cred, NULL, NULL, NULL);
      if (rc != LDAP_SUCCESS)
        elle::err("ldap_sasl_bind: %s", ldap_err2string(rc));
    }

    LDAPClient::LDAPClient(LDAPClient&& other)
      : _ld(other._ld)
      , _domain(other._domain)
    {
      other._ld = nullptr;
    }

    LDAPClient::~LDAPClient()
    {
      if (this->_ld)
        ldap_unbind_ext_s(this->_ld, 0, 0);
    }

    Results
    LDAPClient::search(Attr base,
                       Attr query,
                       std::vector<std::string> const& attrs)
    {
      std::vector<char*> cattrs;
      cattrs.resize(attrs.size() + 1);
      for (int i = 0; i < signed(attrs.size()); ++i)
        cattrs[i] = (char*)attrs[i].c_str();
      cattrs[attrs.size()] = nullptr;
      base = base / this->_domain;
      LDAPMessage* msg;
      if (query.value.empty())
        query.value = "(objectclass=*)";
      int rc = ldap_search_ext_s(_ld, base.value.c_str(),
                                 LDAP_SCOPE_SUBTREE,
                                 query.value.c_str(),
                                 &cattrs.front(), 0,
                                 NULL, NULL, NULL,
                                 0, &msg);
      if (rc != LDAP_SUCCESS)
        elle::err("ldap_search: %s", ldap_err2string(rc));
      Results res;
      LDAPMessage* entry;
      for (entry = ldap_first_entry(this->_ld, msg);
           entry != NULL;
           entry = ldap_next_entry(this->_ld, entry))
      {
        Result p;
        BerElement* ber;
        char* dn = ldap_get_dn(this->_ld, entry);
        p["dn"].push_back(dn);
        ldap_memfree(dn);
        const char* attr;
        for( attr = ldap_first_attribute(this->_ld, entry, &ber);
             attr != NULL;
             attr = ldap_next_attribute(this->_ld, entry, ber))
        {
          std::vector<std::string> vals;
          berval ** bvals = ldap_get_values_len(_ld, entry, attr);
          if (bvals)
            for (int i = 0; bvals[i] != NULL; ++i)
              vals.emplace_back(bvals[i]->bv_val,
                                bvals[i]->bv_val + bvals[i]->bv_len);
          p[attr] = std::move(vals);
        }
        res.emplace_back(std::move(p));
      }
      return res;
    }
  }
}
