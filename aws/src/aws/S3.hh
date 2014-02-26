#ifndef AWS_S3_S3_HH
# define AWS_S3_S3_HH

# include <map>

# include <elle/Printable.hh>
# include <elle/Buffer.hh>
# include <elle/attribute.hh>

# include <aws/CanonicalRequest.hh>
# include <aws/Credentials.hh>
# include <aws/StringToSign.hh>

namespace aws
{
  class S3:
    public elle::Printable
  {
    /*-------------.
    | Construction |
    `-------------*/
  public:
    S3(std::string const& bucket_name,
       std::string const& remote_folder,
       Credentials const& credentials);

    /*-----------.
    | Operations |
    `-----------*/
  public:
    void
    put_object(elle::ConstWeakBuffer const& object,
               std::string const& object_name,
               RequestTime const& request_time);

    /*-----------.
    | Attributes |
    `-----------*/
  private:
    ELLE_ATTRIBUTE(std::string, bucket_name);
    ELLE_ATTRIBUTE(std::string, remote_folder);
    ELLE_ATTRIBUTE(Credentials, credentials);

    /*--------.
    | Helpers |
    `--------*/
  private:
    std::string
    _sha256_hexdigest(elle::ConstWeakBuffer const& buffer);

    RequestHeaders
    _make_put_headers(elle::ConstWeakBuffer const& object,
                      boost::posix_time::ptime const& request_time);

    CanonicalRequest
    _make_put_canonical_request(elle::ConstWeakBuffer const& object,
                                std::string const& object_name,
                                RequestHeaders const& headers);

    StringToSign
    _make_string_to_sign(RequestTime const& request_time,
                         std::string const& canonical_request_sha256);

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
