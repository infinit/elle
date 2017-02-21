#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <elle/cryptography/hash.hh>
#include <elle/format/hexadecimal.hh>
#include <elle/json/json.hh>
#include <elle/test.hh>
#include <elle/service/aws/CanonicalRequest.hh>
#include <elle/service/aws/Credentials.hh>
#include <elle/service/aws/Exceptions.hh>
#include <elle/service/aws/Keys.hh>
#include <elle/service/aws/S3.hh>
#include <elle/service/aws/SigningKey.hh>
#include <elle/service/aws/StringToSign.hh>

#include <reactor/scheduler.hh>
#include <reactor/thread.hh>


ELLE_LOG_COMPONENT("elle.services.aws.test");

// Credentials:
// AccessKeyId
// SecretAccessKey
// SessionToken
// Expiration


static
std::string _bucket_name("us-east-1-buffer-dev-infinit-io");

static
boost::posix_time::ptime _now =
  boost::posix_time::second_clock::universal_time();

static
boost::posix_time::ptime _later = _now + boost::posix_time::hours(1);

static
elle::service::aws::Credentials _GET_credentials(
  "ASIAJOU5RQKL2N6YKOXQ",
  "9yC/yEGXvjH+3At0p6GOt04K8cgeol+klLlrFkLY",
  "AQoDYXdzEGIagAPirqRMvxZjBVQrtGqwzzTKcfyU0cn+a5uyJPto88J09j/WYZdqrQ8YEr6MAk3sXcWlgtZ+k259fkSCqfkwg1ntS/SkrtDaC7GokhnOMRXsqGYV7Hf48QyWV3XpnIp3x0XgJKOvq2Mwfn1ZuFy7364Qk/5lkYLuQ/i2EqMXRgzZFH2TPkYk8+k4l4sNJf1c4/Vl9FapstpbwYha96oAFbZSd9GfTIkFXXhXMCxykE9uDxM2STYGkoFkE37doZUoZB3n6pjPdnV60Hn/KmgRTEC8fn/SdYrmpnpXIRcPi8tL+iOa3x74LcUv0KysNqVZuBIScVCmVnjNxhueSz7BdM9Zo7NsGNzrWBTUShjoXmw+a92eINvCKRv+buD4PYNGk+HekJxF3IGjUGaoBYJkfpwCYJMDsfs/R7DPG3qzqJN465bXDj/8RZmps7qb3ew8AhasWT8cMUDPr2ClGUwLaVkyH1KBvKxc6hhxzZOPuJq4ZzGQ/4axoM1CtP0UjerJdNMg/oDMmQU=",
  "us-east-1",
  _bucket_name,
  "testing",
  _later,
  _now
);

static
elle::service::aws::Credentials _PUT_credentials(
  "ASIAJAGQUOJVE3HO46ZA",
  "pG1iz87bCYOZ9sQ4fkryBgiS/wnS7K+2MJUmS7Hb",
  "AQoDYXdzEGIawAIayH3x2pmusA2EhnhOK+42f07Y0cQILf7yltnXMG6sJ6sbE+o0IgZW8lEkCcZRGcXD1fM6pGAEBEYgO9ZJVXvRG5+riMGXmb7RHWGEEYRidqylFqK7vepkU2+ocFBshxKoSOJMCl5yjU351J3INvkkeTNJ9lpz0mPBWpEv82uBYzo6j0oDJMJutaiFvmRfpS79KWHemOcgUxbjTKChms3askIDJf6zgDTyIymWaIGquFgKwNgCO6UNk2zCCiWf3vtryuPxrlERz/Xm9cCejC4B4FiTjes6fWH7jlxbDu0s+6s7ivdSga68Ex4OJbetpYIn3vOTMTRzyUN/0TvMJPZFRn8joW3cjnJOQNMtoEmtS7Hp1fvOBkk/f1qWAM/GrMvbcJThOAjPuhqbJY7DpP4SNQtGgdwhOjC04Z3fUDgqnyDR/8uZBQ==",
  "us-east-1",
  "none",
  "/",
  _later,
  _now
);



// Test derived from link below with their query string instead of none.
// http://docs.aws.amazon.com/general/latest/gr/sigv4-create-canonical-request.html
static
elle::service::aws::CanonicalRequest
_make_canonical_request()
{
  std::map<std::string, std::string> query;
  query["X-Amz-Algorithm"] = std::string("AWS4-HMAC-SHA256");
  query["Action"] = std::string("ListUsers");
  query["Version"] = std::string("2010-05-08");
  query["X-Amz-Credentials"] =
    std::string("AKIAIOSFODNN7EXAMPLE/20110909/us-east-1/iam/aws4_request");
  query["X-Amz-Date"] = std::string("20110909T233600Z");

  std::map<std::string, std::string> headers;
  headers["content-type"] =
    std::string("application/x-www-form-urlencoded; charset=utf-8");
  headers["host"] = std::string("iam.amazonaws.com");
  headers["x-amz-date"] = std::string("20110909T233600Z");

  std::vector<std::string> signed_headers;
  std::string signed_headers_str;
  for (auto header: headers)
  {
    signed_headers.push_back(header.first);
    signed_headers_str += elle::sprintf("%s;", header.first);
  }
  signed_headers_str =
    signed_headers_str.substr(0, signed_headers_str.size() - 1);
  query["X-Amz-SignedHeaders"] = signed_headers_str;

  std::string content("Action=ListUsers&Version=2010-05-08");
  auto digest = elle::cryptography::hash(content,
                                         elle::cryptography::Oneway::sha256);

  elle::service::aws::CanonicalRequest request(
    reactor::http::Method::POST,
    "/",
    query,
    headers,
    signed_headers,
    elle::format::hexadecimal::encode(digest)
  );
  ELLE_DEBUG("canonical request: %s", request);
  return request;
}

// http://docs.aws.amazon.com/general/latest/gr/sigv4-create-string-to-sign.html
static
elle::service::aws::StringToSign
_make_string_to_sign()
{
  boost::posix_time::ptime request_time(
    boost::gregorian::date(2011, boost::gregorian::Sep, 9),
    boost::posix_time::hours(23) + boost::posix_time::minutes(36)
  );
  elle::service::aws::CanonicalRequest canonical_request = _make_canonical_request();
  elle::service::aws::CredentialScope credential_scope(request_time, elle::service::aws::Service::iam,
                                        "us-east-1");
  elle::service::aws::StringToSign string_to_sign(request_time, credential_scope,
                                   canonical_request.sha256_hash(),
                                   elle::service::aws::SigningMethod::aws4_hmac_sha256);
  ELLE_DEBUG("string to sign: %s", string_to_sign);
  return string_to_sign;
}

ELLE_TEST_SCHEDULED(canonical_request)
{
  elle::service::aws::CanonicalRequest request = _make_canonical_request();
  std::string expected_output = "POST\n/\nAction=ListUsers&Version=2010-05-08&X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credentials=AKIAIOSFODNN7EXAMPLE%2F20110909%2Fus-east-1%2Fiam%2Faws4_request&X-Amz-Date=20110909T233600Z&X-Amz-SignedHeaders=content-type%3Bhost%3Bx-amz-date\ncontent-type:application/x-www-form-urlencoded; charset=utf-8\nhost:iam.amazonaws.com\nx-amz-date:20110909T233600Z\n\ncontent-type;host;x-amz-date\nb6359072c78d70ebee1e81adcbab4f01bf2c23245fa365ef83fe8f1f955085e2";
  std::string expected_hash = "d030ae173c82eeffc6a53f427a3ddafcef9aad71aef5ac9f82122e0742b8c1b8";
  BOOST_CHECK_EQUAL(request.canonical_request(), expected_output);
  BOOST_CHECK_EQUAL(request.sha256_hash(), expected_hash);
}

ELLE_TEST_SCHEDULED(string_to_sign)
{
  elle::service::aws::StringToSign string_to_sign = _make_string_to_sign();
  std::string expected = "AWS4-HMAC-SHA256\n20110909T233600Z\n20110909/us-east-1/iam/aws4_request\nd030ae173c82eeffc6a53f427a3ddafcef9aad71aef5ac9f82122e0742b8c1b8";
  BOOST_CHECK_EQUAL(string_to_sign.string(), expected);
}

// http://docs.aws.amazon.com/general/latest/gr/signature-v4-examples.html
ELLE_TEST_SCHEDULED(signing_key)
{
  std::string aws_secret("wJalrXUtnFEMI/K7MDENG+bPxRfiCYEXAMPLEKEY");
  boost::posix_time::ptime request_time(
    boost::gregorian::date(2012, boost::gregorian::Feb,15));
  elle::service::aws::SigningKey signing_key(aws_secret, request_time,
                              "us-east-1",
                              elle::service::aws::Service::iam);
  BOOST_CHECK_EQUAL(
    elle::format::hexadecimal::encode(signing_key.key()),
    "f4780e2d9f65fa895f9c67b32ce1baf0b0d8a43505a000a1a9e090d414db404d");
}

ELLE_TEST_SCHEDULED(sign_request)
{
  elle::service::aws::StringToSign string_to_sign = _make_string_to_sign();
  std::string aws_secret("wJalrXUtnFEMI/K7MDENG+bPxRfiCYEXAMPLEKEY");
  boost::posix_time::ptime request_time(
    boost::gregorian::date(2011, boost::gregorian::Sep, 9),
    boost::posix_time::hours(23) + boost::posix_time::minutes(36)
  );
  elle::service::aws::SigningKey signing_key(aws_secret, request_time,
                              "us-east-1",
                              elle::service::aws::Service::iam);
  BOOST_CHECK_EQUAL(
    signing_key.sign_message(string_to_sign.string()),
    "c9d1c4e90e9f0b65ae4020a33bada35341ee2f8188c70b2a976e6e767414ed1f");
}

// // Should only be run manually with generated crendentials.
// ELLE_TEST_SCHEDULED(s3_put)
// {
//   elle::service::aws::S3 s3_handler(_PUT_credentials);
//   elle::ConstWeakBuffer object("a fair bit of stuff in a file!");
//   BOOST_CHECK_NO_THROW(s3_handler.put_object(object, "test_object"));
//   BOOST_CHECK_NO_THROW(s3_handler.put_object(object, "test_object_1"));
//   BOOST_CHECK_NO_THROW(s3_handler.put_object(object, "x_different"));
//   BOOST_CHECK_NO_THROW(s3_handler.put_object(object, "x_different_1"));
// }

// // Should only be run manually with generated credentials.
// ELLE_TEST_SCHEDULED(s3_list)
// {
//   elle::service::aws::S3 s3_handler(_GET_credentials);
//   std::vector<std::pair<std::string, elle::service::aws::S3::FileSize>> all =
//     s3_handler.list_remote_folder();
//   BOOST_CHECK_EQUAL(all.size(), 4);
//   for (auto const& item: all)
//   {
//     ELLE_LOG("ALL filename: %s (%s)", item.first, item.second);
//   }
//   std::vector<std::pair<std::string, elle::service::aws::S3::FileSize>> some =
//     s3_handler.list_remote_folder("x");
//   BOOST_CHECK_EQUAL(some.size(), 2);
//   for (auto const& item: some)
//   {
//     ELLE_LOG("SOME filename: %s (%s)", item.first, item.second);
//   }
// }

// // Should only be run manually with generated credentials.
// ELLE_TEST_SCHEDULED(s3_get)
// {
//   elle::service::aws::S3 s3_handler(_GET_credentials);
//   elle::ConstWeakBuffer expected("a fair bit of stuff in a file!");
//   BOOST_CHECK_EQUAL(s3_handler.get_object("test_object"), expected);
//   BOOST_CHECK_THROW(s3_handler.get_object("doesnt_exist"), elle::service::aws::FileNotFound);
// }

// // Should only be run manually with generated credentials.
// ELLE_TEST_SCHEDULED(s3_delete)
// {
//   elle::service::aws::S3 s3_handler(_GET_credentials);
//   BOOST_CHECK_NO_THROW(s3_handler.delete_object("test_object"));
// }

ELLE_TEST_SUITE()
{
  auto timeout = RUNNING_ON_VALGRIND ? 10 : 3;
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(canonical_request), 0, timeout);
  suite.add(BOOST_TEST_CASE(string_to_sign), 0, timeout);
  suite.add(BOOST_TEST_CASE(signing_key), 0, timeout);
  suite.add(BOOST_TEST_CASE(sign_request), 0, timeout);

  // Should only be run manually with generated crendentials.
  // suite.add(BOOST_TEST_CASE(s3_put), 0, timeout * 3);
  // suite.add(BOOST_TEST_CASE(s3_list), 0, timeout * 3);
  // suite.add(BOOST_TEST_CASE(s3_get), 0, timeout * 3);
  // suite.add(BOOST_TEST_CASE(s3_delete), 0, timeout * 3);
}

// -----------------------
// Generate a token script
// -----------------------
// #!/usr/bin/env python3

// # Launch command:
// # ELLE_LOG_LEVEL=infinit.oracles.meta.CloudBufferToken:DEBUG PYTHONPATH=~/sandbox/infinit/_build/macosx64/oracles/meta/server/lib/python:~/sandbox/infinit/oracles:~/sandbox/infinit/oracles/meta/server/src/infinit/oracles/meta/server ./make_token.py

// from cloud_buffer_token import CloudBufferToken
// token_maker = CloudBufferToken('test_user', 'testing', 'GET')

// --------------------------------
// Python code for testing signing:
// --------------------------------
// import hashlib
// import hmac
// import binascii

// def aws_sign(key, message):
//   return hmac.new(key, message.encode('utf-8'), hashlib.sha256).digest()

// def calc_signature(message):
//   aws_secret = 'AWS4wJalrXUtnFEMI/K7MDENG+bPxRfiCYEXAMPLEKEY'
//   aws_date = '20110909'
//   aws_region = 'us-east-1'
//   aws_service = 'iam'
//   k_date = aws_sign(bytes(aws_secret, 'utf-8'), aws_date)
//   k_region = aws_sign(k_date, aws_region)
//   k_service = aws_sign(k_region, aws_service)
//   k_signing = aws_sign(k_service, "aws4_request")
//   print('key: %s \nsigned message: %s' % (
//     binascii.hexlify(k_signing),
//     binascii.hexlify(aws_sign(k_signing, message))))
