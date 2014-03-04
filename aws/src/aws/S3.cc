#include <aws/S3.hh>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <elle/format/hexadecimal.hh>
#include <elle/log.hh>

#include <reactor/http/exceptions.hh>
#include <reactor/http/EscapedString.hh>
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
    _credentials(credentials),
    _host_name(elle::sprintf("%s.s3.amazonaws.com", this->_bucket_name))
  {}


  /*-----------.
  | Operations |
  `-----------*/

  void
  S3::put_object(elle::ConstWeakBuffer const& object,
                 std::string const& object_name)
  {
    ELLE_TRACE_SCOPE("%s: PUT block: %s", *this, object_name);

    if (!this->_credentials.valid())
    {
      throw CredentialsExpired(elle::sprintf("%s: credentials expired: %s",
                               *this, this->_credentials));
    }

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
      "https://%s:%s/%s/%s",
      headers["Host"],
      "443",
      this->_remote_folder,
      object_name
    );
    ELLE_DUMP("url: %s", url);

    try
    {
      reactor::http::Request request(url, reactor::http::Method::PUT, cfg);
      ELLE_DUMP("%s: add body to request: %s", *this, object);
      request.write(reinterpret_cast<char const*>(object.contents()),
                    object.size());
      reactor::wait(request);
      ELLE_DUMP("%s: AWS response: %s", *this, request.response().string());
      if (request.status() != reactor::http::StatusCode::OK)
      {
        throw aws::RequestError(
          elle::sprintf("%s: unable to PUT on S3, got HTTP status: %s",
                        *this, request.status()));
      }
    }
    catch (reactor::http::RequestError const& e)
    {
      throw aws::RequestError(
        elle::sprintf("%s: unable to PUT on S3, unable to perform HTTP request: %s",
                      *this, e.error()));
    }
  }

  std::vector<std::pair<std::string, S3::FileSize>>
  S3::list_remote_folder(std::string const& marker)
  {
    ELLE_TRACE_SCOPE("%s: LIST remote folder", *this);

    if (!this->_credentials.valid())
    {
      throw CredentialsExpired(elle::sprintf("%s: credentials expired: %s",
                               *this, this->_credentials));
    }

    RequestTime request_time =
      boost::posix_time::second_clock::universal_time();

    // Make headers.
    RequestHeaders headers(this->_make_generic_headers(request_time));

    // Make canonical request.
    RequestQuery query;
    query["prefix"] = elle::sprintf("%s/", this->_remote_folder);
    query["delimiter"] = "/";
    if (marker.size() > 0)
      query["marker"] = elle::sprintf("%s/%s", this->_remote_folder, marker);
    CanonicalRequest canonical_request(
      this->_make_list_canonical_request(headers, query));

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
    reactor::http::Request::Configuration cfg(10_sec,
                                              reactor::http::Version::v11);
    for (auto header: headers)
      cfg.header_add(header.first, header.second);

    // Make query string.
    using reactor::http::EscapedString;
    std::string query_str;
    for (auto parameter: query)
    {
      query_str += elle::sprintf("%s=%s&", EscapedString(parameter.first),
                                 EscapedString(parameter.second));
    }
    query_str = query_str.substr(0, query_str.size() - 1);

    auto url = elle::sprintf(
      "https://%s:%s?%s",
      headers["Host"],
      "443",
      query_str
    );
    ELLE_DUMP("url: %s", url);

    try
    {
      reactor::http::Request request(url, reactor::http::Method::GET, cfg);
      reactor::wait(request);
      if (request.status() != reactor::http::StatusCode::OK)
      {
        throw aws::RequestError(
          elle::sprintf("%s: unable to LIST on S3, got HTTP status: %s",
                        *this, request.status()));
      }
      return this->_parse_list_xml(request);
    }
    catch (reactor::http::RequestError const& e)
    {
      throw aws::RequestError(
        elle::sprintf("%s: unable to LIST on S3, unable to perform HTTP request: %s",
                      *this, e.error()));
    }
  }

  elle::Buffer
  S3::get_object(std::string const& object_name)
  {
    ELLE_TRACE_SCOPE("%s: GET remote object", *this);

    if (!this->_credentials.valid())
    {
      throw CredentialsExpired(elle::sprintf("%s: credentials expired: %s",
                               *this, this->_credentials));
    }

    RequestTime request_time =
      boost::posix_time::second_clock::universal_time();

    // Make headers.
    RequestHeaders headers(this->_make_generic_headers(request_time));

    // Make canonical request.
    CanonicalRequest canonical_request(
      this->_make_get_canonical_request(headers, object_name));

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
      "https://%s:%s/%s/%s",
      headers["Host"],
      "443",
      this->_remote_folder,
      object_name
    );
    ELLE_DUMP("url: %s", url);

    try
    {
      reactor::http::Request request(url, reactor::http::Method::GET, cfg);
      reactor::wait(request);
      if (request.status() != reactor::http::StatusCode::OK)
      {
        throw aws::RequestError(
          elle::sprintf("%s: unable to GET on S3, got HTTP status: %s",
                        *this, request.status()));
      }
      auto response = request.response();
      std::string calcd_md5(this->_md5_digest(response));
      std::string aws_md5(request.headers().at("ETag"));
      // Remove quotes around MD5 sum from AWS.
      aws_md5 = aws_md5.substr(1, aws_md5.size() - 2);
      if (calcd_md5 != aws_md5)
      {
        throw aws::RequestError(
          elle::sprintf("%s: GET data corrupt: %s != %s", *this, calcd_md5,
                        aws_md5));
      }
      return response;
    }
    catch (reactor::http::RequestError const& e)
    {
      throw aws::RequestError(
        elle::sprintf("%s: unable to GET on S3, unable to perform HTTP request: %s",
                      *this, e.error()));
    }
  }

  void
  S3::delete_object(std::string const& object_name)
  {
    ELLE_TRACE_SCOPE("%s: DELETE remote object", *this);

    if (!this->_credentials.valid())
    {
      throw CredentialsExpired(elle::sprintf("%s: credentials expired: %s",
                               *this, this->_credentials));
    }

    RequestTime request_time =
      boost::posix_time::second_clock::universal_time();

    // Make headers.
    RequestHeaders headers(this->_make_generic_headers(request_time));

    // Make canonical request.
    CanonicalRequest canonical_request(
      this->_make_delete_canonical_request(headers, object_name));

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
      "https://%s:%s/%s/%s",
      headers["Host"],
      "443",
      this->_remote_folder,
      object_name
    );
    ELLE_DUMP("url: %s", url);

    try
    {
      reactor::http::Request request(url, reactor::http::Method::DELETE, cfg);
      reactor::wait(request);
      ELLE_DUMP("%s: response: %s", *this, request.response().string());
      if (request.status() != reactor::http::StatusCode::OK &&
          request.status() != reactor::http::StatusCode::No_Content)
      {
        throw aws::RequestError(
          elle::sprintf("%s: unable to DELETE on S3, got HTTP status: %s",
                        *this, request.status()));
      }
    }
    catch (reactor::http::RequestError const& e)
    {
      throw aws::RequestError(
        elle::sprintf("%s: unable to GET on DELETE, unable to perform HTTP request: %s",
                      *this, e.error()));
    }
  }


  /*--------.
  | Helpers |
  `--------*/
  std::string
  S3::_md5_digest(elle::ConstWeakBuffer const& buffer)
  {
    std::string res = elle::format::hexadecimal::encode(
      infinit::cryptography::oneway::hash(
        infinit::cryptography::Plain(buffer),
        infinit::cryptography::oneway::Algorithm::md5).buffer()
    );
    return res;
  }

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

  std::string
  S3::_amz_date(RequestTime const& request_time)
  {
    std::string date = boost::posix_time::to_iso_string(request_time);
    date = date.substr(0, 15);
    date += "Z";
    return date;
  }

  std::vector<std::string>
  S3::_signed_headers(RequestHeaders const& headers)
  {
    std::vector<std::string> signed_headers;
    for (auto header: headers)
      signed_headers.push_back(header.first);
    return signed_headers;
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

  std::vector<std::pair<std::string, S3::FileSize>>
  S3::_parse_list_xml(std::istream& stream)
  {
    std::vector<std::pair<std::string, FileSize>> res;
    using boost::property_tree::ptree;
    ptree file_list;
    read_xml(stream, file_list);
    for (auto const& base_element: file_list.get_child("ListBucketResult"))
    {
      if (base_element.first == "Contents")
      {
        std::string fname = base_element.second.get<std::string>("Key");
        if (fname != elle::sprintf("%s/", this->_remote_folder))
        {
          int pos = fname.size() - this->_remote_folder.size();
          fname = fname.substr(this->_remote_folder.size() + 1, fname.size() -
                               (pos + 1));
          FileSize fsize = base_element.second.get<FileSize>("Size");
          std::pair<std::string, S3::FileSize> file_pair(fname, fsize);
          res.push_back(file_pair);
        }
      }
    }
    return res;
  }

  RequestHeaders
  S3::_make_put_headers(elle::ConstWeakBuffer const& object,
                        RequestTime const& request_time)
  {
    ELLE_DUMP("%s: generate PUT headers", *this);
    RequestHeaders headers;
    headers["Content-Type"] = std::string("binary/octet-stream");
    headers["Host"] = this->_host_name;
    headers["x-amz-date"] = this->_amz_date(request_time);
    headers["x-amz-content-sha256"] = this->_sha256_hexdigest(object);
    headers["x-amz-storage-class"] = std::string("REDUCED_REDUNDANCY");
    headers["x-amz-security-token"] = this->_credentials.session_token();
    return headers;
  }

  RequestHeaders
  S3::_make_generic_headers(RequestTime const& request_time)
  {
    ELLE_DUMP("%s: generate LIST or GET headers", *this);
    RequestHeaders headers;
    headers["Host"] = this->_host_name;
    headers["Content-Type"] = std::string("application/json");
    elle::ConstWeakBuffer empty_object("");
    headers["x-amz-date"] = this->_amz_date(request_time);
    headers["x-amz-content-sha256"] = this->_sha256_hexdigest(empty_object);
    headers["x-amz-security-token"] = this->_credentials.session_token();
    return headers;
  }

  aws::CanonicalRequest
  S3::_make_put_canonical_request(elle::ConstWeakBuffer const& object,
                                  std::string const& object_name,
                                  RequestHeaders const& headers)
  {
    RequestQuery empty_query;

    aws::CanonicalRequest res(
      reactor::http::Method::PUT,
      elle::sprintf("/%s/%s", this->_remote_folder, object_name),
      empty_query,
      headers,
      this->_signed_headers(headers),
      _sha256_hexdigest(object)
    );
    ELLE_DUMP("%s: generated PUT canonical request: %s",
              *this, res.canonical_request());
    return res;
  }

  aws::CanonicalRequest
  S3::_make_list_canonical_request(RequestHeaders const& headers,
                                   RequestQuery const& query)
  {
    elle::ConstWeakBuffer empty_object("");

    aws::CanonicalRequest res(
      reactor::http::Method::GET,
      "/",
      query,
      headers,
      this->_signed_headers(headers),
      _sha256_hexdigest(empty_object)
    );
    ELLE_DUMP("%s: generated LIST canonical request: %s",
              *this, res.canonical_request());
    return res;
  }

  aws::CanonicalRequest
  S3::_make_get_canonical_request(RequestHeaders const& headers,
                                  std::string const& object_name)
  {
    RequestQuery empty_query;
    elle::ConstWeakBuffer empty_object("");

    aws::CanonicalRequest res(
      reactor::http::Method::GET,
      elle::sprintf("/%s/%s", this->_remote_folder, object_name),
      empty_query,
      headers,
      this->_signed_headers(headers),
      _sha256_hexdigest(empty_object)
    );
    ELLE_DUMP("%s: generated GET canonical request: %s",
              *this, res.canonical_request());
    return res;
  }

  aws::CanonicalRequest
  S3::_make_delete_canonical_request(RequestHeaders const& headers,
                                     std::string const& object_name)
  {
    RequestQuery empty_query;
    elle::ConstWeakBuffer empty_object("");

    aws::CanonicalRequest res(
      reactor::http::Method::DELETE,
      elle::sprintf("/%s/%s", this->_remote_folder, object_name),
      empty_query,
      headers,
      this->_signed_headers(headers),
      _sha256_hexdigest(empty_object)
    );
    ELLE_DUMP("%s: generated DELETE canonical request: %s",
              *this, res.canonical_request());
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
