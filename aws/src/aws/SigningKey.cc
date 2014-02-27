#include <aws/SigningKey.hh>

#include <elle/format/hexadecimal.hh>
#include <elle/printf.hh>

#include <cryptography/oneway.hh>

namespace aws
{
  static
  infinit::cryptography::Digest
  _aws_hmac(std::string const& message,
            infinit::cryptography::Digest const& key)
  {
    using namespace infinit::cryptography;
    Digest res = oneway::hmac(
      Plain(elle::ConstWeakBuffer(message)), key, oneway::Algorithm::sha256);
    return res;
  }

  static
  infinit::cryptography::Digest
  _create_digest(std::string const& aws_secret,
                 RequestTime const& request_time,
                 Region const& aws_region,
                 Service const& aws_service)
  {
    std::string date_str = boost::posix_time::to_iso_string(request_time);
    date_str = date_str.substr(0, 8);
    std::string secret_str(elle::sprintf("AWS4%s", aws_secret));
    using namespace infinit::cryptography;
    Digest k_secret(elle::Buffer(secret_str.data(), secret_str.size()));
    Digest k_date = _aws_hmac(date_str, k_secret);
    Digest k_region = _aws_hmac(elle::sprintf("%s", aws_region), k_date);
    Digest k_service = _aws_hmac(elle::sprintf("%s", aws_service), k_region);
    Digest k_signing = _aws_hmac(elle::sprintf("%s", RequestType::aws4),
                                 k_service);
    return k_signing;
  }

  SigningKey::SigningKey(std::string const& aws_secret,
                         RequestTime const& request_time,
                         Region const& aws_region,
                         Service const& aws_service):
    _key(_create_digest(aws_secret, request_time, aws_region, aws_service))
  {}

  std::string
  SigningKey::sign_message(std::string const& message)
  {
    infinit::cryptography::Digest digest = _aws_hmac(message, this->_key);
    return elle::format::hexadecimal::encode(digest.buffer());
  }

  void
  SigningKey::print(std::ostream& stream) const
  {
    stream << "AWS signing key hex digest: "
           << elle::format::hexadecimal::encode(this->_key.buffer());
  }
}