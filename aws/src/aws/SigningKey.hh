#ifndef AWS_SIGNING_KEY_HH
# define AWS_SIGNING_KEY_HH

# include <string>

# include <boost/date_time/posix_time/posix_time.hpp>

# include <elle/attribute.hh>
# include <elle/Printable.hh>

# include <cryptography/Digest.hh>

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
               boost::posix_time::ptime const& date,
               std::string const& aws_region,
               std::string const& aws_service);

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
