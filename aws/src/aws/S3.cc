#include <aws/S3.hh>

#include <vector>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <elle/format/hexadecimal.hh>
#include <elle/log.hh>

#include <reactor/http/exceptions.hh>
#include <reactor/http/Request.hh>
#include <reactor/scheduler.hh>

#include <cryptography/oneway.hh>

#include <aws/SigningKey.hh>
#include <aws/Keys.hh>
#include <aws/Exceptions.hh>

ELLE_LOG_COMPONENT("aws.S3");

namespace aws
{
  /*-------------.
  | Construction |
  `-------------*/
  S3::S3(std::string const& bucket_name,
         std::string const& remote_folder,
         aws::Credentials const& credentials):
    _bucket_name(bucket_name),
    _remote_folder(remote_folder),
    _credentials(credentials)
  {}


  /*-----------.
  | Operations |
  `-----------*/

  void
  S3::put_object(elle::ConstWeakBuffer const& object,
                 std::string const& object_name)
  {
    ELLE_TRACE_SCOPE("%s: PUT block: %s", *this, object_name);

    RequestTime request_time =
      boost::posix_time::second_clock::universal_time();

    // Make headers.
    RequestHeaders headers(this->_make_put_headers(object, request_time));

    // Make canonical request.
    CanonicalRequest canonical_request(
      this->_make_put_canonical_request(object, object_name, headers));

    // Make string to sign.
    StringToSign string_to_sign(this->_make_string_to_sign(
                                request_time, canonical_request.sha256_hash()));

    // Make Authorization header.
    // http://docs.aws.amazon.com/AmazonS3/latest/API/sig-v4-header-based-auth.html
    aws::SigningKey key(this->_credentials.secret_access_key(),
                        request_time,
                        Region::us_east_1,
                        Service::s3);
    std::map<std::string, std::string> auth;

    // Make credential string.
    auth["AWS4-HMAC-SHA256 Credential"] =
      this->_credentials.credential_string(request_time, aws::Region::us_east_1,
                                          aws::Service::s3);

    // Make signed headers string.
    std::string signed_headers_str;
    for (auto header: headers)
      signed_headers_str += elle::sprintf("%s;", header.first);
    signed_headers_str =
      signed_headers_str.substr(0, signed_headers_str.size() - 1);
    auth["SignedHeaders"] = signed_headers_str;
    // Make signature string.
    auth["Signature"] =
      key.sign_message(string_to_sign.string());
    // Make authorization header string.
    std::string auth_str;
    for (auto item: auth)
      auth_str += elle::sprintf("%s=%s, ", item.first, item.second);
    auth_str = auth_str.substr(0, auth_str.size() - 2);
    headers["Authorization"] = auth_str;

    // Add headers to request.
    reactor::http::Request::Configuration cfg(300_sec,
                                              reactor::http::Version::v11);
    for (auto header: headers)
      cfg.header_add(header.first, header.second);

    auto url = elle::sprintf(
      "http://%s/%s/%s",
      headers["Host"],
      this->_remote_folder,
      object_name
    );
    ELLE_DUMP("url: %s", url);

    try
    {
      reactor::http::Request request(url, reactor::http::Method::PUT, cfg);
      ELLE_DUMP("%s: add body to request: %s", *this, object);
      request.write(
        reinterpret_cast<char const*>(object.contents()), object.size());
      reactor::wait(request);
      ELLE_DUMP("%s: AWS response: %s", *this, request.response().string());
      if (request.status() != reactor::http::StatusCode::OK)
      {
        throw aws::RequestError(elle::sprintf("unable to PUT on S3: %s",
                                request.response().string()));
      }
    }
    catch (reactor::http::RequestError const& e)
    {
      throw aws::RequestError(elle::sprintf("unable to PUT on S3: %s",
                              e.error()));
    }
  }


  /*--------.
  | Helpers |
  `--------*/
  std::string
  S3::_sha256_hexdigest(elle::ConstWeakBuffer const& buffer)
  {
    std::string res = elle::format::hexadecimal::encode(
      infinit::cryptography::oneway::hash(
        infinit::cryptography::Plain(buffer),
        infinit::cryptography::oneway::Algorithm::sha256).buffer()
    );
    return res;
  }

  RequestHeaders
  S3::_make_put_headers(elle::ConstWeakBuffer const& object,
                        RequestTime const& request_time)
  {
    ELLE_DUMP("%s: generate headers", *this);
    std::map<std::string, std::string> headers;
    headers["Content-Type"] = std::string("binary/octet-stream");
    headers["Host"] = elle::sprintf("%s.s3.amazonaws.com", this->_bucket_name);
    std::string date = boost::posix_time::to_iso_string(request_time);
    date = date.substr(0, 15);
    date += "Z";
    headers["x-amz-date"] = date;
    headers["x-amz-content-sha256"] = this->_sha256_hexdigest(object);
    // XXX We can look at using REDUCED_REDUNDANCY to save money.
    headers["x-amz-storage-class"] = std::string("STANDARD");
    headers["x-amz-security-token"] = this->_credentials.security_token();
    return headers;
  }

  aws::CanonicalRequest
  S3::_make_put_canonical_request(elle::ConstWeakBuffer const& object,
                                  std::string const& object_name,
                                  RequestHeaders const& headers)
  {
    std::vector<std::string> signed_headers;
    for (auto header: headers)
      signed_headers.push_back(header.first);

    std::map<std::string, std::string> empty_query;

    aws::CanonicalRequest res(
      reactor::http::Method::PUT,
      elle::sprintf("/%s/%s", this->_remote_folder, object_name),
      empty_query,
      headers,
      signed_headers,
      _sha256_hexdigest(object)
    );
    ELLE_DUMP("%s: generated canonical request: %s",
              *this, res.canonical_request());
    return res;
  }

  aws::StringToSign
  S3::_make_string_to_sign(RequestTime const& request_time,
                           std::string const& canonical_request_sha256)
  {
    aws::CredentialScope credential_scope(request_time,
                                          Service::s3, Region::us_east_1);

    aws::StringToSign res(request_time, credential_scope,
                          canonical_request_sha256,
                          SigningMethod::aws4_hmac_sha256);

    ELLE_DUMP("%s: generated string to sign: %s", *this, res.string());
    return res;
  }

  /*----------.
  | Printable |
  `----------*/
  void
  S3::print(std::ostream& stream) const
  {
    stream << "S3: bucket=" << this->_bucket_name
           <<    " remote_folder=" << this->_remote_folder;
  }

}
