#ifndef AWS_STRING_TO_SIGN_HH
# define AWS_STRING_TO_SIGN_HH

# include <string>

# include <boost/date_time/posix_time/posix_time.hpp>

# include <elle/attribute.hh>
# include <elle/Printable.hh>

namespace aws
{
  class CredentialScope:
    public elle::Printable
  {
  public:
    CredentialScope(boost::posix_time::ptime const& request_time,
                    std::string const& aws_service,
                    std::string const& aws_region = "us-east-1");

    ELLE_ATTRIBUTE_R(std::string, credential_scope_str);

  public:
    virtual
    void
    print(std::ostream& stream) const;
  };

  class StringToSign:
    public elle::Printable
  {
    /*-------------.
    | Construction |
    `-------------*/
  public:
    /// http://docs.aws.amazon.com/general/latest/gr/sigv4-create-string-to-sign.html
    StringToSign(boost::posix_time::ptime const& request_time,
                 CredentialScope const& credential_scope,
                 std::string const& hashed_canonical_request,
                 std::string const& algorithm = "AWS4-HMAC-SHA256");

    ~StringToSign() = default;

    ELLE_ATTRIBUTE_R(std::string, string_to_sign);

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
