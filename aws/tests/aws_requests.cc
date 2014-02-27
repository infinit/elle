#include <aws/CanonicalRequest.hh>
#include <aws/Credentials.hh>
#include <aws/Exceptions.hh>
#include <aws/Keys.hh>
#include <aws/S3.hh>
#include <aws/SigningKey.hh>
#include <aws/StringToSign.hh>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <elle/format/hexadecimal.hh>
#include <elle/json/json.hh>
#include <elle/test.hh>

#include <reactor/scheduler.hh>
#include <reactor/thread.hh>

#include <cryptography/oneway.hh>

ELLE_LOG_COMPONENT("aws.test");

// Test derived from link below with their query string instead of none.
// http://docs.aws.amazon.com/general/latest/gr/sigv4-create-canonical-request.html
static
aws::CanonicalRequest
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
  infinit::cryptography::Digest digest = infinit::cryptography::oneway::hash(
    infinit::cryptography::Plain(elle::ConstWeakBuffer(content)),
    infinit::cryptography::oneway::Algorithm::sha256
  );

  aws::CanonicalRequest request(
    reactor::http::Method::POST,
    "/",
    query,
    headers,
    signed_headers,
    elle::format::hexadecimal::encode(digest.buffer())
  );
  ELLE_DEBUG("canonical request: %s", request);
  return request;
}

// http://docs.aws.amazon.com/general/latest/gr/sigv4-create-string-to-sign.html
static
aws::StringToSign
_make_string_to_sign()
{
  boost::posix_time::ptime request_time(
    boost::gregorian::date(2011, boost::gregorian::Sep, 9),
    boost::posix_time::hours(23) + boost::posix_time::minutes(36)
  );
  aws::CanonicalRequest canonical_request = _make_canonical_request();
  aws::CredentialScope credential_scope(request_time, aws::Service::iam,
                                        aws::Region::us_east_1);
  aws::StringToSign string_to_sign(request_time, credential_scope,
                                   canonical_request.sha256_hash(),
                                   aws::SigningMethod::aws4_hmac_sha256);
  ELLE_DEBUG("string to sign: %s", string_to_sign);
  return string_to_sign;
}

ELLE_TEST_SCHEDULED(canonical_request)
{
  aws::CanonicalRequest request = _make_canonical_request();
  std::string expected_output = "POST\n/\nAction=ListUsers&Version=2010-05-08&X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credentials=AKIAIOSFODNN7EXAMPLE%2F20110909%2Fus-east-1%2Fiam%2Faws4_request&X-Amz-Date=20110909T233600Z&X-Amz-SignedHeaders=content-type%3Bhost%3Bx-amz-date\ncontent-type:application/x-www-form-urlencoded; charset=utf-8\nhost:iam.amazonaws.com\nx-amz-date:20110909T233600Z\n\ncontent-type;host;x-amz-date\nb6359072c78d70ebee1e81adcbab4f01bf2c23245fa365ef83fe8f1f955085e2";
  std::string expected_hash = "d030ae173c82eeffc6a53f427a3ddafcef9aad71aef5ac9f82122e0742b8c1b8";
  BOOST_CHECK_EQUAL(request.canonical_request(), expected_output);
  BOOST_CHECK_EQUAL(request.sha256_hash(), expected_hash);
}

ELLE_TEST_SCHEDULED(string_to_sign)
{
  aws::StringToSign string_to_sign = _make_string_to_sign();
  std::string expected = "AWS4-HMAC-SHA256\n20110909T233600Z\n20110909/us-east-1/iam/aws4_request\nd030ae173c82eeffc6a53f427a3ddafcef9aad71aef5ac9f82122e0742b8c1b8";
  BOOST_CHECK_EQUAL(string_to_sign.string(), expected);
}

// http://docs.aws.amazon.com/general/latest/gr/signature-v4-examples.html
ELLE_TEST_SCHEDULED(signing_key)
{
  std::string aws_secret("wJalrXUtnFEMI/K7MDENG+bPxRfiCYEXAMPLEKEY");
  boost::posix_time::ptime request_time(
    boost::gregorian::date(2012, boost::gregorian::Feb,15));
  aws::SigningKey signing_key(aws_secret, request_time,
                              aws::Region::us_east_1, aws::Service::iam);
  BOOST_CHECK_EQUAL(
    elle::format::hexadecimal::encode(signing_key.key().buffer()),
    "f4780e2d9f65fa895f9c67b32ce1baf0b0d8a43505a000a1a9e090d414db404d");
}

ELLE_TEST_SCHEDULED(sign_request)
{
  aws::StringToSign string_to_sign = _make_string_to_sign();
  std::string aws_secret("wJalrXUtnFEMI/K7MDENG+bPxRfiCYEXAMPLEKEY");
  boost::posix_time::ptime request_time(
    boost::gregorian::date(2011, boost::gregorian::Sep, 9),
    boost::posix_time::hours(23) + boost::posix_time::minutes(36)
  );
  aws::SigningKey signing_key(aws_secret, request_time,
                              aws::Region::us_east_1, aws::Service::iam);
  BOOST_CHECK_EQUAL(
    signing_key.sign_message(string_to_sign.string()),
    "c9d1c4e90e9f0b65ae4020a33bada35341ee2f8188c70b2a976e6e767414ed1f");
}

// Should only be run manually with generated crendentials.
ELLE_TEST_SCHEDULED(s3_put)
{
  std::string access_id("ASIAJHJ4PCAIXB42AIMA");
  std::string token("AQoDYXdzEKf//////////wEa0AJJQynCMikZmOjHgf8Xs7vCNkuGBFPXehVPsWODFvwEkfyNL98mMY+xai965DEWw4nENPdiHiH/v6E8+qcL6Fgsnp+UyzkngsrZmbtZtkXzJ8DJlqTxQnqku1O3Z6PIJ2YajIeY/+ISduuAUTQeOxug6knRBubHTqNH3no+La8apxdKkLo1wEiu+fOjpHPbbH9Q9Nf68z/zF2R+oPb+ZQ5ouhM6StpKTShFDe9CdGLi6VLrbGsJgLrmIHGyvqPVbYOIsHHlG3qBoghnw3yAoQjQ24Ez2mla0hYVFNWJe19iBSMPH3/8f+jo3a35oWslZPvNqY0Og2SMsay6OuGkeEI9+qxi0v2qcO51nSwQsuIq0f6md4Udxcf3wGK1l7QYcQDAX7q2wJWzOplApDHjNpTJ3SnBHNUDl69etUNIU81vv50IizAQfBBRnSdRJujo/JEg+/i8mAU=");
  std::string secret_access_key("MqWkQTVezR0TbFODL6SPQt6dn/1WDGK7R52gemlV");
  std::string expiry("2014-02-27T21:24:11Z");
  aws::Credentials credentials(access_id, secret_access_key, token, expiry);

  aws::S3 s3_handler("io.infinit.buffer.us0",
                     "testing",
                     credentials);

  elle::ConstWeakBuffer object("a fair bit of stuff in a file!");

  BOOST_CHECK_NO_THROW(s3_handler.put_object(object, "test_object_2"));
}

// Should only be run manually with generated credentials.
ELLE_TEST_SCHEDULED(s3_list)
{
  std::string access_id("ASIAJ23NEQTXMXM77WVQ");
  std::string token("AQoDYXdzEKb//////////wEakAPlkxDjjFF/MqgWDvevi4PVBVY1PeC8/cbTDruW67E9ocSf5/R1wA8cYamBSDYjKlFR0fxlBP+OxHY81mtTkoiHSZW3VMz9faWRq9X8+5PF8YxaFQk29d8puvxPmI6Hxd2oJKsw+ZHU1lotEabzs6b8bGn4h4KxAM9/bi4CrgOAj85Y32uoOIWlhGv4q3kwJRhZkLJZnDxYaRfYfxGe8z6SKzRX7yt0dwraPkCKdB1Gq8rVmPzkX+7/DSe7ItkS2IScik6xsVfbXDyJcf9cTdQQP/8QZzBR1aqH8DOuaOAMnSS7mLLcwA/bJjsNRnKjOZMYnMgM1JoOXpFP8mSMe9YuMr337coaEbN3pmVHF3SMgYvMdiFnF2zqBynuNA5RL8AP0vbQ6t1u5Y1PjYL21p4ZDgt9GYFFE2l19Tc8WvgT5nDyMdv312E3dkyb5rWkKZI/N04CP/CJz7oAVp89Zmq3mmfTR7fXtl233/qJ/ZudD3MWAdgL/xoiAuZzC5F6WM8r+5QINOoGvrvN4eMw3VRXILX2vJgF");
  std::string secret_access_key("UnUcvi/b2fXhmsx3rvGnPFcIQfDMiEYCnvJTFg9g");
  std::string expiry("2014-02-27T21:18:45Z");
  aws::Credentials credentials(access_id, secret_access_key, token, expiry);

  aws::S3 s3_handler("io.infinit.buffer.us0",
                     "testing",
                     credentials);
  s3_handler.list_remote_folder();
}

// Should only be run manually with generated credentials.
ELLE_TEST_SCHEDULED(s3_get)
{
  std::string access_id("ASIAJ23NEQTXMXM77WVQ");
  std::string token("AQoDYXdzEKb//////////wEakAPlkxDjjFF/MqgWDvevi4PVBVY1PeC8/cbTDruW67E9ocSf5/R1wA8cYamBSDYjKlFR0fxlBP+OxHY81mtTkoiHSZW3VMz9faWRq9X8+5PF8YxaFQk29d8puvxPmI6Hxd2oJKsw+ZHU1lotEabzs6b8bGn4h4KxAM9/bi4CrgOAj85Y32uoOIWlhGv4q3kwJRhZkLJZnDxYaRfYfxGe8z6SKzRX7yt0dwraPkCKdB1Gq8rVmPzkX+7/DSe7ItkS2IScik6xsVfbXDyJcf9cTdQQP/8QZzBR1aqH8DOuaOAMnSS7mLLcwA/bJjsNRnKjOZMYnMgM1JoOXpFP8mSMe9YuMr337coaEbN3pmVHF3SMgYvMdiFnF2zqBynuNA5RL8AP0vbQ6t1u5Y1PjYL21p4ZDgt9GYFFE2l19Tc8WvgT5nDyMdv312E3dkyb5rWkKZI/N04CP/CJz7oAVp89Zmq3mmfTR7fXtl233/qJ/ZudD3MWAdgL/xoiAuZzC5F6WM8r+5QINOoGvrvN4eMw3VRXILX2vJgF");
  std::string secret_access_key("UnUcvi/b2fXhmsx3rvGnPFcIQfDMiEYCnvJTFg9g");
  std::string expiry("2014-02-27T21:18:45Z");
  aws::Credentials credentials(access_id, secret_access_key, token, expiry);

  aws::S3 s3_handler("io.infinit.buffer.us0",
                     "testing",
                     credentials);
  s3_handler.get_object("test_object");
}

// Should only be run manually with generated credentials.
ELLE_TEST_SCHEDULED(s3_delete)
{
  std::string access_id("ASIAJ23NEQTXMXM77WVQ");
  std::string token("AQoDYXdzEKb//////////wEakAPlkxDjjFF/MqgWDvevi4PVBVY1PeC8/cbTDruW67E9ocSf5/R1wA8cYamBSDYjKlFR0fxlBP+OxHY81mtTkoiHSZW3VMz9faWRq9X8+5PF8YxaFQk29d8puvxPmI6Hxd2oJKsw+ZHU1lotEabzs6b8bGn4h4KxAM9/bi4CrgOAj85Y32uoOIWlhGv4q3kwJRhZkLJZnDxYaRfYfxGe8z6SKzRX7yt0dwraPkCKdB1Gq8rVmPzkX+7/DSe7ItkS2IScik6xsVfbXDyJcf9cTdQQP/8QZzBR1aqH8DOuaOAMnSS7mLLcwA/bJjsNRnKjOZMYnMgM1JoOXpFP8mSMe9YuMr337coaEbN3pmVHF3SMgYvMdiFnF2zqBynuNA5RL8AP0vbQ6t1u5Y1PjYL21p4ZDgt9GYFFE2l19Tc8WvgT5nDyMdv312E3dkyb5rWkKZI/N04CP/CJz7oAVp89Zmq3mmfTR7fXtl233/qJ/ZudD3MWAdgL/xoiAuZzC5F6WM8r+5QINOoGvrvN4eMw3VRXILX2vJgF");
  std::string secret_access_key("UnUcvi/b2fXhmsx3rvGnPFcIQfDMiEYCnvJTFg9g");
  std::string expiry("2014-02-27T21:18:45Z");
  aws::Credentials credentials(access_id, secret_access_key, token, expiry);

  aws::S3 s3_handler("io.infinit.buffer.us0",
                     "testing",
                     credentials);
  s3_handler.delete_object("test_object");
}

ELLE_TEST_SUITE()
{
  auto timeout = 3;
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

// Python code for testing signing:
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
