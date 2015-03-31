#include "../cryptography.hh"
#include "../Sample.hh"

#include <cryptography/rsa/PrivateKey.hh>
#include <cryptography/rsa/PublicKey.hh>
#include <cryptography/rsa/keypair.hh>
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
  elle::String representation("AAAAAGECAAAAAAAAMIICXQIBAAKBgQC3479dATedYhK6GitnUyPFgmtLye1WJP9p8yiuolyhcWxBS4rke++VWjFLKPjcYdBVzI7EVc3x1vhFVRzUlUJHo9v6QORmk+rv+oruxbXjstwYtMfHG+uuWkJJ1jpqSfhm+v2lYsobFYxV7yis+nAllouRwmE+V03tOZuoFNziPwIDAQABAoGAB82dSDgOARqxSr5jix3nDbGMFgo6jQZC5F8SId736U3j1x16hlmQABl0sAjMDqYD7jAg+sndVF7X6DFFaOvwhQFPPLbi4Qi6uNYw04MpTV2STQZqPHKd8KTIhTakpGiAyamP7GqRXFc1QGPYsdmX1v/JkCF6sXWE6+Ivwgb0TfECQQDdBUjyZS4LhYGtEqJhHBILbdeNjk0kLD/AAu4nq8HvgtC3tf2hgTjd9H7DUDNMw2NFzhbjEKCb4wV9dlEMWSTHAkEA1P4W1HjI0PTjxUMedaXUVc9oZOBKHRXwJZnYxGMcmmsT4hwduNYaCoZLnAz5PNidKQNyrPpeMiGYAcVmneLuyQJBAK32He9jwfUhlOqjkxk6Hd3PC+0IByGkjlzUMZ0Z7GBXJ26hmMsIybixWyDhvC78TuO/jXYVPUTVUBnrCWndtDsCQQC5OmdcS9ZIclAK4v4ATkDEh2VAJRYPCV8ZNBwhXhgn6msWPmFSt09aS8RBZ77wvWifnFIEQI/ehPyOvb8qG4qRAkA+ZOppvEetYTdKXPqb2Yc5AtKlCWS7rRHKWTq3fEQe4Qq1WyRhewL4R+ZpXUB9sj1YxjEma1e2qqPAkaFBOF+nAwAFAAQA");
  auto extractor =
    elle::serialize::from_string<
      elle::serialize::InputBase64Archive>(representation);
  infinit::cryptography::rsa::PrivateKey k(extractor);

  // The following operations are based on hard-coded base64 string which
  // represent the data on which to operate, in their serialized form.

  // Decrypt plain from [representation 2]
  {
    elle::String archive("AAAAAMgAAAAAAAAAAAAAAIAAAAAAAAAAB3PUB9sqd9DOlosmQlrtSEk8V1zkroCdF7+2Fj1txehYgD6PhngMrgKd5a/FQN4T0OCsPZbsKmLafX0utLcsWQK0LHxpwpHFkhoXrMAUum67W7GYrvR20A/oSUVgove6fWM55vRSobBMfrE47gLcE7rqomDfx+TgalrRTLoKqRkAAAAAMAAAAAAAAABTYWx0ZWRfX89eIJ611X9a5KdSTtO4BY1Yn/gSfr1ku149sPFmn6i1gmsPNPemwZI=");

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
    elle::String archive("AAAAAMgAAAAAAAAAAAAAAIAAAAAAAAAAILDegcxjIsUCW+j9w3zYuRnqkNY7V6HYehSImp2uAAz17QcqD8+SdCUkVRt51BDJLzxpelS04pN30AmYLgOE/vycIwPy9UD+c0k2uRa6kPXHw5IApHucryxM0YO8u92zwwlDG4UsNmuCIsrwFIVNNeAxjMM5HLiOVW04amdd1dwAAAAAMAAAAAAAAABTYWx0ZWRfX/8z5hJnrtuDeBe0NBfVIMznuQYsex2NoaI6w0AWLPxc8shR7xCRwOM=");

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
      "AAAAAGECAAAAAAAAMIICXQIBAAKBgQC3479dATedYhK6GitnUyPFgmtLye1WJP9p8yiuolyhcWxBS4rke++VWjFLKPjcYdBVzI7EVc3x1vhFVRzUlUJHo9v6QORmk+rv+oruxbXjstwYtMfHG+uuWkJJ1jpqSfhm+v2lYsobFYxV7yis+nAllouRwmE+V03tOZuoFNziPwIDAQABAoGAB82dSDgOARqxSr5jix3nDbGMFgo6jQZC5F8SId736U3j1x16hlmQABl0sAjMDqYD7jAg+sndVF7X6DFFaOvwhQFPPLbi4Qi6uNYw04MpTV2STQZqPHKd8KTIhTakpGiAyamP7GqRXFc1QGPYsdmX1v/JkCF6sXWE6+Ivwgb0TfECQQDdBUjyZS4LhYGtEqJhHBILbdeNjk0kLD/AAu4nq8HvgtC3tf2hgTjd9H7DUDNMw2NFzhbjEKCb4wV9dlEMWSTHAkEA1P4W1HjI0PTjxUMedaXUVc9oZOBKHRXwJZnYxGMcmmsT4hwduNYaCoZLnAz5PNidKQNyrPpeMiGYAcVmneLuyQJBAK32He9jwfUhlOqjkxk6Hd3PC+0IByGkjlzUMZ0Z7GBXJ26hmMsIybixWyDhvC78TuO/jXYVPUTVUBnrCWndtDsCQQC5OmdcS9ZIclAK4v4ATkDEh2VAJRYPCV8ZNBwhXhgn6msWPmFSt09aS8RBZ77wvWifnFIEQI/ehPyOvb8qG4qRAkA+ZOppvEetYTdKXPqb2Yc5AtKlCWS7rRHKWTq3fEQe4Qq1WyRhewL4R+ZpXUB9sj1YxjEma1e2qqPAkaFBOF+nAwAFAAQA"
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
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("PrivateyKey");

  suite->add(BOOST_TEST_CASE(test_represent));
  suite->add(BOOST_TEST_CASE(test_generate));
  suite->add(BOOST_TEST_CASE(test_construct));
  suite->add(BOOST_TEST_CASE(test_operate));
  suite->add(BOOST_TEST_CASE(test_compare));
  suite->add(BOOST_TEST_CASE(test_serialize));

  boost::unit_test::framework::master_test_suite().add(suite);
}
