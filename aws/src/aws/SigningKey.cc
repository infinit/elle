#include <aws/SigningKey.hh>

#include <elle/format/hexadecimal.hh>

#include <cryptography/oneway.hh>

namespace aws
{
  static
  infinit::cryptography::Digest
  _create_digest(std::string const& aws_secret,
                 boost::posix_time::ptime const& date,
                 std::string const& aws_region,
                 std::string const& aws_service)
  {
    std::string date_str = boost::posix_time::to_iso_string(date);
    date_str = date_str.substr(0, 8);
    std::string secret_str(elle::sprintf("AWS4%s", aws_secret));
    using namespace infinit::cryptography;
    Digest k_secret(elle::Buffer(secret_str.data(), secret_str.size()));
    Digest k_date = oneway::hmac(Plain(
      elle::ConstWeakBuffer(date_str)), k_secret, oneway::Algorithm::sha256);
    Digest k_region = oneway::hmac(Plain(
      elle::ConstWeakBuffer(aws_region)), k_date, oneway::Algorithm::sha256);
    Digest k_service = oneway::hmac(Plain(
      elle::ConstWeakBuffer(aws_service)), k_region, oneway::Algorithm::sha256);
    Digest k_signing = oneway::hmac(Plain(
      elle::ConstWeakBuffer("aws4_request")),
                                    k_service, oneway::Algorithm::sha256);
    return k_signing;
  }

  SigningKey::SigningKey(std::string const& aws_secret,
                         boost::posix_time::ptime const& date,
                         std::string const& aws_region,
                         std::string const& aws_service):
    _key(_create_digest(aws_secret, date, aws_region, aws_service))
  {}

  void
  SigningKey::print(std::ostream& stream) const
  {
    stream << "AWS signing key hex digest: "
           << elle::format::hexadecimal::encode(this->_key.buffer());
  }
}