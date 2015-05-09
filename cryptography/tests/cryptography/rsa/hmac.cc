#include "../cryptography.hh"
#include "../Sample.hh"

#include <cryptography/Exception.hh>
#include <cryptography/Oneway.hh>
#include <cryptography/Plain.hh>
#include <cryptography/random.hh>
#include <cryptography/rsa/hmac.hh>
#include <cryptography/rsa/KeyPair.hh>

#include <elle/serialize/insert.hh>
#include <elle/serialize/extract.hh>
#include <elle/serialize/HexadecimalArchive.hh>
#include <elle/format/hexadecimal.hh>

static std::string const _input(
  "- Suck it Gill!");

/*----------.
| Represent |
`----------*/

template <elle::Natural32 N,
          infinit::cryptography::Oneway O>
void
test_represent_n(infinit::cryptography::rsa::PrivateKey const& k)
{
  // N)
  {
    infinit::cryptography::Digest digest =
      infinit::cryptography::rsa::hmac::sign(
        infinit::cryptography::Plain(_input),
        k,
        O);
    std::string representation =
      elle::format::hexadecimal::encode(digest.buffer());
    elle::printf("[representation %s] %s\n", N, representation);
  }
}

static
void
test_represent()
{
  // WARNING: To uncomment only if one wants to update the representations.
  return;

  // These generate hexadecimal-based representations which can be used in
  // other tests.

  // 0)
  infinit::cryptography::rsa::KeyPair keypair =
    infinit::cryptography::rsa::keypair::generate(2048);
  elle::String archive;
  elle::serialize::to_string<
    elle::serialize::OutputBase64Archive>(archive) << keypair;
  elle::printf("[representation 0] %s\n", archive);

  // MD5.
  test_represent_n<1, infinit::cryptography::Oneway::md5>(keypair.k());
  // SHA.
  test_represent_n<2, infinit::cryptography::Oneway::sha>(keypair.k());
  // SHA-1.
  test_represent_n<3, infinit::cryptography::Oneway::sha1>(keypair.k());
  // SHA-224.
  test_represent_n<4, infinit::cryptography::Oneway::sha224>(keypair.k());
  // SHA-256.
  test_represent_n<5, infinit::cryptography::Oneway::sha256>(keypair.k());
  // SHA-384.
  test_represent_n<6, infinit::cryptography::Oneway::sha384>(keypair.k());
  // SHA-512.
  test_represent_n<7, infinit::cryptography::Oneway::sha512>(keypair.k());
}

/*--------.
| Operate |
`--------*/

template <infinit::cryptography::Oneway O,
          elle::Natural32 S>
void
test_operate_x(infinit::cryptography::rsa::KeyPair const& keypair,
               elle::String const& R)
{
  // Verify a HMAC digest.
  {
    elle::Buffer buffer = elle::format::hexadecimal::decode(R);

    BOOST_CHECK_EQUAL(
      infinit::cryptography::rsa::hmac::verify(
        infinit::cryptography::Digest(buffer),
        infinit::cryptography::Plain(_input),
        keypair.K(),
        O),
      true);
  }

  // HMAC a complex type.
  {
    Sample const input(
      42,
      infinit::cryptography::random::generate<elle::String>(S));

    infinit::cryptography::Digest digest =
      infinit::cryptography::rsa::hmac::sign(input, keypair.k(), O);

    BOOST_CHECK_EQUAL(
      infinit::cryptography::rsa::hmac::verify(
        digest,
        input,
        keypair.K(),
        O),
      true);
  }

  // Try to verify an invalid HMAC.
  {
    elle::Buffer buffer =
      infinit::cryptography::random::generate<elle::Buffer>(16);

    BOOST_CHECK_EQUAL(
      infinit::cryptography::rsa::hmac::verify(
        infinit::cryptography::Digest(buffer),
        infinit::cryptography::Plain(_input),
        keypair.K(),
        O),
      false);
  }
}

static
void
test_operate()
{
  // Extract the key pair from [representation 0]
  elle::String representation("AAAAAAAADgEAAAAAAAAwggEKAoIBAQDNxNfqKFC+UpD8LOfN4aLhOEDB0RwXJbIyQmOa3D+suHDweKXH2VNc+sgBPjKynbcdXaT99FmVW7/NsoKRZxFkHUo4FtFXgV735EtYrqocx0eVAyocMKU3HLnjuA627O8M/9IyueteMOB2ndUrDt7gqTSciK9mMFkyuChI5/vGREBtnx99ETtSW4K4HdI3E56M9EVNDX/VkZ1CmsRCY02gP97qpEEJY9eAEb6N/ckZ1V1uLUAhE/IWl4gcr6hrNf/CbE/7zAi7F6zbxuG+UHOMi5z2spnWc2hzxmn4Q344nIIM+6SHhtDbclz747PbXh6HY07hLKZG2HfvySXL97qtAgMBAAEDAAUABAALAAEAAAAAAKcEAAAAAAAAMIIEowIBAAKCAQEAzcTX6ihQvlKQ/CznzeGi4ThAwdEcFyWyMkJjmtw/rLhw8Hilx9lTXPrIAT4ysp23HV2k/fRZlVu/zbKCkWcRZB1KOBbRV4Fe9+RLWK6qHMdHlQMqHDClNxy547gOtuzvDP/SMrnrXjDgdp3VKw7e4Kk0nIivZjBZMrgoSOf7xkRAbZ8ffRE7UluCuB3SNxOejPRFTQ1/1ZGdQprEQmNNoD/e6qRBCWPXgBG+jf3JGdVdbi1AIRPyFpeIHK+oazX/wmxP+8wIuxes28bhvlBzjIuc9rKZ1nNoc8Zp+EN+OJyCDPukh4bQ23Jc++Oz214eh2NO4SymRth378kly/e6rQIDAQABAoIBADuMWcfj/un1TwwLmtHUGKUA+IyejXiNajJtUBIC51J+1gBhieh6Fkv0L2lcrKNQTSvsa9Eqo7o7oRhQLsURhNDqQQ5WXCLsx6UJplx1v7kwWqDfF5Ks6UM3xvwFygxaKRwj9JrFfz30+yCYsJYTaaCulxEt5US3fTjWXqIqsIxBsyYk7Xni3kC7gR60Cc02Y7nTusRaUP+mf7o6/lxlX/w2CziCRe0vsQIefRnhqaqmFqC5U5FRbLHDQuU25R3nWLvvoD6PoBD5nWsY4Afd/7QS9pmJ4fe8S4aMmQcVr4T0Z84XHtyy2X8yQvkmc5a3YanJXL9fqIGjO43Uo7byFukCgYEA88Zxh3uGUxogswxK8YVE+DNgPtGxgvd/sByhntKb5QE/wn3cGVbzhuV1N+b/2+P9h+X/JFZ8LJaYO9PM4wYooXKv1fqrkwabFjXPBAVxVP6UL+6LQ+zA9mrrnL3571REmoTfIxyGkVKbXdq8XvqpZ9LIxDPXhynZQO43Gnp6PLMCgYEA2BZ68QbbKdMUiIjnFzV9U39MbL2JwRKOcFZkzAkCjRIi+FnrMIgrLSJaPDF4K0vSJdVHxwsSd3dpyXv258bFM0pAUi7pyPqEjACdoZ6oijZbRhjpVUwQ1RIBKxGmMv1zS3aGUNQEnmx4e4GrwyqUecmXrEbZ+Ah07UsoeGLumx8CgYA7fgCS6eh8O3IjgCqCCt4sHI2uhwplRfWZi0M+OXPmTYm7829Vl8zS7QjQXcWvalKjadP8kEa5sv7wCt6DwSsFS7Hx7L0WopQStYsSFin0bjTIBdVi/FKTUMCzfgGTEOO1Ohwiu4Fz0c8s9obmPxI8zkpE58n/qIzmtbwo8B8I1QKBgQDUyY5XOwEmto1JxkaKPEolpv2DN0IntjY9uVWY9mZktK9rEGDhcbXSxDlHfQ9w00ElsFj2UJjmu9iYaFJC+yHF2CBuzXO6TbeohhKzzKD7OXu4E1/ydu08ZHZyCvc2zw5yCU1K2v2RZA9gz9hsX8nGBBYvt7V84hcCdThtFD8S0QKBgEvb4eYtXw3uXDu4P+cUd8kZZeQYrhdmqGDxDIfCEmHKmOY/KCZAs1l4rAxI3me3BRqIcvb0Xy+Ucesn4AXjPlqDqfQ4/WV5nN8BxOL/Kda7/Ur3rsrTNDxyNgUWQJom++2Y6js8L4Z5IOQYH640axgi+wDZ2ZKzdmYM/vIHQH4gAwAFAAQA");
  auto extractor =
    elle::serialize::from_string<
      elle::serialize::InputBase64Archive>(representation);
  infinit::cryptography::rsa::KeyPair keypair(extractor);

  // MD5 based on [representation 1].
  test_operate_x<infinit::cryptography::Oneway::md5, 820>(keypair,
                                                          "7b4d53e1483757c4d11caa97e04f78fdb54eceac0093748027f44f9e335b5de75a0f090ef4e8a1ca10baef7211d86c36a1ebf0ba5dde49c2416bdceaf7b2e054546bbf0280facc459a7d489faca49edb706affd5d4f9b3e38c998f181e4d1e7041d5849ece29153a1874d8e78e98cde6ce0b578aee2096d0bfd752a70817f8f49dc2368138a461fe2074fcc6affb656887ef5b18a4e3a9b8b04c4941ccd6a9e5c1d4eb6deb1d8a1435dcd10ba809674185bd10687bb0676395a39447f9282d8a50ff9bd9a24808956860c04d81a32f7efa8cc3a06baa69d90961a437ac391b856fbd84c040d0d3c8bc6a4770e743457ab7054c6e2505793690dad8e4b24844e1");
  // SHA based on [representation 2].
  test_operate_x<infinit::cryptography::Oneway::sha, 31>(keypair,
                                                         "65708910a5fef6e34704137f3f19af3bbfcc62bd3583d4d6b65a60a607bf3c87b82a8735078d5d6e745a36e43e5f0a63a6dc292a56ea163845e1efbcdceb1acf19a81832bfb6249e2cf6a744a977d4d289314bc09fc3f0749d604968916d610df7164981eafe11844f0132ad11e4d8850b009520d322f49d64d1092473096fd8754e67bab4c5aa5cacf56fc4d03c3dacbca8360a32e78ff5e5c63a6332b6c4020fb8246601782a42a3847278eb6f415cf95ca0988ec8d9b8187562b8570a223892fcaf375d761b82c8c4e5d4a582d96823ede9236a17507fc84adc53984791a7cc6ca4c8602ae8b0b8ee0da8d44954d8da62affc5d44ac477f374ee8de9c669d");
  // SHA-1 based on [representation 3].
  test_operate_x<infinit::cryptography::Oneway::sha1, 9028>(keypair,
                                                            "755a445ee3d32233a10e47e5383f1412c0837e99cb041598c5533ea2b2fc26b31e2c6dfe82f71bace7dc3af6a39e4d32984e4a321612cd588c3f19a8ad6e5bb43e7aeb1f1965d6b7b4a8c1aa4e87b58e0eb8f259647f158f54d415ca868cc491f1061aad820b57cae6f9c8838ddcd8080eeb0a7f7c5dca06bf5332d9fdc3073f6cdd005fa358e054efc53c3882a7648a862ec59141921a4d355283ccc8f4d5278caff006d225ff3eecbcec72881e31afcc42487c0b2106785ac00ef74900d7a1b2fa50c85876d2aa03c301d75dd02d3a5edb5c48aeafc0d9888510fef24691d1011b3d741ca043f5410a369d5c3727ab8d1f6f9eb09bc34ba72cecf5ee48750a");
  // SHA-224 based on [representation 4].
  test_operate_x<infinit::cryptography::Oneway::sha224, 630>(keypair,
                                                             "3ce8a744cb68229d7cc8c2d4c16c16f836bb300b139528b5d9cb4e8e2909cc61a686d646cc5bb0878a37c2619381202fc01ea5ee2734a971ec98d20f926e93ece2c00111f9223bea67cf5d7c026370fedd8c66051a1cf747eb5bb0fe8d98f0631edb8ff5e6d612e6c91b6932a347ed1a7e430ef6fce26778c2372c8e8f56067bd56843db8b6dceff5524898bc7f7900fb472e398cecaad8a1c578965c288a002c6ecb2a9d4a1d4e11d78a74e54d63df586953b45e67a39bd339f1cb20b94c436a38656aac15799d6951a2cce46638c3abc8458dea7ea1d01f6a7804f7d6d27490ed95b3446a2c94a60a7b48bb10ec7960a61c3bbd4b018512d110bda40cbbde0");
  // SHA-256 based on [representation 5].
  test_operate_x<infinit::cryptography::Oneway::sha256, 73>(keypair,
                                                            "a6d07bc4bae6f0ff4c2ab1e434dff6669c8547a2beef936d0b03eb26da73f16077ee332248bc62249aac664522103e1490791077042094f4455525b804ff84bbed301526f720a4b60733c4efab45b37764d2822c8125a541800d9cb2629b968da52fdd6feb634852ca08d518c490bedae5bff16f14e3c0ce1d56106313442f9ac7125e80accf818cc00765c21586b486b5d0c28a50e3d47c263a2472b9db4aca2dd934195e4db534424a2a528fb3aa00b96c38bb0559d607f7bda1c0f346d219aaec8f575cf54db7b5407816526d14420d28b8a45225297b86a99e70cb843bea796f031959b9b5e85a28034fa4cc5ee61230fdbbd93100e0cc9acc6b59e44f81");
  // SHA-384 based on [representation 6].
  test_operate_x<infinit::cryptography::Oneway::sha384, 933>(keypair,
                                                             "8dd7c85f125815912a108266da6da800bddbd1c9a29d12eb13d98d9707ba32d30985a60a2f5e5cdb290f1af0d51b667ed044a161780930286d9a5e3c3ff1381b788af3e5993fffcec373ed00bfd03610687bf9ca99655124be56cad8efec792796453c6b6590d249c6448e789edcf34abdc17885baad77036a231abd35a20e2e8478668a3e16c06205ed3dc2294b9a1b5377ef77c731a43c70e5487445902e7b87b7ecb8b6c8c3ac3be865b7bab8fa1aa8d195784ab719df9fe1f0e5ab4f2eea1803ee6c6e67b2a524e926be0e6dfc9bf9ab66153fe6e2b56cdd85c1c09b2b841d83fce912c1314fd1689b5208699c274f99d173286449bbb198e019d9cd1f24");
  // SHA-512 based on [representation 7].
  test_operate_x<infinit::cryptography::Oneway::sha512, 9223>(keypair,
                                                              "1339fc0aec6480a53c1294e2cd04c6064ac8a4cc2981a7f544eecfe3cdf2026eb10ad6a048e893e4b8a4d7a57f34d2634d3c07d4df55f5d5442765521c3863e0f70c9b51b153d3fe42d63ea1e8ce32894776d2f590381d76207e6a03c6a7dca1433b7c507cfdc806513b790a03cfa30a8940106f964bb44ab5da2a525c9a8ea16c57b1b60229a4accd214f0b452c9bb8feae6d3a50fa850c06d2f41c708e958169c47fcd84e3f196b6588519d6f446c4eeb9b45b972f94375da6b19068e26df394f3615dc4710a959d2ce4568693dea74bd77e5d73df1e223a1053949237cc22c73cf4541d2d0ac7b94a6c93df21e964a8ea7a6a832e4a8816045af0cf0b6066");
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("rsa/hmac");

  suite->add(BOOST_TEST_CASE(test_represent));
  suite->add(BOOST_TEST_CASE(test_operate));

  boost::unit_test::framework::master_test_suite().add(suite);
}
