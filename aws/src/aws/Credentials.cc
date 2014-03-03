#include <aws/Credentials.hh>

#include <elle/printf.hh>
#include <elle/log.hh>

#include <aws/Exceptions.hh>

ELLE_LOG_COMPONENT("aws.Credentials");

namespace aws
{
  Credentials::Credentials(std::string const& access_key_id,
                           std::string const& secret_access_key,
                           std::string const& session_token,
                           std::string const& expiration):
    _access_key_id(access_key_id),
    _secret_access_key(secret_access_key),
    _session_token(session_token),
    _expiration_str(expiration)
  {
    // For debugging.
    if (this->_expiration_str == "never")
      return;

    // Take the AWS server response and convert it to a ptime.
    // eg: 2014-02-26T23:34:19Z -> 20140226T233419
    std::string time_str;
    for (auto c: this->_expiration_str)
    {
      if (c != '-' && c != ':' && c != 'Z')
      {
        time_str += c;
      }
    }
    this->_expiry = boost::posix_time::from_iso_string(time_str);
    if (!this->valid())
    {
      throw CredentialsError("credentials have expired");
    }
  }

  std::string
  Credentials::credential_string(RequestTime const& request_time,
                                 Region const& aws_region,
                                 Service const& aws_service)
  {
    std::string date_str = boost::posix_time::to_iso_string(request_time);
    date_str = date_str.substr(0, 8);
    std::string res = elle::sprintf(
      "%s/%s/%s/%s/aws4_request",
      this->_access_key_id,
      date_str,
      aws_region,
      aws_service
    );
    return res;
  }

  bool
  Credentials::valid()
  {
    // For debugging.
    if (this->_expiration_str == "never")
      return true;

    using namespace boost::posix_time;
    ptime now = second_clock::universal_time();
    bool res;
    if (this->_expiry < now)
    {
      ELLE_DEBUG("%s: credentials are have expired", *this);
      res = false;
    }
    else
    {
      ELLE_DEBUG("%s: credentials are valid", *this);
      res = true;
    }
    return res;
  }

  void
  Credentials::print(std::ostream& stream) const
  {
    stream << "AWS credentials: access_id=" << this->_access_key_id
           << "expiry=" << this->_expiration_str;
  }
}
