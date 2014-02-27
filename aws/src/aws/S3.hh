#ifndef AWS_S3_S3_HH
# define AWS_S3_S3_HH

# include <map>
# include <vector>

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
               std::string const& object_name);

    void
    list_remote_folder();

    elle::Buffer
    get_object(std::string const& object_name);

    /*-----------.
    | Attributes |
    `-----------*/
  private:
    ELLE_ATTRIBUTE(std::string, bucket_name);
    ELLE_ATTRIBUTE(std::string, remote_folder);
    ELLE_ATTRIBUTE(Credentials, credentials);
    ELLE_ATTRIBUTE(std::string, host_name);

    /*--------.
    | Helpers |
    `--------*/
  private:
    std::string
    _sha256_hexdigest(elle::ConstWeakBuffer const& buffer);

    std::string
    _amz_date(RequestTime const& request_time);

    std::vector<std::string>
    _signed_headers(RequestHeaders const& headers);

    StringToSign
    _make_string_to_sign(RequestTime const& request_time,
                         std::string const& canonical_request_sha256);

    RequestHeaders
    _make_put_headers(elle::ConstWeakBuffer const& object,
                      RequestTime const& request_time);

    RequestHeaders
    _make_get_headers(RequestTime const& request_time);

    RequestHeaders
    _make_list_get_headers(RequestTime const& request_time);

    CanonicalRequest
    _make_put_canonical_request(elle::ConstWeakBuffer const& object,
                                std::string const& object_name,
                                RequestHeaders const& headers);

    CanonicalRequest
    _make_list_canonical_request(RequestHeaders const& headers,
                                 RequestQuery const& query);

    CanonicalRequest
    _make_get_canonical_request(RequestHeaders const& headers,
                                    std::string const& object_name);

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
