#ifndef AWS_CREDENTIALS_HH
# define AWS_CREDENTIALS_HH

# include <string>

# include <boost/date_time/posix_time/posix_time.hpp>

# include <elle/attribute.hh>
# include <elle/Printable.hh>

# include <aws/Keys.hh>

namespace aws
{
  class Credentials:
    public elle::Printable
  {
      /*-------------.
      | Construction |
      `-------------*/
    public:
      Credentials(std::string const& access_id,
                  std::string const& secret_access_key,
                  std::string const& security_token,
                  std::string const& expiry);

      std::string
      credential_string(RequestTime const& request_time,
                        Region const& aws_region,
                        Service const& aws_service);

      bool
      valid();

      ELLE_ATTRIBUTE_R(std::string, access_id);
      ELLE_ATTRIBUTE_R(std::string, secret_access_key);
      ELLE_ATTRIBUTE_R(std::string, security_token);
      ELLE_ATTRIBUTE_R(std::string, expiry_str);
      ELLE_ATTRIBUTE_R(boost::posix_time::ptime, expiry);

      /*----------.
      | Printable |
      `----------*/
    public:
      virtual
      void
      print(std::ostream& stream) const;
  };
}

#endif
