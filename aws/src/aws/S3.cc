#include <aws/S3.hh>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <elle/finally.hh>
#include <elle/format/hexadecimal.hh>
#include <elle/format/base64.hh>
#include <elle/log.hh>
#include <elle/os/environ.hh>

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
  static
  boost::posix_time::time_duration
  default_timeout()
  {
    static boost::optional<boost::posix_time::time_duration> v;
    if (!v)
    {
      v = 500_sec;
      std::string opt = elle::os::getenv("INFINIT_S3_TIMEOUT", "");
      if (!opt.empty())
        v = boost::posix_time::seconds(boost::lexical_cast<int>(opt));
    }
    return v.get();
  }

  // Stay as close as possible to reference java implementation from amazon
  // http://docs.aws.amazon.com/AmazonS3/latest/API/sig-v4-header-based-auth.html
  static
  std::string
  uri_encode(std::string const& input, bool encodeSlash)
  {
    const char* syms = "0123456789ABCDEF";
    std::string result;
    for (unsigned i = 0; i < input.length() ; ++i)
    {
      char ch = input[i];
      if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9') || ch == '_' || ch == '-' || ch == '~' || ch == '.') {
        result+= ch;
      }
      else if (ch == '/') {
        result += (encodeSlash ? "%2F" : "/");
      }
      else
        result += std::string("%") + syms[((unsigned char)ch)>>4] + syms[((unsigned char)ch)&0x0F];
    }
    return result;
  }

  /*-------------.
  | Construction |
  `-------------*/
  S3::S3(aws::Credentials const& credentials):
    _credentials(credentials),
    _host_name(elle::sprintf("%s.s3.amazonaws.com", this->_credentials.bucket()))
  {}

  S3::S3(std::function<Credentials(bool)> query_credentials)
    : _credentials(query_credentials(true))
    , _host_name(elle::sprintf("%s.s3.amazonaws.com", this->_credentials.bucket()))
    , _query_credentials(query_credentials)
  {}

  static std::string query_parameters(RequestQuery const& query)
  {
    using reactor::http::EscapedString;
    if (query.empty())
      return "";
    std::string res = "?";
    for (auto const& parameter: query)
    {
      res += elle::sprintf("%s=%s&", EscapedString(parameter.first),
                                 EscapedString(parameter.second));
    }
    res = res.substr(0, res.size()-1);
    return res;
  }
  /*-----------.
  | Operations |
  `-----------*/

  std::string S3::put_object(elle::ConstWeakBuffer const& object,
                       std::string const& object_name,
                       RequestQuery const& query,
                       bool ommit_redundancy)
  {

    ELLE_TRACE_SCOPE("%s: PUT block: %s", *this, object_name);

    // Make headers.
    RequestHeaders headers;
    if (!ommit_redundancy)
      headers["x-amz-storage-class"] = std::string("REDUCED_REDUNDANCY");

    auto url = elle::sprintf(
      "/%s/%s",
      this->_credentials.folder(),
      object_name
    );
    ELLE_DEBUG("url: %s", url);

    auto request = _build_send_request(
      url, reactor::http::Method::PUT,
      query, headers,
      "binary/octet-stream",
      object);
    auto const& response = request->headers();
    auto etag = response.find("ETag");
    if (etag == response.end())
      return "";
    else
      return etag->second;
  }

  std::vector<std::pair<std::string, S3::FileSize>>
  S3::list_remote_folder(std::string const& marker)
  {
    ELLE_TRACE_SCOPE("%s: LIST remote folder", *this);

    RequestQuery query;
    query["prefix"] = elle::sprintf("%s/", this->_credentials.folder());
    query["delimiter"] = "/";
    if (marker.size() > 0)
      query["marker"] = elle::sprintf("%s/%s", this->_credentials.folder(), marker);

    auto request = _build_send_request("/", reactor::http::Method::GET, query);
    return this->_parse_list_xml(*request);
   }

  std::vector<std::pair<std::string, S3::FileSize>>
  S3::list_remote_folder_full()
  {
    std::string marker;
    bool first = true;
    std::vector<std::pair<std::string, S3::FileSize>> result;
    std::vector<std::pair<std::string, S3::FileSize>> chunk;
    do
    {
      chunk = list_remote_folder(marker);
      marker = chunk.back().first;
        result.insert(result.end(),
                      first ? chunk.begin():chunk.begin()+1, chunk.end());
      first = false;
    }
    while (chunk.size() >= 1000);
    return result;
  }

  elle::Buffer
  S3::get_object_chunk(std::string const& object_name,
                       FileSize offset, FileSize size)
  {
    RequestHeaders headers;
    headers["Range"] = elle::sprintf("bytes=%s-%s", offset, offset+size-1);
    return get_object(object_name, headers);
  }

  elle::Buffer
  S3::get_object(std::string const& object_name, RequestHeaders headers)
  {
    ELLE_TRACE_SCOPE("%s: GET remote object", *this);

    auto url = elle::sprintf(
      "/%s/%s",
      this->_credentials.folder(),
      object_name
    );
    ELLE_DEBUG("url: %s", url);

    auto request = _build_send_request(url, reactor::http::Method::GET,
                                       RequestQuery(), headers);
    auto response = request->response();
    std::string calcd_md5(this->_md5_digest(response));
    std::string aws_md5(request->headers().at("ETag"));
    // Remove quotes around MD5 sum from AWS.
    aws_md5 = aws_md5.substr(1, aws_md5.size() - 2);
    // AWS sends as ETAG for ranged get FULLMD5-CHUNK, we cannot validate it.
    if (aws_md5.find('-') == std::string::npos && calcd_md5 != aws_md5)
    {
      throw aws::CorruptedData(
        elle::sprintf("%s: GET data corrupt: %s != %s", *this, calcd_md5,
                      aws_md5));
    }
    return response;
  }


  void
  S3::delete_folder()
  {
    ELLE_DEBUG("%s: fetching folder content", *this);
    std::vector<std::pair<std::string, S3::FileSize>> content =
      list_remote_folder_full();
    ELLE_DEBUG("%s: deleting %s objects", *this, content.size());
    // multi-delete has a limit at 1000 items...in theory. But passing 1000
    // objects asplode the AWS xml parser.
    int block_size = 200;
    for (int i = 0; i < (int)content.size(); i+= block_size)
    {    // build delete request payload
      std::string xml;
      xml += "<Delete><Quiet>true</Quiet>";
      for (int j=i; j < (int)content.size() && j-i <= block_size; ++j)
      {
        xml += "<Object><Key>"
          + _credentials.folder() + "/" + content[j].first + "</Key></Object>";
      }
      xml += "</Delete>";
      elle::ConstWeakBuffer payload(xml);
      // build the request
      RequestHeaders headers;
      headers["Content-MD5"] = elle::format::base64::encode(
          infinit::cryptography::oneway::hash(
          infinit::cryptography::Plain(payload),
          infinit::cryptography::oneway::Algorithm::md5).buffer()).string();

      RequestQuery query;
      query["delete"] = "";

      _build_send_request("/", reactor::http::Method::POST,
        query, headers,
        "text/xml", payload);
    }
    delete_object(_credentials.folder());
  }

  void
  S3::delete_object(std::string const& object_name, RequestQuery const& query)
  {
    ELLE_TRACE_SCOPE("%s: DELETE remote object", *this);

    auto url = elle::sprintf(
      "/%s/%s",
      this->_credentials.folder(),
      object_name
    );
    ELLE_DEBUG("url: %s", url);

    _build_send_request(url, reactor::http::Method::DELETE, query);

  }

  std::string
  S3::multipart_initialize(std::string const& object_name,
                           std::string const& mime_type)
  {
    ELLE_TRACE_SCOPE("%s: initialize multipart: %s", *this, object_name);
    RequestHeaders headers;
    headers["x-amz-storage-class"] = std::string("REDUCED_REDUNDANCY");
    RequestQuery query;
    query["uploads"] = "";
    auto url = elle::sprintf(
      "/%s/%s",
      this->_credentials.folder(),
      object_name
    );
    ELLE_DUMP("url: %s", url);

    auto request = _build_send_request(
      url, reactor::http::Method::POST,
      query, headers,
      mime_type);
    using boost::property_tree::ptree;
    ptree response;
    read_xml(*request, response);
    return response.get<std::string>("InitiateMultipartUploadResult.UploadId");
  }

  std::string
  S3::multipart_upload(std::string const& object_name,
                       std::string const& upload_key,
                       elle::ConstWeakBuffer const& object,
                       int chunk)
  {
    RequestQuery query;
    query["partNumber"] = boost::lexical_cast<std::string>(chunk+1);
    query["uploadId"] = upload_key;
    return put_object(object, object_name, query, true);
  }

  void
  S3::multipart_finalize(std::string const& object_name,
                         std::string const& upload_key,
                         std::vector<MultiPartChunk> const& chunks)
  {
    ELLE_TRACE_SCOPE("%s: finalize multipart: %s", *this, object_name);

    // Generate request payload
    std::string xchunks;
    xchunks = "<CompleteMultipartUpload>";
    for (auto const& chunk: chunks)
    {
      xchunks += elle::sprintf("<Part><PartNumber>%s</PartNumber><ETag>\"%s\"</ETag></Part>",
                                chunk.first+1, chunk.second);
    }
    xchunks += "</CompleteMultipartUpload>";
    RequestQuery query;
    query["uploadId"] = upload_key;

    auto url = elle::sprintf(
      "/%s/%s",
      this->_credentials.folder(),
      object_name
    );

    auto request = _build_send_request(url, reactor::http::Method::POST,
      query, RequestHeaders(),
      "text/xml", xchunks);
    // This request can 200 OK and still return an error in XML
    using boost::property_tree::ptree;
    ptree response;

    read_xml(*request, response);
    if (response.find("Error") != response.not_found()
      || response.find("CompleteMultipartUploadResult") == response.not_found())
    {
      throw aws::RequestError(elle::sprintf("%s: during S3 finalize on %s:%s",
                                            *this, object_name, request->response()));
    }
  }

  void
  S3::multipart_abort(std::string const& object_name,
                      std::string const& upload_key)
  {
    RequestQuery query;
    query["uploadId"] = upload_key;
    delete_object(object_name, query);
  }

  std::vector<S3::MultiPartChunk>
  S3::multipart_list(std::string const& object_name,
                     std::string const& upload_key)
  {
    ELLE_TRACE_SCOPE("%s: LIST multipart chunks", *this);

    std::vector<S3::MultiPartChunk> res;
    int max_id = -1;
    while (true)
    {
      RequestQuery query;
       query["uploadId"] = upload_key;
      if (max_id != -1)
        query["part-number-marker"] = boost::lexical_cast<std::string>(max_id);
      auto url = elle::sprintf("/%s/%s",
                               this->_credentials.folder(),
                               object_name);

      auto request = _build_send_request(url, reactor::http::Method::GET, query);

      using boost::property_tree::ptree;
      ptree response;
      read_xml(*request, response);
      for (auto const& part: response.get_child("ListPartsResult"))
      {
        if (part.first != "Part")
          continue;
        MultiPartChunk chunk(part.second.get<int>("PartNumber") -1,
                             part.second.get<std::string>("ETag"));
        ELLE_DUMP("listed chunk %s %s", chunk.first, chunk.second);
        res.push_back(chunk);
      }
      if (!response.get<bool>("ListPartsResult.IsTruncated", false))
        return res;
      // recompute max_id for next request
      max_id = std::max_element(res.begin(), res.end(),
        [](MultiPartChunk const& a, MultiPartChunk const& b)
          { return a.first < b.first;})->first + 1;
      ELLE_DUMP("Marker for next iteration is %s", max_id);

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
                                          Service::s3, _credentials.region());

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
        if (fname != elle::sprintf("%s/", this->_credentials.folder()))
        {
          int pos = fname.size() - this->_credentials.folder().size();
          fname = fname.substr(this->_credentials.folder().size() + 1, fname.size() -
                               (pos + 1));
          FileSize fsize = base_element.second.get<FileSize>("Size");
          std::pair<std::string, S3::FileSize> file_pair(fname, fsize);
          res.push_back(file_pair);
        }
      }
    }
    return res;
  }

  reactor::http::Request::Configuration
  S3::_initialize_request(RequestTime request_time,
                          CanonicalRequest const& canonical_request,
                          const RequestHeaders& initial_headers,
                          boost::posix_time::time_duration timeout)
  {

    // Make headers.
    RequestHeaders headers(initial_headers);
    StringToSign string_to_sign(this->_make_string_to_sign(
                                request_time, canonical_request.sha256_hash()));
    // Make Authorization header.
    // http://docs.aws.amazon.com/AmazonS3/latest/API/sig-v4-header-based-auth.html
    aws::SigningKey key(this->_credentials.secret_access_key(),
                        request_time,
                        _credentials.region(),
                        Service::s3);
    std::map<std::string, std::string> auth;
    // Make credential string.
    auth["AWS4-HMAC-SHA256 Credential"] =
      this->_credentials.credential_string(request_time, aws::Service::s3);
    // Make signed headers string.
    std::string signed_headers_str;
    for (auto const& header: headers)
      signed_headers_str += elle::sprintf("%s;", header.first);
    signed_headers_str =
      signed_headers_str.substr(0, signed_headers_str.size() - 1);
    auth["SignedHeaders"] = signed_headers_str;
    // Make signature string.
    auth["Signature"] =
      key.sign_message(string_to_sign.string());
    // Make authorization header string.
    std::string auth_str;
    for (auto const& item: auth)
      auth_str += elle::sprintf("%s=%s, ", item.first, item.second);
    auth_str = auth_str.substr(0, auth_str.size() - 2);
    ELLE_DUMP("Final authorization string: '%s'", auth_str);
    headers["Authorization"] = auth_str;

    // Add headers to request.
    reactor::http::Request::Configuration cfg(timeout,
                                              reactor::http::Version::v11);
    for (auto header: headers)
      cfg.header_add(header.first, header.second);
    return cfg;
  }

  void
  S3::_check_request_status(reactor::http::Request& request,
                            std::string const& url,
                            bool dump_response)
  {
    typedef reactor::http::StatusCode StatusCode;
    auto status = request.status();
    bool ok = false;
    bool fatal = false;
    elle::SafeFinally log_result([&]
      {
        using namespace elle::log;
        // response can only be read once, so don't touch it unless
        // allowed to. And we assume an error cond allows us to.
        if (dump_response || !ok)
          detail::Send(ok ? Logger::Level::dump : Logger::Level::log,
                       ok ? Logger::Type::info :
                           (fatal ? Logger::Type::warning : Logger::Type::info),
                       true, _trace_component_,
                       __FILE__, __LINE__, ELLE_COMPILER_PRETTY_FUNCTION,
                       "AWS S3 error '%s' on %s: %s",
                       status, url, request.response());
      });
    switch(status)
    {
      // Log response in case of error
      // http://docs.aws.amazon.com/AmazonS3/latest/API/ErrorResponses.html
      case StatusCode::Internal_Server_Error:
        throw TransientError("Internal Server Error");
      case StatusCode::Conflict:
        throw TransientError("Conflict");
      case StatusCode::Service_Unavailable:
        throw TransientError("Service Unavailable"); // SlowDown, Service Unavailable
      case StatusCode::Bad_Request:
      {
        // creds expired is a bad request with xml payload
        // <Error><Code>ExpiredToken</Code> ...
        // we need to distinguish that case
        using boost::property_tree::ptree;
        ptree response;
        // reading the response stream, nobody else will get it
        read_xml(request, response);
        std::string code = response.get<std::string>("Error.Code", "");
        ELLE_DEBUG("S3: Bad request reply with error code: %s", code);
        if (code == "ExpiredToken" || code == "TokenRefreshRequired")
          throw CredentialsExpired(elle::sprintf(
            "%s: credentials expired with '%s': %s",
            *this, code, this->_credentials));
        else if (code == "RequestTimeout") // yes aws can do that, it happens for real.
          throw TransientError("Request timed out");
        ELLE_TRACE("%s: assuming fatal error code: %s", *this, code);
        fatal = true;
        throw RequestError(
          elle::sprintf("%s: error on %s status '%s' code '%s'",
                        *this, url, status, code));
      }
      case reactor::http::StatusCode::Not_Found:
        fatal = true;
        throw aws::FileNotFound(
          elle::sprintf("%s: error on %s: object not found",
                        *this, url));
      case reactor::http::StatusCode::Forbidden:
        ELLE_TRACE("%s: error status: invalid credentials", *this);
        throw aws::CredentialsNotValid(
           elle::sprintf("%s: error on %s:, forbidden",
                         *this, url));
      case reactor::http::StatusCode::OK:
      case reactor::http::StatusCode::No_Content:
      case reactor::http::StatusCode::Partial_Content:
        ok = true;
        break;
      default:
        fatal = true;
        throw aws::RequestError(
          elle::sprintf("%s: on %s status '%s' payload '%s'",
                        *this, url, status, request.response()));
    }
    // ok case returns here, all other cases throw
  }

  /*----------.
  | Printable |
  `----------*/
  void
  S3::print(std::ostream& stream) const
  {
    stream << "S3: bucket=" << this->_credentials.bucket()
           <<    " remote_folder=" << this->_credentials.folder();
  }

  std::unique_ptr<reactor::http::Request>
  S3::_build_send_request(
    std::string const& url,
    reactor::http::Method method,
    RequestQuery const& query,
    RequestHeaders const& extra_headers,
    std::string const& content_type,
    elle::ConstWeakBuffer const& payload,
    boost::optional<boost::posix_time::time_duration> timeout_opt
    )
  {
    boost::posix_time::time_duration timeout = default_timeout();
    if (timeout_opt)
      timeout = timeout_opt.get();
    // Transient errors on requests is perfectly reasonable
    int attempt = 0;
    static int max_attempts = -1;
    if (max_attempts == -1)
    {
      max_attempts = 0;
      std::string opt = elle::os::getenv("INFINIT_S3_MAX_ATTEMPTS", "");
      if (!opt.empty())
        max_attempts = boost::lexical_cast<int>(opt);
    }
    while (true)
    {
      RequestTime request_time =
        boost::posix_time::second_clock::universal_time();
      RequestHeaders headers(extra_headers);
      headers["x-amz-date"] = this->_amz_date(request_time);
      headers["x-amz-content-sha256"] = this->_sha256_hexdigest(payload);
      headers["x-amz-security-token"] = this->_credentials.session_token();
      headers["Host"] = this->_host_name;
      if (!content_type.empty())
        headers["Content-Type"] = content_type;

      std::string url_encoded = uri_encode(url, false);
      // false means no encoding the slashes, see:
      // http://docs.aws.amazon.com/AmazonS3/latest/API/sig-v4-header-based-auth.html
      CanonicalRequest canonical_request(
        method, url_encoded,  query, headers,
        _signed_headers(headers), _sha256_hexdigest(payload)
      );
      reactor::http::Request::Configuration cfg(
        _initialize_request(request_time, canonical_request, headers, timeout));
      std::string full_url = elle::sprintf(
        "https://%s:%s%s%s",
        _host_name,
        "443",
        url_encoded,
        query_parameters(query)
        );
      ELLE_DEBUG("Full url: %s", full_url);
      for (auto const& h: headers)
        ELLE_DUMP("header %s: %s", h.first, h.second);
      std::unique_ptr<reactor::http::Request> request;
      try
      {
        request = elle::make_unique<reactor::http::Request>(full_url, method, cfg);
        if (payload.size())
          request->write(reinterpret_cast<char const*>(payload.contents()),
            payload.size());
        reactor::wait(*request);
      }
      catch (reactor::http::RequestError const& e)
      {
        ++attempt;
        // we have nothing better to do, so keep retrying
        ELLE_LOG("S3 request error: %s (attempt %s)", e.error(), attempt);
        if (max_attempts && attempt >= max_attempts)
          throw aws::RequestError(
            elle::sprintf("%s: unable to PUT on S3, unable to perform HTTP request: %s",
                        *this, e.error()));
        else
          continue;
      }

      try
      {
        _check_request_status(*request, url);
      }
      catch(CredentialsExpired const&)
      {
        ELLE_TRACE("%s: aws credentials expired at %s (can_query=%s)",
          *this, _credentials.expiration_str(), !!_query_credentials);
        if (!_query_credentials)
          throw;
        _credentials = _query_credentials(false);
        _host_name = elle::sprintf("%s.s3.amazonaws.com",
                                   this->_credentials.bucket());
        ELLE_TRACE("%s: acquired new credentials expiring %s",
          *this,  _credentials.expiration_str());
        continue;
      }
      catch(TransientError const& err)
      {
        ++attempt;
        // we have nothing better to do, so keep retrying
        ELLE_LOG("S3 transient error '%s' (attempt %s)", err.what(), attempt);
        if (max_attempts && attempt >= max_attempts)
          throw aws::RequestError(
            elle::sprintf("%s: unable to PUT on S3, unable to perform HTTP request: timeout",
                        *this));
        else
          continue;
      }
      // Consider all other failures as fatal errors
      return std::move(request);
    }
  }
}
