#include <algorithm>

#include <elle/cryptography/hash.hh>
#include <elle/format/hexadecimal.hh>
#include <elle/print.hh>
#include <elle/service/aws/CanonicalRequest.hh>

#include <elle/reactor/http/EscapedString.hh>

ELLE_LOG_COMPONENT("elle.services.aws.CanonicalRequest");

namespace elle
{
  namespace service
  {
    namespace aws
    {
      CanonicalRequest::CanonicalRequest(
        elle::reactor::http::Method http_method,
        std::string const& canonical_uri,
        RequestQuery const& query,
        RequestHeaders const& headers,
        std::vector<std::string> const& signed_headers,
        std::string const& payload_sha256)
      {
        this->_canonical_request =
          elle::print("%s\n%s\n%s\n%s\n\n%s\n%s",
                      http_method,
                      canonical_uri,
                      this->_canonical_query_string(query),
                      this->_canonical_headers_string(headers),
                      this->_signed_headers_string(signed_headers),
                      payload_sha256);
        ELLE_DUMP("%s", this->_canonical_request);
      }

      std::string
      CanonicalRequest::sha256_hash() const
      {
        elle::Buffer digest =
          elle::cryptography::hash(
            elle::ConstWeakBuffer(this->_canonical_request),
            elle::cryptography::Oneway::sha256);
        return elle::format::hexadecimal::encode(digest);
      }

      // FIXME: query_parameters in S3.cc.
      std::string
      CanonicalRequest::_canonical_query_string(
        std::map<std::string, std::string> const& query)
      {
        auto res = std::string{};
        using elle::reactor::http::EscapedString;
        for (auto const& parameter: query)
          res += elle::print("%s%s=%s",
                             res.empty() ? "" : "&",
                             EscapedString(parameter.first),
                             EscapedString(parameter.second));
        return res;
      }

      std::string
      CanonicalRequest::_canonical_headers_string(RequestHeaders const& headers)
      {
        auto res = std::string{};
        for (auto const& h: headers)
          // XXX May need to trim whitespace from header values
          res += elle::print("%s%s:%s",
                             res.empty() ? "" : "\n",
                             boost::to_lower_copy(h.first),
                             h.second);
        return res;
      }

      std::string
      CanonicalRequest::_signed_headers_string(
        std::vector<std::string> const& signed_headers)
      {
        auto res = std::string{};
        for (auto const& h: signed_headers)
          res += elle::print("%s%s",
                             res.empty() ? "" : ";",
                             boost::to_lower_copy(h));
        return res;
      }

      void
      CanonicalRequest::print(std::ostream& stream) const
      {
        stream << this->_canonical_request;
      }
    }
  }
}
