#include <elle/Duration.hh>
#include <elle/log.hh>
#include <elle/print.hh>
#include <elle/serialization/Serializer.hh>
#include <elle/serialization/SerializerIn.hh>
#include <elle/service/aws/Credentials.hh>
#include <elle/service/aws/Exceptions.hh>

ELLE_LOG_COMPONENT("elle.services.aws.Credentials");

namespace elle
{
  namespace service
  {
    namespace aws
    {
      Credentials::Credentials(std::string access_key_id,
                               std::string secret_access_key,
                               std::string session_token,
                               std::string region,
                               std::string bucket,
                               std::string folder,
                               Time expiration,
                               Time server_time,
                               boost::optional<std::string> endpoint)
        : _access_key_id{std::move(access_key_id)}
        , _secret_access_key{std::move(secret_access_key)}
        , _session_token{std::move(session_token)}
        , _region{std::move(region)}
        , _bucket{std::move(bucket)}
        , _folder{std::move(folder)}
        , _server_time(std::move(server_time))
        , _expiry(std::move(expiration))
        , _skew()
        , _federated_user(true)
        , _endpoint(endpoint)
      {
        this->_initialize();
      }

      Credentials::Credentials(std::string access_key_id,
                               std::string secret_access_key,
                               std::string region,
                               std::string bucket,
                               std::string folder,
                               boost::optional<std::string> endpoint)
        : _access_key_id{std::move(access_key_id)}
        , _secret_access_key{std::move(secret_access_key)}
        , _region{std::move(region)}
        , _bucket{std::move(bucket)}
        , _folder{std::move(folder)}
        , _server_time(elle::Clock::now())
        , _expiry(Time::max())
        , _skew()
        , _federated_user(false)
        , _endpoint(endpoint)
      {
        this->_initialize();
      }

      void
      Credentials::_initialize()
      {
        // Assume creation time is now, and compute skew with local clock
        this->_skew = elle::Clock::now() - this->_server_time;
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
                                     Service const& aws_service) const
      {
        return elle::print("%s/%s/%s/%s/aws4_request",
          this->_access_key_id, format("%Y%m%d", request_time), this->_region,
                           aws_service);
      }

      bool
      Credentials::valid()
      {
        if (this->_expiry < Clock::now())
        {
          ELLE_DEBUG("%s: credentials have expired", this);
          return false;
        }
        else
        {
          ELLE_DEBUG("%s: credentials are valid", this);
          return true;
        }
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
          this->_expiry = Time::max();
          this->_server_time = elle::Clock::now();
        }
        s.serialize("endpoint", this->_endpoint);
        if (s.in())
          this->_initialize();
      }


      void
      Credentials::print(std::ostream& os) const
      {
        elle::print(os, "aws::Credentials(access_id = \"%s\"",
                    this->_access_key_id);
        if (this->federated_user())
          os << ", expiry = " << this->_expiry;
        if (this->endpoint())
          os << ", endpoint = " << this->endpoint().get();
        os << ')';
      }
    }
  }
}
