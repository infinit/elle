#include "../cryptography.hh"
#include "../Sample.hh"

#include <cryptography/Exception.hh>
#include <cryptography/Oneway.hh>
#include <cryptography/Plain.hh>
#include <cryptography/random.hh>
#include <cryptography/hmac.hh>
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
      infinit::cryptography::hmac::sign(
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
      infinit::cryptography::hmac::verify(
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
      infinit::cryptography::hmac::sign(input, keypair.k(), O);

    BOOST_CHECK_EQUAL(
      infinit::cryptography::hmac::verify(
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
      infinit::cryptography::hmac::verify(
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
  elle::String representation("AgACAAAADgEAAAAAAAAwggEKAoIBAQDG9Q7jLalUjR0UXfq5SRgDGJ8PXKj6HW51FAsFOx5BgDjeowA20cG4datLpOWqFSk6pB2n9GaArz/AYJW49wdofZO1YroXv1fmxtD1MxxrV6k0tyKGJBK7rVzh1yyoVecKl8YyhdAKFlJ5rb/AaeUz9nPhffdAsc/ESLQqHH6tSTkQOrlqevAFJow2hKm1L5jq6HO5k81nEXQd7nS1vEUxxLT688IzojlXqRnQFl+Kv8mIw8Kx1n145aSrb2rPitvwPbJgNr0fJSMQDxwWarpRxIeX+/NtJcqoEz6jjcJku4+Oa44aYu2jnYve5UEvmXuKBeVRLVnOlmiMTrETKcJTAgMBAAEDAAUABAALAAEAAgAAAKkEAAAAAAAAMIIEpQIBAAKCAQEAxvUO4y2pVI0dFF36uUkYAxifD1yo+h1udRQLBTseQYA43qMANtHBuHWrS6TlqhUpOqQdp/RmgK8/wGCVuPcHaH2TtWK6F79X5sbQ9TMca1epNLcihiQSu61c4dcsqFXnCpfGMoXQChZSea2/wGnlM/Zz4X33QLHPxEi0Khx+rUk5EDq5anrwBSaMNoSptS+Y6uhzuZPNZxF0He50tbxFMcS0+vPCM6I5V6kZ0BZfir/JiMPCsdZ9eOWkq29qz4rb8D2yYDa9HyUjEA8cFmq6UcSHl/vzbSXKqBM+o43CZLuPjmuOGmLto52L3uVBL5l7igXlUS1ZzpZojE6xEynCUwIDAQABAoIBAGQe5kMEs3XNHZkmQgoD8oV8Wm8MnixfqzpXv8P+IBeuQvAm7vCnVelRrcX/UWPqCjlsZVAj+YFAr8BL2oyeI6g9qZtZ93QD4mRMINODyQVrs8bHciQ2+wZfE6LC+cIsmHwILME3/Z1V5mM9QIW/xIwFtgL/tPFlibypKaEXfdgNYJjcu/UKwQZp82ZSNhUHz/J0UaENYosCSUKbqqqGXOkq2i5ah1i3v1HFFUjWQhXXM31LudEOmz30fHA1havotKAUr/Jb7fUY1EyZjqSwn9Kw6Y1AiCbrWxIpVFi06hRMa+YsXdX0NKxdkPKDdvgLSBKYvuZRbfgl9finwaH2WWECgYEA++78JObUIhWzDqCweEAp6URhBC7rBMLfHJ8+JOLVy8o22NswYQpAodumoST2evAxr7xIHc7Pw4zlcqLZOBNv9NGvW3xGetyXrBW3BGh2zDHyJtWTd8wlh4zUrLaP/qgiO+tXKKmY8S4hbELllmqhoUOMKZ2oEgsU8/5DfH5b71UCgYEAyisreoMqMDJetGKF8j86/Wk0eMicJmApXdJrxsl8AtzUGRheNVN9qOwYBtvIN/hISsJl6qXrFxiO8NMys8OJ1lDQeUzavUTEOX1TcOWxE9LAjct9RmJJHnTpQjT0Dj7Eu1oct6QBE/VrwBoGHSCLrTVccTi/9f/FxygAwVBHWwcCgYEAoLdEgwX+LgBJyfsYmJ3WHslQWqrToqXuTwQHUOCkIu1NL+okYsN3l8nX8tTxqFuyVrl2CA323Hj1prvcdrlvom5hv6r4C0jOOSlhJ568qmbyY3O8L6A8EMz2n3eXRI1dyXvIjlgjCTi0eavzapF7qrS8pY+9uET5iO2bf7/cRkECgYEAqU2bvJ5JX1FfVArKB4OCQCOxaJD/i3euTL7huPiUnMUX7eglRN0ld6g3us6ljpGi4NCJjwbhqFbNpcoIRNi0/sBwxZRh/myvM0HL9di1AVfWHyjRQXcfqpzXlrVHyQIxuIAPWo7CxFcbpCVnB09xCV2Ni9etkjJfC5hpgXpcN1MCgYEApb9nucDRkvopV+661bw5Mxj1tfu31S3J+yuE1Ag0OLLuK5xRFpTYihg8e4U4BEaon361IybNsTEDpVZReDiFenrhTPlTqIdA+ZSl5wOOVbDoHttGtEpVGKbyegqWxSTIJcfEb8nzMz49NAFffdFmy+L4HMus/rb/omsuWaFqyiQDAAUABAA=");
  auto extractor =
    elle::serialize::from_string<
      elle::serialize::InputBase64Archive>(representation);
  infinit::cryptography::rsa::KeyPair keypair(extractor);

  // MD5 based on [representation 1].
  test_operate_x<infinit::cryptography::Oneway::md5, 820>(keypair,
                                                          "1c4c2fc76c0c904b7fc675bc3f25c6e09f205a69f1434b1a84ba120213ffb49d5cb086247e366dac74d08c53f89fbeb781053cabbbe88f92810a076d084d76f05cbd5e487ff00951afb60ec0aece2e1281a5a2d4318d3d8d5c23cc86d8186ae3fc7652b8f1ea1cb1e46c156e0eb84adf539444c996b9b52c16411bbe85955638fbadd2aca4a5ffab207b73afae9a45fcc029357a746b0e70455b78ae816facacf7bbdc0112121d2fa58b336fcf1bd76a53ae06049f7e430a16d0e5b70a4f2bd758a8a5e977f2b9f89e863d421646ad023fe80a649acd6160d8fc37d53ab19b78e855dd13abdf069d62b65c2fd65a103425faf7b080b9fc09a3515a91f29136b6");
  // SHA based on [representation 2].
  test_operate_x<infinit::cryptography::Oneway::sha, 31>(keypair,
                                                         "0646d811820c7e3fce73f50aa4bbc18f29b2f3c643fe672436df12b5e30c6f0fc0831b7fbc09a6724c6e7046b99ba1be45acaafafa09f932db4b2ef4bdc8d438849d19f00e84e86ac5c18c979c71a99cc8995780e64307ef4caff635a3f50c43a8972e1bab718eb1183428b03960a1336c196ac655105d8181a6486414991ad8e417c0f3cd43ff3e55eabf964d4b1d92c7a6ae8dd71d9e3b40aab932c7ea6095a6483081c5ea3b9dd925d50c1eb16d8059020b693623d43c18ece33a55735bd078ec28c9255ed62a48f958fef9f0ec6f9810d8a35dfb51a5bb7e5ead1131d7ebd48d21a9824b3f83490ccf8f6b86a5e1ed753cd5dc849b601734c0d3b6f4d836");
  // SHA-1 based on [representation 3].
  test_operate_x<infinit::cryptography::Oneway::sha1, 9028>(keypair,
                                                            "c6017ba3761c43c28a3e98e2cfc1fddd2c7091616ebf7975d4601f38c47a5aa6f33769446837cdf5d9f5b3fbf3b71c5b829d071686abb07ef46a591184aae17fe38ff851b087063df8b40c54925b19f9c815ef0590d7d4e7340340900d53b18a4bea88518dd3c1c9267d600cfab9f5dce77bcbb405c0ac2bb00c45e761fb6607922a8ff49828d9808d801cd08ee9c32bd3a09843b35aa7664d14dd09411d17aa6ead218f637bc7d0365af5a09944916361af31f7ef1bbf9ef274b4aba2960d303055ed5ee866b7e7a469f047eb60fcb73f02bea54db42b1e52b481b6c1fde0f7ddea3b87f19d6920747335bf7a24717fee1ce59d6d72ccf22d4944146782fe2a");
  // SHA-224 based on [representation 4].
  test_operate_x<infinit::cryptography::Oneway::sha224, 630>(keypair,
                                                             "aae61d693b45032cc23064ae91b0a61093ee68bdae6380658702ef4da7b8d4f57c9b137aa34c663a0e13b932c33f160ad42c4896ae8860f13ba71e49ce5118888ce8793d9df0e6ab7102a182bba0ace3dd8f5f92aed38e6f883959848c4cdc51c217ff174c8c51bc6effd92f5f7dba2b76db1330ef27e8ff8c93cd6861df8db153e5c617489b737207f82627c2c4bcb1cbf18e719a96476aa7ede8cbf6a27a793cd47d068b69e8b38f69d528c1b8da88b3d9ffad2a87ef456aded226badf7b0857fe1122084f824467c1cf62fc4da0b326e205040512527701fee5aab202a6731336f332cca0d4ee46664387f852187ae6e28737f88f19e6bbfbaa47fd4117bb");
  // SHA-256 based on [representation 5].
  test_operate_x<infinit::cryptography::Oneway::sha256, 73>(keypair,
                                                            "8d41670652058a9acb2c09e2d9b87221668cc5bf45f6bf6360b7d53d7a7aa9d16d71f6f62ebb3010afc3962451701412fb4f16760ae9baf4a8b990d4da0df75fc1c436490bd14b21435f312cbe69affe574e35ed3a47bc939d03a02fd8173f56ccf110244f12cf0bec203ca0d398ade0eca04dfe748bad14152d0419c9f5e96d03c5be3d8f030df1564ef53037394d9f376a87b8d5a6f45c601016dabbf1da32bdfbfeffec2f091e18f39456854829f4a3215cfb2e9748a02e34f3c9aea9a54ef4e2a34129735a159ca7f0bf0c6934ac54243e403f60ff9ae0aff36f4c9a5f5f024a975eac51478469b9300ec7428d5e68a6c6fa77a44eafa09ae83966b3a2ec");
  // SHA-384 based on [representation 6].
  test_operate_x<infinit::cryptography::Oneway::sha384, 933>(keypair,
                                                             "809df9c03e8542ae8bbe963c7875a1a6cf6588ffe29252f5cd7062a58f6f285cefe934330ea780bd262bf6301136cbbd75bb1233dedc0dc6f7cb8666f361623ab2d74415eaff5cc6af6efb17c2437edbb9861daa6ac66e34acbad61ea8aff6fc8f2a3127b6dfb6b99458abd76e16b817eee45d8b6e1e6e745291aa3855652476744efb47171a1dcca7f4e60f814f3610d58a3b870ed1eb13b8214ef59a61f134d0aebb959b04041e1279e544e57b2e320060952ed6af5aea7f728f0b353d426278925af76c0f4632b3d7c5bf5b2db6c043d7b3e0f783548349c874f6ccc3702013e398e64aef16eaa783c24ac9caa2e7c50e1cd222d40c6947268e34fedb807d");
  // SHA-512 based on [representation 7].
  test_operate_x<infinit::cryptography::Oneway::sha512, 9223>(keypair,
                                                              "3158f2ceea235b2142d143c3554eb614e4f5c966743bbdc04fd11a4eb838a642ce1153f9623514fefcc2c5c59acd7beb723b8485d7d054baaac28045b195c53c88d93daaf398415555c0ce53a82ef1d25bbf469207516b4890499cedd531e87922cfeae4386e5c54d98921bc445835cf48485ec1fc8fe2863435a3096003036f65059ebe87f47eaef1c3f44c313adfe331387934a16146ec44d8ecb62d0b6c1cb7af73817ac65f0d6952761ef80ed86d7bf122caa388bdfb2e92e065f741a00ebc88000e69f9ec6d1bde0f43e4fcbf6b1377b80e7f9214963d8dfb2f38a04af4811887554851262e6f34eaa3fbbc6ac8b919465c177d07401a9e824ecdf3b949");
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
