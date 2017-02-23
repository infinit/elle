#include <elle/cryptography/hmac.hh>
#include <elle/format/hexadecimal.hh>
#include <elle/printf.hh>
#include <elle/service/aws/SigningKey.hh>

namespace elle
{
  namespace service
  {
    namespace aws
    {
      static
      elle::Buffer
      _aws_hmac(std::string const& message,
                elle::Buffer const& key)
      {
        return (elle::cryptography::hmac::sign(
                  message,
                  key.string(),
                  elle::cryptography::Oneway::sha256));
      }

      static
      elle::Buffer
      _create_digest(std::string const& aws_secret,
                     RequestTime const& request_time,
                     std::string const& aws_region,
                     Service const& aws_service)
      {
        std::string date_str = boost::posix_time::to_iso_string(request_time);
        date_str = date_str.substr(0, 8);
        std::string secret_str(elle::sprintf("AWS4%s", aws_secret));
        elle::Buffer k_secret(secret_str.data(), secret_str.size());
        elle::Buffer k_date = _aws_hmac(date_str, k_secret);
        elle::Buffer k_region = _aws_hmac(elle::sprintf("%s", aws_region),
                                          k_date);
        elle::Buffer k_service =
          _aws_hmac(elle::sprintf("%s", aws_service), k_region);
        elle::Buffer k_signing =
          _aws_hmac(elle::sprintf("%s", RequestType::aws4), k_service);

        return k_signing;
      }

      SigningKey::SigningKey(std::string const& aws_secret,
                             RequestTime const& request_time,
                             std::string const& aws_region,
                             Service const& aws_service):
        _key(_create_digest(aws_secret, request_time, aws_region, aws_service))
      {}

      std::string
      SigningKey::sign_message(std::string const& message)
      {
        elle::Buffer digest = _aws_hmac(message, this->_key);
        return elle::format::hexadecimal::encode(digest);
      }

      void
      SigningKey::print(std::ostream& stream) const
      {
        stream << "AWS signing key hex digest: "
               << elle::format::hexadecimal::encode(this->_key);
      }
    }
  }
}
