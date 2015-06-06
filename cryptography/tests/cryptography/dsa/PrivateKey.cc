#include "../cryptography.hh"
#include "../Sample.hh"

#include <cryptography/rsa/PrivateKey.hh>
#include <cryptography/rsa/PublicKey.hh>
#include <cryptography/rsa/KeyPair.hh>
#include <cryptography/Exception.hh>

#include <elle/printf.hh>
#include <elle/serialize/insert.hh>
#include <elle/serialize/extract.hh>

/*----------.
| Represent |
`----------*/

static elle::String const _input1("chiche, donne nous tout!");
static Sample const _input2(3240913, "Dave");

static
void
_test_represent()
{
  infinit::cryptography::rsa::KeyPair keypair =
    infinit::cryptography::rsa::keypair::generate(1024);

  // 1)
  {
    elle::String archive;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive) << keypair.k();
    elle::printf("[representation 1] %s\n", archive);
  }

  // 2)
  {
    infinit::cryptography::Code code =
      keypair.K().encrypt(
        infinit::cryptography::Plain{_input1});
    elle::String archive;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive) << code;
    elle::printf("[representation 2] %s\n", archive);
  }

  // 3)
  {
    infinit::cryptography::Code code = keypair.K().encrypt(_input2);
    elle::String archive;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive) << code;
    elle::printf("[representation 3] %s\n", archive);
  }
}

static
void
test_represent()
{
  // WARNING: To uncomment only if one wants to update the representations.
  return;

  // These generate base64-based representations which can be used in
  // other tests.

  _test_represent();
}

/*---------.
| Generate |
`---------*/

static
infinit::cryptography::rsa::PrivateKey
_test_generate(elle::Natural32 const length = 1024)
{
  infinit::cryptography::rsa::KeyPair keypair =
    infinit::cryptography::rsa::keypair::generate(length);

  infinit::cryptography::rsa::PrivateKey k(keypair.k());

  return (k);
}

static
void
test_generate()
{
  _test_generate();
}

/*----------.
| Construct |
`----------*/

static
void
test_construct()
{
  infinit::cryptography::rsa::PrivateKey k1 = _test_generate(2048);

  // PrivateKey copy.
  infinit::cryptography::rsa::PrivateKey k2(k1);

  BOOST_CHECK_EQUAL(k1, k2);

  // PrivateKey move.
  infinit::cryptography::rsa::PrivateKey k3(std::move(k1));

  BOOST_CHECK_EQUAL(k2, k3);
}

/*--------.
| Operate |
`--------*/

static
void
test_operate()
{
  // Construct a private key from [representation 1]
  elle::String representation("AAAAAGECAAAAAAAAMIICXQIBAAKBgQDLZNdiD3/FlwPR+fNPYfD+nxMvLLeXo3Ym8lD+OF7szsza5Tfx+/jnQvBU5TpX/JEqoDMWKbzmLfZ/B/3jRNqt7SbMS8S8Qx7gpMo1z8VjudBZX+Ci2XjbJSmKDO9FXBoCZ+Y6/51yy+IGO9Vnjl60b+h5fPvgb0DgfHbleNdc7wIDAQABAoGAXZ0oVQX1NShwpbB5gEv/8/GbjtbyGWUu+HYHOTsQ1cXJG+bsfQAQa8CvhSOMWvrwhcH+QXEsdHUrQwSWoogKbbgI9XS3DFvwKmbmw36JzZaScmx5RIL01YmEDURvkHE9BHwmG5rLpFIDP905On/VC1pnQi6aGUOfT9KABS/ZR4ECQQD6Vt65zzcjiBjknRwm/N1g+yXlkBIW+c5Nm0GWW1W95lb3wlfmNu/BzCQ/uUuntUjjf/GVCkvBZrgkgpIU7t+/AkEAz/44V8Sxq+P91l48eokTS321kyIVJ3ILQq5xIOL+LLxuOGjwXXjZ5cyKKG+w0iCVaSX9Ao5rp9aqan2/t/nO0QJBAIBx9hFsSqRGfk8+dWVjtMq/0e/5fe6f+cRD9uEIt/OZreGtq6AJa/cJoUe48SJ5l45y5SdNgxUWeUDyZbWr4s0CQQDB0CWWe54IC5qEgCmhEUxegxSItzC5IgcmKsEWIQ8yhp+6hua8YxnAjfmLtpAxLdWsNo7uK+Jbcqg4NVdKS0iRAkB3iACbdSenm02+2QpmLc2rrdks6Dkl9UH3T+iyrv8vJJonRdrq4nEaEPEDLKtV/pZLWwYDrBvfP73TP9PqW/6MAwAFAAQA");
  auto extractor =
    elle::serialize::from_string<
      elle::serialize::InputBase64Archive>(representation);
  infinit::cryptography::rsa::PrivateKey k(extractor);

  // The following operations are based on hard-coded base64 string which
  // represent the data on which to operate, in their serialized form.

  // Decrypt plain from [representation 2]
  {
    elle::String archive("AAAAAMgAAAAAAAAAAAAAAIAAAAAAAAAAUiI7plqRg/54OcdcDxT2nvIa+28whX5S/7WsfIOg7bXUwaJPRalrfh58N7R/yhb6t+Zc9bDIHItJrYYrcezxy2pdC1B5MrNSlR4kIpC3wOKgEVnkVKQFsvFkfYmMOYukmduSx988J7z1ihV+JJY7y6Y1nrJdoTdk4ns1sTG205AAAAAAMAAAAAAAAABTYWx0ZWRfX57lXQIK80fmpLdRdfTGgKLKgQp1YuibOTC8nKPSria+9eubpYK973E=");

    auto extractor =
      elle::serialize::from_string<
        elle::serialize::InputBase64Archive>(archive);
    infinit::cryptography::Code code(extractor);
    infinit::cryptography::Clear clear = k.decrypt(code);
    elle::String const output(clear.buffer().string());

    BOOST_CHECK_EQUAL(_input1, output);
  }

  // Decrypt complex type from [representation 3].
  {
    elle::String archive("AAAAAMgAAAAAAAAAAAAAAIAAAAAAAAAAkG2VHD70dOrJRAAjD64hJhXa2+ICHENi96ohb6WV+lVs8Bd7GUPvr8jr/cldu7zCU3cqTuyVl3rOyBSaoGj5s64OtMi9SKD8RPbOhfe3me9yP6taQ5up48quVJfGb5Jo9YrBOMkNJbUnDLzaApKs9j+xeEpsIUIemdgUbRMs+1IAAAAAMAAAAAAAAABTYWx0ZWRfX2TtURewlP2LXzYF2NLa1nZMh6vcGESeOeOUgI1EH5BEBn1C1O0KH8A=");

    auto extractor =
      elle::serialize::from_string<
        elle::serialize::InputBase64Archive>(archive);
    infinit::cryptography::Code code(extractor);
    Sample output = k.decrypt<Sample>(code);

    BOOST_CHECK_EQUAL(_input2, output);
  }
}

/*--------.
| Compare |
`--------*/

static
void
test_compare()
{
  infinit::cryptography::rsa::PrivateKey k1 = _test_generate(1024);
  infinit::cryptography::rsa::PrivateKey k2 = _test_generate(1024);

  // With high probabilituy, this should not be the case. Otherwise,
  // the random generator is probably broken.
  BOOST_CHECK(k1 != k2);
  BOOST_CHECK(!(k1 == k2));
}

/*----------.
| Serialize |
`----------*/

static
void
test_serialize()
{
  // Serialize/deserialize.
  {
    infinit::cryptography::rsa::PrivateKey k1 = _test_generate(2048);

    elle::String archive;
    elle::serialize::to_string(archive) << k1;

    auto extractor = elle::serialize::from_string(archive);
    infinit::cryptography::rsa::PrivateKey k2(extractor);

    BOOST_CHECK_EQUAL(k1, k2);
  }

  // For every hard-coded strings [representation 1] in every format,
  // deserialize the public keys, re-serialize them, make sure they can
  // be used and finally upgrade them: useful for detecting changes in formats.
  {
    std::vector<elle::String> const archives{
      // format 0
      "AAAAAGECAAAAAAAAMIICXQIBAAKBgQDLZNdiD3/FlwPR+fNPYfD+nxMvLLeXo3Ym8lD+OF7szsza5Tfx+/jnQvBU5TpX/JEqoDMWKbzmLfZ/B/3jRNqt7SbMS8S8Qx7gpMo1z8VjudBZX+Ci2XjbJSmKDO9FXBoCZ+Y6/51yy+IGO9Vnjl60b+h5fPvgb0DgfHbleNdc7wIDAQABAoGAXZ0oVQX1NShwpbB5gEv/8/GbjtbyGWUu+HYHOTsQ1cXJG+bsfQAQa8CvhSOMWvrwhcH+QXEsdHUrQwSWoogKbbgI9XS3DFvwKmbmw36JzZaScmx5RIL01YmEDURvkHE9BHwmG5rLpFIDP905On/VC1pnQi6aGUOfT9KABS/ZR4ECQQD6Vt65zzcjiBjknRwm/N1g+yXlkBIW+c5Nm0GWW1W95lb3wlfmNu/BzCQ/uUuntUjjf/GVCkvBZrgkgpIU7t+/AkEAz/44V8Sxq+P91l48eokTS321kyIVJ3ILQq5xIOL+LLxuOGjwXXjZ5cyKKG+w0iCVaSX9Ao5rp9aqan2/t/nO0QJBAIBx9hFsSqRGfk8+dWVjtMq/0e/5fe6f+cRD9uEIt/OZreGtq6AJa/cJoUe48SJ5l45y5SdNgxUWeUDyZbWr4s0CQQDB0CWWe54IC5qEgCmhEUxegxSItzC5IgcmKsEWIQ8yhp+6hua8YxnAjfmLtpAxLdWsNo7uK+Jbcqg4NVdKS0iRAkB3iACbdSenm02+2QpmLc2rrdks6Dkl9UH3T+iyrv8vJJonRdrq4nEaEPEDLKtV/pZLWwYDrBvfP73TP9PqW/6MAwAFAAQA"
    };

    infinit::cryptography::test::formats<
      infinit::cryptography::rsa::PrivateKey>(archives);
  }
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("rsa/PrivateyKey");

  suite->add(BOOST_TEST_CASE(test_represent));
  suite->add(BOOST_TEST_CASE(test_generate));
  suite->add(BOOST_TEST_CASE(test_construct));
  suite->add(BOOST_TEST_CASE(test_operate));
  suite->add(BOOST_TEST_CASE(test_compare));
  suite->add(BOOST_TEST_CASE(test_serialize));

  boost::unit_test::framework::master_test_suite().add(suite);
}
