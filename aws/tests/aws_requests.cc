#include <aws/CanonicalRequest.hh>
#include <aws/SigningKey.hh>
#include <aws/StringToSign.hh>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <elle/format/hexadecimal.hh>
#include <elle/json/json.hh>
#include <elle/test.hh>

#include <reactor/scheduler.hh>
#include <reactor/Thread.hh>

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
  aws::CredentialScope credential_scope(request_time, "iam");
  aws::StringToSign string_to_sign(request_time, credential_scope,
                                   canonical_request.sha256_hash());
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
  std::string expected = "AWS4-HMAC-SHA256\n20110909T233600Z\n20110909/us-east-1/iam/aws4_request\nd030ae173c82eeffc6a53f427a3ddafcef9aad71aef5ac9f82122e0742b8c1b8";
  BOOST_CHECK_EQUAL(_make_string_to_sign().string_to_sign(), expected);
}

// http://docs.aws.amazon.com/general/latest/gr/sigv4-calculate-signature.html
ELLE_TEST_SCHEDULED(signing_key)
{
  std::string aws_secret("wJalrXUtnFEMI/K7MDENG+bPxRfiCYEXAMPLEKEY");
  boost::posix_time::ptime request_time(
    boost::gregorian::date(2011, boost::gregorian::Sep, 9));
  std::string aws_region("us-east-1");
  std::string aws_service("iam");
  aws::SigningKey signing_key(aws_secret, request_time,
                              aws_region, aws_service);
  BOOST_CHECK_EQUAL(
    elle::format::hexadecimal::encode(signing_key.key().buffer()),
    "98f1d889fec4f4421adc522bab0ce1f82e6929c262ed15e5a94c90efd1e3b0e7");
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(canonical_request), 0, 3);
  suite.add(BOOST_TEST_CASE(string_to_sign), 0, 3);
  suite.add(BOOST_TEST_CASE(signing_key), 0, 3);
}
