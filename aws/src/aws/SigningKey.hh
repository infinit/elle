#ifndef AWS_SIGNING_KEY_HH
# define AWS_SIGNING_KEY_HH

# include <string>

# include <boost/date_time/posix_time/posix_time.hpp>

# include <elle/attribute.hh>
# include <elle/Printable.hh>

# include <cryptography/Digest.hh>

# include <aws/Keys.hh>

namespace aws
{
  class SigningKey:
    public elle::Printable
  {
    /*-------------.
    | Construction |
    `-------------*/
  public:
    SigningKey(std::string const& aws_secret,
               RequestTime const& request_time,
               Region const& aws_region,
               Service const& aws_service);

    std::string
    sign_message(std::string const& message);

    ELLE_ATTRIBUTE_R(infinit::cryptography::Digest, key);

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
