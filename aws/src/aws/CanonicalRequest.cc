#include <aws/CanonicalRequest.hh>

#include <algorithm>

#include <elle/format/hexadecimal.hh>

#include <reactor/http/EscapedString.hh>

#include <cryptography/hash.hh>
#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
# include <cryptography/_legacy/Digest.hh>
#endif

ELLE_LOG_COMPONENT("aws.CanonicalRequest");

namespace aws
{
  CanonicalRequest::CanonicalRequest(
    reactor::http::Method http_method,
    std::string const& canonical_uri,
    RequestQuery const& query,
    RequestHeaders const& headers,
    std::vector<std::string> const& signed_headers,
    std::string const& payload_sha256)
  {
    this->_canonical_request = std::string(
      elle::sprintf("%s\n%s\n%s\n%s\n\n%s\n%s",
        http_method,
        canonical_uri,
        this->_canonical_query_string(query),
        this->_canonical_headers_string(headers),
        this->_signed_headers_string(signed_headers),
        payload_sha256));
    ELLE_DUMP("%s", this->_canonical_request);
  }

  std::string
  CanonicalRequest::sha256_hash() const
  {
#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
    infinit::cryptography::Digest digest =
      infinit::cryptography::hash(
        infinit::cryptography::Plain(
          elle::ConstWeakBuffer(this->_canonical_request)),
        infinit::cryptography::Oneway::sha256);
    return elle::format::hexadecimal::encode(digest.buffer());
#else
    elle::Buffer digest =
      infinit::cryptography::hash(
        elle::ConstWeakBuffer(this->_canonical_request),
        infinit::cryptography::Oneway::sha256);
    return elle::format::hexadecimal::encode(digest);
#endif
  }

  std::string
  CanonicalRequest::_canonical_query_string(
    std::map<std::string, std::string> const& query)
  {
    if (query.empty())
      return "";

    std::string res;
    using reactor::http::EscapedString;
    for (auto const& parameter: query)
    {
      res.append(elle::sprintf("%s=%s&", EscapedString(parameter.first),
                 EscapedString(parameter.second)));
    }
    return res.substr(0, res.size() - 1);
  }

  std::string
  CanonicalRequest::_canonical_headers_string(RequestHeaders const& headers)
  {
    if (headers.empty())
      return "";

    std::string res;
    for (auto const& header: headers)
    {
      std::string key = header.first;
      std::transform(key.begin(), key.end(), key.begin(), ::tolower);
      std::string value = header.second;
      // XXX May need to trim whitespace from header values
      res.append(elle::sprintf("%s:%s\n", key, value));
    }
    res = res.substr(0, res.size() - 1);
    return res;
  }

  std::string
  CanonicalRequest::_signed_headers_string(
    std::vector<std::string> const& signed_headers)
  {
    if (signed_headers.empty())
      return "";

    std::string res;
    for (auto const& header: signed_headers)
    {
      std::string key = header;
      std::transform(key.begin(), key.end(), key.begin(), ::tolower);
      res.append(elle::sprintf("%s;", key));
    }
    res = res.substr(0, res.size() - 1);
    return res;
  }

  void
  CanonicalRequest::print(std::ostream& stream) const
  {
    stream << this->_canonical_request;
  }
}
