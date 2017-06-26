#include <elle/printf.hh>
#include <elle/service/aws/StringToSign.hh>

namespace elle
{
  namespace service
  {
    namespace aws
    {
      CredentialScope::CredentialScope(RequestTime const& request_time,
                                       Service const& aws_service,
                                       std::string const& aws_region)
      {
        auto const date = date::format("%Y%m%d", request_time);
        this->_credential_scope_str =
          elle::sprintf("%s/%s/%s/%s",
                        date, aws_region, aws_service, RequestType::aws4);
      }

      void
      CredentialScope::print(std::ostream& stream) const
      {
        stream << this->_credential_scope_str;
      }

      StringToSign::StringToSign(RequestTime const& request_time,
                                 CredentialScope const& credential_scope,
                                 std::string const& hashed_canonical_request,
                                 SigningMethod const& algorithm)
      {
        auto iso_time = date::format("%Y%m%dT%H%M%S", request_time);
        iso_time = elle::sprintf("%sZ", iso_time.substr(0, 15));
        this->_string = elle::sprintf("%s\n%s\n%s\n%s",
                                      algorithm,
                                      iso_time,
                                      credential_scope,
                                      hashed_canonical_request);
      }

      void
      StringToSign::print(std::ostream& stream) const
      {
        stream << this->_string;
      }
    }
  }
}
