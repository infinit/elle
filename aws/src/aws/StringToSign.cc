#include <aws/StringToSign.hh>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("aws.StringToSign");

namespace aws
{
  CredentialScope::CredentialScope(boost::posix_time::ptime const& request_time,
                                   std::string const& aws_service,
                                   std::string const& aws_region)
  {
    std::string date = boost::posix_time::to_iso_string(request_time);
    date = date.substr(0, 8);
    this->_credential_scope_str = std::string(elle::sprintf(
      "%s/%s/%s/%s",
      date,
      aws_region,
      aws_service,
      "aws4_request"
    ));
  }

  void
  CredentialScope::print(std::ostream& stream) const
  {
    stream << this->_credential_scope_str;
  }

  StringToSign::StringToSign(boost::posix_time::ptime const& request_time,
                             CredentialScope const& credential_scope,
                             std::string const& hashed_canonical_request,
                             std::string const& algorithm)
  {
    std::string iso_time = boost::posix_time::to_iso_string(request_time);
    iso_time = elle::sprintf("%sZ", iso_time.substr(0, 15));
    this->_string_to_sign = std::string(elle::sprintf(
      "%s\n%s\n%s\n%s",
      algorithm,
      iso_time,
      credential_scope,
      hashed_canonical_request
    ));
  }

  void
  StringToSign::print(std::ostream& stream) const
  {
    stream << this->_string_to_sign;
  }
}
