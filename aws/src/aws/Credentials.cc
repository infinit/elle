#include <aws/Credentials.hh>

#include <elle/log.hh>
#include <elle/printf.hh>
#include <elle/serialization/Serializer.hh>
#include <elle/serialization/SerializerIn.hh>

#include <aws/Exceptions.hh>

ELLE_LOG_COMPONENT("aws.Credentials");

namespace aws
{
  Credentials::Credentials(std::string const& access_key_id,
                           std::string const& secret_access_key,
                           std::string const& session_token,
                           std::string const& region,
                           std::string const& bucket,
                           std::string const& folder,
                           boost::posix_time::ptime expiration,
                           boost::posix_time::ptime server_time)
    : _access_key_id(access_key_id)
    , _secret_access_key(secret_access_key)
    , _session_token(session_token)
    , _region(region)
    , _bucket(bucket)
    , _folder(folder)
    , _server_time(std::move(server_time))
    , _expiry(std::move(expiration))
    , _skew()
    , _federated_user(true)
  {
    this->_initialize();
  }

  Credentials::Credentials(std::string const& access_key_id,
                           std::string const& secret_access_key,
                           std::string const& region,
                           std::string const& bucket,
                           std::string const& folder)
    : _access_key_id(access_key_id)
    , _secret_access_key(secret_access_key)
    , _session_token()
    , _region(region)
    , _bucket(bucket)
    , _folder(folder)
    , _server_time(boost::posix_time::second_clock::universal_time())
    , _expiry(boost::posix_time::pos_infin)
    , _skew()
    , _federated_user(false)
  {
    this->_initialize();
  }

  void
  Credentials::_initialize()
  {
    // Assume creation time is now, and compute skew with local clock
    this->_skew =
      boost::posix_time::second_clock::universal_time() - this->_server_time;
    ELLE_TRACE("Computed clock skew of %s", this->_skew);
    if (!this->valid())
    {
      // Maybe the local clock is wrong. AWS will tell us if creds are
      // realy expired.
      ELLE_WARN("Credentials appear to be expired: %s", *this);
    }
  }

  std::string
  Credentials::credential_string(RequestTime const& request_time,
                                 Service const& aws_service)
  {
    std::string date_str = boost::posix_time::to_iso_string(request_time);
    date_str = date_str.substr(0, 8);
    std::string res = elle::sprintf(
      "%s/%s/%s/%s/aws4_request",
      this->_access_key_id,
      date_str,
      this->_region,
      aws_service
    );
    return res;
  }

  bool
  Credentials::valid()
  {
    using namespace boost::posix_time;
    ptime now = second_clock::universal_time();
    bool res;
    if (this->_expiry < now)
    {
      ELLE_DEBUG("%s: credentials have expired", *this);
      res = false;
    }
    else
    {
      ELLE_DEBUG("%s: credentials are valid", *this);
      res = true;
    }
    return res;
  }

  Credentials::Credentials(elle::serialization::SerializerIn& s)
  {
    this->serialize(s);
  }

  void
  Credentials::serialize(elle::serialization::Serializer& s)
  {
    s.serialize("access_key_id", this->_access_key_id);
    s.serialize("secret_access_key", this->_secret_access_key);
    s.serialize("session_token", this->_session_token);
    if (s.in())
      this->_federated_user = this->session_token() ? true : false;
    s.serialize("region", this->_region);
    s.serialize("bucket", this->_bucket);
    s.serialize("folder", this->_folder);
    if (this->federated_user())
    {
      s.serialize("expiration", this->_expiry);
      s.serialize("current_time", this->_server_time);
    }
    else if (s.in())
    {
      this->_expiry = boost::posix_time::pos_infin;
      this->_server_time = boost::posix_time::second_clock::universal_time();
    }
    if (s.in())
      this->_initialize();
  }


  void
  Credentials::print(std::ostream& stream) const
  {
    stream << "aws::Credentials(access_id = \"" << this->_access_key_id << "\"";
    if (this->federated_user())
      stream << ", expiry = " << this->_expiry;
    stream << ")";
  }
}
