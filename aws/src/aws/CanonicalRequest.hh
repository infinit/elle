#ifndef AWS_CANONICAL_REQUEST_HH
# define AWS_CANONICAL_REQUEST_HH

# include <string>
# include <map>
# include <vector>

# include <elle/Printable.hh>

# include <reactor/http/Request.hh>

# include <aws/Keys.hh>

namespace aws
{
  class CanonicalRequest:
    public elle::Printable
  {
    /*-------------.
    | Construction |
    `-------------*/
  public:
    /// http://docs.aws.amazon.com/general/latest/gr/sigv4-create-canonical-request.html
    CanonicalRequest(
      reactor::http::Method http_method,
      std::string const& canonical_uri,
      std::map<std::string, std::string> const& query,
      RequestHeaders const& headers,
      std::vector<std::string> const& signed_headers,
      std::string const& payload_sha256);

    ELLE_ATTRIBUTE_R(std::string, canonical_request);

    std::string
    sha256_hash();

  private:
    std::string
    _canonical_query_string(std::map<std::string, std::string> const& query);

    std::string
    _canonical_headers_string(RequestHeaders const& headers);

    std::string
    _signed_headers_string(std::vector<std::string> const& header_keys);

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
