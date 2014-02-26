#include <aws/Credentials.hh>

#include <elle/printf.hh>

namespace aws
{
  Credentials::Credentials(std::string const& access_id,
                           std::string const& secret_access_key,
                           std::string const& security_token):
    _access_id(access_id),
    _secret_access_key(secret_access_key),
    _security_token(security_token)
  {}

  std::string
  Credentials::credential_string(RequestTime const& request_time,
                                 Region const& aws_region,
                                 Service const& aws_service)
  {
    std::string date_str = boost::posix_time::to_iso_string(request_time);
    date_str = date_str.substr(0, 8);
    std::string res = elle::sprintf(
      "%s/%s/%s/%s/aws4_request",
      this->_access_id,
      date_str,
      aws_region,
      aws_service
    );
    return res;
  }
}
